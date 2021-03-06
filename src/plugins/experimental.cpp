/*
 * experimental.cpp
 *
 *  Created on: 11 янв. 2017 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <plugins/experimental.h>
#include <core/util/Color.h>
#include <core/colors.h>

namespace lsp
{
#ifndef LSP_NO_EXPERIMENTAL
    static const uint32_t colors[] =
    {
        0xff0000,   // Red
        0xffff00,   // Yellow
        0x00ff00,   // Green
        0x00ffff,   // Cyan
        0x0000ff,   // Blue
        0xff00ff    // Magenta
    };

    test_plugin::test_plugin(): plugin_t(metadata)
    {
        for (size_t i=0; i<2; ++i)
        {
            pIn[i]          = NULL;
            pOut[i]         = NULL;
        }
        pMesh           = NULL;
        pGain           = NULL;
        fGain           = 1.0f;
        nPhase          = 0;
        nColorID        = 0;
        nStatus         = STATUS_UNSPECIFIED;

        pFileName       = NULL;
        pHeadCut        = NULL;
        pTailCut        = NULL;
        pFadeIn         = NULL;
        pFadeOut        = NULL;
        pStatus         = NULL;
        pLength         = NULL;
        pData           = NULL;

        pOutFile        = NULL;
        pOutCmd         = NULL;
        pOutStatus      = NULL;
        pOutProgress    = NULL;

        nProgCurr       = 0;
        nProgLast       = 0;
        bFileSet        = false;
    }

    test_plugin::~test_plugin()
    {
    }

    void test_plugin::update_settings()
    {
        fGain           = pGain->getValue();

        // Process file
        path_t *path = pFileName->getBuffer<path_t>();
        if ((path != NULL) && (path->pending()))
        {
            const char *fname = path->get_path();
            size_t len = strlen(fname);
            if (len < 4)
                nStatus     = (len > 0) ? STATUS_BAD_ARGUMENTS : STATUS_UNSPECIFIED;
            else
            {
                fname += len - 4;
                if (fname[0] != '.')
                    nStatus     = STATUS_BAD_ARGUMENTS;
                else if ((fname[1] != 'w') && (fname[1] != 'W'))
                    nStatus     = STATUS_BAD_ARGUMENTS;
                else if ((fname[2] != 'a') && (fname[2] != 'A'))
                    nStatus     = STATUS_BAD_ARGUMENTS;
                else if ((fname[3] != 'v') && (fname[3] != 'v'))
                    nStatus     = STATUS_BAD_ARGUMENTS;
                else
                    nStatus     = STATUS_OK;
            }

            path->accept();
        }
    }

    void test_plugin::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        // Remember pointers to ports
        size_t port_id = 0;
        for (size_t i=0; i<2; ++i)
            pIn[i]      = vPorts[port_id++];
        for (size_t i=0; i<2; ++i)
            pOut[i]     = vPorts[port_id++];
        pGain           = vPorts[port_id++];
        pMesh           = vPorts[port_id++];

        pFileName       = vPorts[port_id++];
        pHeadCut        = vPorts[port_id++];
        pTailCut        = vPorts[port_id++];
        pFadeIn         = vPorts[port_id++];
        pFadeOut        = vPorts[port_id++];
        pStatus         = vPorts[port_id++];
        pLength         = vPorts[port_id++];
        pData           = vPorts[port_id++];

        pOutFile        = vPorts[port_id++];
        pOutCmd         = vPorts[port_id++];
        pOutStatus      = vPorts[port_id++];
        pOutProgress    = vPorts[port_id++];
    }

    void test_plugin::process(size_t samples)
    {
        for (size_t i=0; i<2; ++i)
        {
            // Get data buffers
            float *in = pIn[i]->getBuffer<float>();
            if (in == NULL)
                continue;
            float *out = pOut[i]->getBuffer<float>();
            if (out == NULL)
                continue;

            // dsp::scale - optimized version for
            //  *(out++) = *(in++) * fGain    x   samples times
            dsp::scale3(out, in, fGain, samples);
        }

        size_t out_status = pOutStatus->getValue();
        // We're 'loading' file?
        if (out_status == STATUS_LOADING)
        {
            nProgCurr   += samples;
            if (nProgCurr >= nProgLast)
            {
                nProgCurr = nProgLast;
                pOutStatus->setValue(STATUS_OK);

                path_t *path = pOutFile->getBuffer<path_t>();
                if (path->accepted())
                    path->commit();
            }
            pOutProgress->setValue(float(nProgCurr * 100.0f) / float(nProgLast));
//            lsp_trace("set progress %f%%", pOutProgress->getValue());
            bFileSet = false;
        }
        else
        {
            // We've got pending file primitive?
            path_t *path = pOutFile->getBuffer<path_t>();
            if ((path != NULL) && (path->pending()))
            {
                bFileSet = true;
                path->accept();
            }

            // We can launch file saving routine?
            if ((bFileSet) && (pOutCmd->getValue() > 0.5f))
            {
                pOutStatus->setValue(STATUS_LOADING);
                nProgLast       = fSampleRate * 10;
                nProgCurr       = 0;

                pOutProgress->setValue(0);
            }
        }


        // Fill mesh with some stuff
        nPhase      = ((nPhase + samples) & 0x7ffff);

        mesh_t *mesh = pMesh->getBuffer<mesh_t>();
        if ((mesh != NULL) && (mesh->isEmpty()))
        {
            float phase = (nPhase * M_PI * 2.0f) / float(0x80000);
            for (size_t i=0; i<320; ++i)
            {
                mesh->pvData[0][i] = 10.0f * expf(logf(2400.0f) * i / 320.0f);
                mesh->pvData[1][i] = 10 * (1.0f + cosf(phase + (2.0f * M_PI * i) / 160.0f));
            }
            mesh->data(2, 320);
        }

        // Process file
        path_t *path = pFileName->getBuffer<path_t>();
        if ((path != NULL) && (path->accepted()))
        {
            // Commit path state
            pLength->setValue(samples_to_millis(fSampleRate, 256));

            mesh = pData->getBuffer<mesh_t>();
            if (mesh != NULL)
            {
                if (nStatus == STATUS_OK)
                {
                    size_t N = pData->metadata()->start;

                    float *dst = mesh->pvData[0];
                    for (size_t i=0; i<N; ++i)
                        dst[i] = fabs(sinf(0.025f * M_PI * i));

                    dst = mesh->pvData[1];
                    for (size_t i=0; i<N; ++i)
                        dst[i] = fabs(cosf(0.025f * M_PI * i) / (1.0f + 0.05f * i));

                    mesh->data(2, N);
                }
                else
                    mesh->data(0, 0);
            }

            path->commit();
        }

        pStatus->setValue(nStatus);

        // Query inline display for redraw
        pWrapper->query_display_draw();
    }

    bool test_plugin::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // Init canvas
        if (!cv->init(width, height))
            return false;
        width   = cv->width();
        height  = cv->height();

        size_t base         = nColorID / 20;
        float mix           = (nColorID % 20) * 0.05f;
        if ((++nColorID) >= 120)
            nColorID = 0;

        uint32_t b_color    = colors[base];
        uint32_t m_color    = colors[(base+1)%6];

        float b             = (b_color & 0xff) * (1.0f - mix) + (m_color & 0xff) * mix;
        float g             = ((b_color >> 8) & 0xff) * (1.0f - mix) + ((m_color >> 8) & 0xff) * mix;
        float r             = ((b_color >> 16) & 0xff) * (1.0f - mix) + ((m_color >> 16) & 0xff) * mix;

        if (b > 0xff)
            b = 0xff;
        if (g > 0xff)
            g = 0xff;
        if (r > 0xff)
            r = 0xff;

        b_color             = uint32_t(b) | (uint32_t(g) << 8) | (uint32_t(r) << 16);

        cv->set_color_rgb(CV_BACKGROUND);
        cv->paint();

        cv->set_line_width(2.0);
        cv->set_color(b_color);
        cv->circle(width >> 1, height >>1, width >> 2);


        return true;
    }
#endif
}


