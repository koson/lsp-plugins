/*
 * oversampling.cpp
 *
 *  Created on: 21 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>

#define RTEST_BUF_SIZE  0x1000

namespace native
{
    void lanczos_resample_2x2(float *dst, const float *src, size_t count);
    void lanczos_resample_2x3(float *dst, const float *src, size_t count);
    void lanczos_resample_3x2(float *dst, const float *src, size_t count);
    void lanczos_resample_3x3(float *dst, const float *src, size_t count);
    void lanczos_resample_4x2(float *dst, const float *src, size_t count);
    void lanczos_resample_4x3(float *dst, const float *src, size_t count);
    void lanczos_resample_6x2(float *dst, const float *src, size_t count);
    void lanczos_resample_6x3(float *dst, const float *src, size_t count);
    void lanczos_resample_8x2(float *dst, const float *src, size_t count);
    void lanczos_resample_8x3(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void lanczos_resample_2x2(float *dst, const float *src, size_t count);
        void lanczos_resample_2x3(float *dst, const float *src, size_t count);
        void lanczos_resample_3x2(float *dst, const float *src, size_t count);
        void lanczos_resample_3x3(float *dst, const float *src, size_t count);
        void lanczos_resample_4x2(float *dst, const float *src, size_t count);
        void lanczos_resample_4x3(float *dst, const float *src, size_t count);
        void lanczos_resample_6x2(float *dst, const float *src, size_t count);
        void lanczos_resample_6x3(float *dst, const float *src, size_t count);
        void lanczos_resample_8x2(float *dst, const float *src, size_t count);
        void lanczos_resample_8x3(float *dst, const float *src, size_t count);
    }
)

//-----------------------------------------------------------------------------
// Performance test for lanczos resampling
PTEST_BEGIN("dsp.resampling", oversampling, 5, 1000)

    void call(float *out, const float *in, size_t count, size_t times, const char *text, resampling_function_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s oversampling for %d -> %d samples ...\n", text, int(count), int(count * times));
        size_t zeros = count*times + RESAMPLING_RESERVED_SAMPLES;

        PTEST_LOOP(text,
            dsp::fill_zero(out, zeros);
            func(out, in, count);
        );
    }

    PTEST_MAIN
    {
        float *out          = new float[RTEST_BUF_SIZE*8 + RESAMPLING_RESERVED_SAMPLES];
        float *in           = new float[RTEST_BUF_SIZE];

        // Prepare data
        for (size_t i=0; i<RTEST_BUF_SIZE; ++i)
            in[i]               = (i % 1) ? 1.0f : -1.0f;
        dsp::fill_zero(out, RTEST_BUF_SIZE * 8 + RESAMPLING_RESERVED_SAMPLES);

        // Do tests
        call(out, in, RTEST_BUF_SIZE, 2, "2x2 native", native::lanczos_resample_2x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 2, "2x2 sse", sse::lanczos_resample_2x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 2, "2x3 native", native::lanczos_resample_2x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 2, "2x3 sse", sse::lanczos_resample_2x3));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 3, "3x2 native", native::lanczos_resample_3x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 3, "3x2 sse", sse::lanczos_resample_3x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 3, "3x3 native", native::lanczos_resample_3x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 3, "3x3 sse", sse::lanczos_resample_3x3));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 4, "4x2 native", native::lanczos_resample_4x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 4, "4x2 sse", sse::lanczos_resample_4x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 4, "4x3 native", native::lanczos_resample_4x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 4, "4x3 sse", sse::lanczos_resample_4x3));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 6, "6x2 native", native::lanczos_resample_6x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 6, "6x2 sse", sse::lanczos_resample_6x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 6, "6x3 native", native::lanczos_resample_6x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 6, "6x3 sse", sse::lanczos_resample_6x3));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 8, "8x2 native", native::lanczos_resample_8x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 8, "8x2 sse", sse::lanczos_resample_8x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 8, "8x3 native", native::lanczos_resample_8x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 8, "8x3 sse", sse::lanczos_resample_8x3));
        PTEST_SEPARATOR;

        delete [] out;
        delete [] in;
    }

PTEST_END


