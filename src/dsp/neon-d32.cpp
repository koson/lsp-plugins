/*
 * neon-d32.cpp
 *
 *  Created on: 2 сент. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <core/types.h>
#include <test/test.h>
#include <dsp/arch/arm/features.h>

#define DSP_ARCH_ARM_NEON_32_IMPL

namespace neon_d32 // TODO: make constants common for all architectures
{
    //-------------------------------------------------------------------------
    // Constants definition
    #define DSP_F32VEC4(name, v)        static const float name[] __lsp_aligned16          = { v, v, v, v }
    #define DSP_U32VEC4(name, v)        static const uint32_t name[] __lsp_aligned16       = { uint32_t(v), uint32_t(v), uint32_t(v), uint32_t(v) }

    #define DSP_F32VECX4(name, a, b, c, d)  static const float name[] __lsp_aligned16      = { a, b, c, d }
    #define DSP_U32VECX4(name, a, b, c, d)  static const uint32_t name[] __lsp_aligned16   = { uint32_t(a), uint32_t(b), uint32_t(c), uint32_t(d) }

    #define DSP_F32REP4(v)              v, v, v, v
    #define DSP_U32REP4(v)              uint32_t(v), uint32_t(v), uint32_t(v), uint32_t(v)

    #define DSP_F32ARRAY(name, ...)     static const float name[] __lsp_aligned16          = { __VA_ARGS__ }
    #define DSP_U32ARRAY(name, ...)     static const uint32_t name[] __lsp_aligned16       = { __VA_ARGS__ }

    #include <dsp/common/const/const16.h>

    #undef DSP_F32ARRAY_IMPL
    #undef DSP_F32ARRAY

    #undef DSP_U32REP4
    #undef DSP_F32REP4

    #undef DSP_U32VECX4
    #undef DSP_F32VECX4

    #undef DSP_U32VEC4
    #undef DSP_F32VEC4
}

#include <dsp/arch/arm/neon-d32/copy.h>
#include <dsp/arch/arm/neon-d32/complex.h>
#include <dsp/arch/arm/neon-d32/pcomplex.h>
#include <dsp/arch/arm/neon-d32/convolution.h>
#include <dsp/arch/arm/neon-d32/graphics.h>
#include <dsp/arch/arm/neon-d32/pmath.h>

#undef DSP_ARCH_ARM_NEON_32_IMPL

#define EXPORT2(function, export)           dsp::function = neon_d32::export; TEST_EXPORT(neon_d32::export);
#define EXPORT1(function)                   EXPORT2(function, function)

namespace neon_d32
{
    void dsp_init(const arm::cpu_features_t *f)
    {
        if ((f->hwcap & (HWCAP_ARM_NEON | HWCAP_ARM_VFPD32)) != (HWCAP_ARM_NEON | HWCAP_ARM_VFPD32))
            return;

        lsp_trace("Optimizing DSP for NEON-D32 instruction set");
        EXPORT1(copy);
        EXPORT1(move);

        EXPORT1(complex_mul2);
        EXPORT1(complex_mul3);
        EXPORT1(complex_div2);
        EXPORT1(complex_rdiv2);
        EXPORT1(complex_div3);
        EXPORT1(complex_mod);
        EXPORT1(complex_rcp1);
        EXPORT1(complex_rcp2);

        EXPORT1(pcomplex_r2c);
        EXPORT1(pcomplex_c2r);
        EXPORT1(pcomplex_add_r);
        EXPORT1(pcomplex_mul2);
        EXPORT1(pcomplex_mul3);
        EXPORT1(pcomplex_div2);
        EXPORT1(pcomplex_rdiv2);
        EXPORT1(pcomplex_div3);
        EXPORT1(pcomplex_mod);
        EXPORT1(pcomplex_rcp1);
        EXPORT1(pcomplex_rcp2);

        EXPORT1(convolve);

        EXPORT1(axis_apply_log);
        EXPORT1(rgba32_to_bgra32);

        EXPORT1(abs1);
        EXPORT1(abs2);

        EXPORT1(scale2);
        EXPORT1(scale3);

        EXPORT1(add2);
        EXPORT1(sub2);
        EXPORT1(mul2);
        EXPORT1(div2);

        EXPORT1(abs_add2);
        EXPORT1(abs_sub2);
        EXPORT1(abs_mul2);
        EXPORT1(abs_div2);

        EXPORT1(scale_add3);
        EXPORT1(scale_sub3);
        EXPORT1(scale_mul3);
        EXPORT1(scale_div3);

        EXPORT1(add3);
        EXPORT1(sub3);
        EXPORT1(mul3);
        EXPORT1(div3);

        EXPORT1(abs_add3);
        EXPORT1(abs_sub3);
        EXPORT1(abs_mul3);
        EXPORT1(abs_div3);

        EXPORT1(scale_add4);
        EXPORT1(scale_sub4);
        EXPORT1(scale_mul4);
        EXPORT1(scale_div4);

        // Misc for testing
        TEST_EXPORT(neon_d32::complex_mul3_x12);
    }
}

//#endif /* ARCH_ARM */
