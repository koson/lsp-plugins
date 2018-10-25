/*
 * scramble.h
 *
 *  Created on: 24 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FFT_SCRAMBLE_H_
#define DSP_ARCH_ARM_NEON_D32_FFT_SCRAMBLE_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    static void scramble_direct(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        IF_ARCH_ARM( size_t count = 1 << rank );

        if ((dst_re == src_re) || (dst_im == src_im))
        {
            IF_ARCH_ARM(
                    size_t i, j, rrank = rank;
                    float *d_re, *d_im;
            );

            // Self algorithm
            ARCH_ARM_ASM(
                // Do bit-reverse shuffle
                __ASM_EMIT("rsb         %[rrank], %[rrank], $32")           // rrank = 32 - rank
                __ASM_EMIT("push        {%[dst_re], %[dst_im], %[src_re], %[src_im]}")
                __ASM_EMIT("vmov        %[i], $1")                          // i = 0

                __ASM_EMIT("1:")
                __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
                __ASM_EMIT("add         %[src_re], $4")
                __ASM_EMIT("lsr         %[j], %[rrank]")                    // j = reverse_bits(i) >> rank
                __ASM_EMIT("add         %[src_im], $4")
                __ASM_EMIT("cmp         %[i], %[j]")                        // i <=> j
                __ASM_EMIT("bhs         2f")                                // if (i >= j) continue
                __ASM_EMIT("add         %[d_re], %[dst_re], %[j], LSL $2")  // d_re = &dst_re[j]
                __ASM_EMIT("vldm        %[src_re], {s0}")                   // s0 = *src_re
                __ASM_EMIT("add         %[d_im], %[dst_im], %[j], LSL $2")  // d_im = &dst_im[j]
                __ASM_EMIT("vldm        %[src_im], {s1}")                   // s1 = *src_im
                __ASM_EMIT("vldm        %[d_re], {s2}")                     // s2 = *td_re
                __ASM_EMIT("vldm        %[d_im], {s3}")                     // s3 = *td_im
                __ASM_EMIT("vstm        %[src_re]!, {s2}")                  // *(src_re++) = s2
                __ASM_EMIT("vstm        %[src_im]!, {s3}")                  // *(src_im++) = s3
                __ASM_EMIT("vstm        %[d_re], {s0}")                     // *d_re = s0
                __ASM_EMIT("vstm        %[d_im], {s1}")                     // *d_im = s1
                __ASM_EMIT("2:")
                __ASM_EMIT("add         %[i], $1")                          // i++
                __ASM_EMIT("cmp         %[i], %[count]")                    // i <=> count
                __ASM_EMIT("blo         1b")

                __ASM_EMIT("pop         {%[dst_re], %[dst_im], %[src_re], %[src_im]}")

#if 0
                // Perform butterflies


                __ASM_EMIT("3:")
                __ASM_EMIT("vld2.32     {q0-q1}, %[dst_re]")                // q0 = r0 r2 r4 r6, q1 = r1 r3 r5 r7
                __ASM_EMIT("vld2.32     {q2-q3}, %[dst_im]")                // q2 = i0 i2 i4 i6, q3 = i1 i3 i5 i7
                __ASM_EMIT("vadd.f32    q4, q0, q1")                        // q4 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6'
                __ASM_EMIT("vadd.f32    q5, q2, q3")                        // q5 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6'
                __ASM_EMIT("vsub.f32    q0, q0, q1")                        // q0 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7'
                __ASM_EMIT("vsub.f32    q1, q2, q3")                        // q1 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7'

                // q4 = r0' r2' r4' r6'
                // q0 = r1' r3' r5' r7'
                // q5 = i0' i2' i4' i6'
                // q1 = i1' i3' i5' i7'
                __ASM_EMIT("vuzp.32     q4, q0")                            // q4 = r0' r4' r1' r5', q0 = r2' r6' r3' r7'
                __ASM_EMIT("vuzp.32     q5, q1")                            // q5 = i0' i4' i1' i5', q1 = i2' i6' i3' i7'
                __ASM_EMIT("vswp        d1, d3")                            // q0 = r2' r6' i3' i7', q1 = i2' i6' r3' r7'
                __ASM_EMIT("vadd.f32    q2, q4, q0")                        // q2 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5"
                __ASM_EMIT("vsub.f32    q3, q4, q0")                        // q3 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7"
                __ASM_EMIT("vadd.f32    q0, q5, q1")                        // q0 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7"
                __ASM_EMIT("vsub.f32    q1, q5, q1")                        // q1 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5"

                // q0 = i0" i4" i3" i7"
                // q1 = i2" i6" i1" i5"
                // q2 = r0" r4" r1" r5"
                // q3 = r2" r6" r3" r7"
                __ASM_EMIT("vuzp.32    q2, q3")                             // q2 = r0" r1" r2" r3", q3 = r4" r5" r6" r7"
                __ASM_EMIT("vuzp.32    q0, q1")                             // q0 = i0" i3" i2" i1", q1 = i4" i7" i6" i5"
                __ASM_EMIT("vswp       s1, s3")                             // q0 = i0" i1" i2" i3"
                __ASM_EMIT("vswp       s5, s7")                             // q1 = i4" i5" i6" i7"

                __ASM_EMIT("vst1.32    {q2-q3}, %[dst_re]!")
                __ASM_EMIT("vst1.32    {q0-q1}, %[dst_im]!")
#endif
                : [src_re] "+r" (src_re), [src_im] "+r" (src_im),
                  [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
                  [d_re] "=&r" (d_re), [d_im] "=&r" (d_im),
                  [rrank] "+r" (rrank), [i] "=&r" (i), [j] "=&r" (j)
                : [count] "r" (count)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5"
            );
        }
        else
        {

        }
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FFT_SCRAMBLE_H_ */