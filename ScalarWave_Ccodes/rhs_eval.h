/*
Part P5: Declare the function to evaluate the scalar wave RHSs
 */
void rhs_eval(const paramstruct *restrict params, const REAL *restrict in_gfs, REAL *restrict rhs_gfs) {
#include "set_Cparameters-SIMD.h"

#pragma omp parallel for
    for(int i2=NGHOSTS; i2<NGHOSTS+Nxx2; i2++) {
        for(int i1=NGHOSTS; i1<NGHOSTS+Nxx1; i1++) {
            for(int i0=NGHOSTS; i0<NGHOSTS+Nxx0; i0+=SIMD_width) {
                
                {
                   /* 
                    * NRPy+ Finite Difference Code Generation, Step 1 of 2: Read from main memory and compute finite difference stencils:
                    */
                   /*
                    *  Original SymPy expressions:
                    *  "[const REAL_SIMD_ARRAY uu_dDD00 = invdx0**2*(-5*uu/2 + 4*uu_i0m1_i1_i2/3 - uu_i0m2_i1_i2/12 + 4*uu_i0p1_i1_i2/3 - uu_i0p2_i1_i2/12),
                    *    const REAL_SIMD_ARRAY uu_dDD11 = invdx1**2*(-5*uu/2 + 4*uu_i0_i1m1_i2/3 - uu_i0_i1m2_i2/12 + 4*uu_i0_i1p1_i2/3 - uu_i0_i1p2_i2/12),
                    *    const REAL_SIMD_ARRAY uu_dDD22 = invdx2**2*(-5*uu/2 + 4*uu_i0_i1_i2m1/3 - uu_i0_i1_i2m2/12 + 4*uu_i0_i1_i2p1/3 - uu_i0_i1_i2p2/12)]"
                    */
                   const REAL_SIMD_ARRAY uu_i0_i1_i2m2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0,i1,i2-2)]);
                   const REAL_SIMD_ARRAY uu_i0_i1_i2m1 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0,i1,i2-1)]);
                   const REAL_SIMD_ARRAY uu_i0_i1m2_i2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0,i1-2,i2)]);
                   const REAL_SIMD_ARRAY uu_i0_i1m1_i2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0,i1-1,i2)]);
                   const REAL_SIMD_ARRAY uu_i0m2_i1_i2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0-2,i1,i2)]);
                   const REAL_SIMD_ARRAY uu_i0m1_i1_i2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0-1,i1,i2)]);
                   const REAL_SIMD_ARRAY uu = ReadSIMD(&in_gfs[IDX4S(UUGF, i0,i1,i2)]);
                   const REAL_SIMD_ARRAY uu_i0p1_i1_i2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0+1,i1,i2)]);
                   const REAL_SIMD_ARRAY uu_i0p2_i1_i2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0+2,i1,i2)]);
                   const REAL_SIMD_ARRAY uu_i0_i1p1_i2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0,i1+1,i2)]);
                   const REAL_SIMD_ARRAY uu_i0_i1p2_i2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0,i1+2,i2)]);
                   const REAL_SIMD_ARRAY uu_i0_i1_i2p1 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0,i1,i2+1)]);
                   const REAL_SIMD_ARRAY uu_i0_i1_i2p2 = ReadSIMD(&in_gfs[IDX4S(UUGF, i0,i1,i2+2)]);
                   const REAL_SIMD_ARRAY vv = ReadSIMD(&in_gfs[IDX4S(VVGF, i0,i1,i2)]);
                   const double tmpFD_Rational_4_3_FDcoeff = 1.333333333333333333333333333333333;
                   const REAL_SIMD_ARRAY _Rational_4_3_FDcoeff = ConstSIMD(tmpFD_Rational_4_3_FDcoeff);
                   
                   const double tmpFD_Rational_m1_12_FDcoeff = -0.08333333333333333333333333333333333;
                   const REAL_SIMD_ARRAY _Rational_m1_12_FDcoeff = ConstSIMD(tmpFD_Rational_m1_12_FDcoeff);
                   
                   const double tmpFD_Rational_m5_2_FDcoeff = -2.500000000000000000000000000000000;
                   const REAL_SIMD_ARRAY _Rational_m5_2_FDcoeff = ConstSIMD(tmpFD_Rational_m5_2_FDcoeff);
                   
                   const REAL_SIMD_ARRAY tmpFD0 = MulSIMD(uu, _Rational_m5_2_FDcoeff);
                   const REAL_SIMD_ARRAY uu_dDD00 = MulSIMD(FusedMulAddSIMD(uu_i0p1_i1_i2, _Rational_4_3_FDcoeff, FusedMulAddSIMD(uu_i0m2_i1_i2, _Rational_m1_12_FDcoeff, FusedMulAddSIMD(uu_i0m1_i1_i2, _Rational_4_3_FDcoeff, FusedMulAddSIMD(uu_i0p2_i1_i2, _Rational_m1_12_FDcoeff, tmpFD0)))), MulSIMD(invdx0, invdx0));
                   const REAL_SIMD_ARRAY uu_dDD11 = MulSIMD(FusedMulAddSIMD(uu_i0_i1p1_i2, _Rational_4_3_FDcoeff, FusedMulAddSIMD(uu_i0_i1m2_i2, _Rational_m1_12_FDcoeff, FusedMulAddSIMD(uu_i0_i1m1_i2, _Rational_4_3_FDcoeff, FusedMulAddSIMD(uu_i0_i1p2_i2, _Rational_m1_12_FDcoeff, tmpFD0)))), MulSIMD(invdx1, invdx1));
                   const REAL_SIMD_ARRAY uu_dDD22 = MulSIMD(FusedMulAddSIMD(uu_i0_i1_i2p1, _Rational_4_3_FDcoeff, FusedMulAddSIMD(uu_i0_i1_i2m2, _Rational_m1_12_FDcoeff, FusedMulAddSIMD(uu_i0_i1_i2m1, _Rational_4_3_FDcoeff, FusedMulAddSIMD(uu_i0_i1_i2p2, _Rational_m1_12_FDcoeff, tmpFD0)))), MulSIMD(invdx2, invdx2));
                   /* 
                    * NRPy+ Finite Difference Code Generation, Step 2 of 2: Evaluate SymPy expressions and write to main memory:
                    */
                   /*
                    *  Original SymPy expressions:
                    *  "[const REAL_SIMD_ARRAY __RHS_exp_0 = vv,
                    *    const REAL_SIMD_ARRAY __RHS_exp_1 = uu_dDD00*wavespeed**2 + uu_dDD11*wavespeed**2 + uu_dDD22*wavespeed**2]"
                    */
                   const REAL_SIMD_ARRAY tmp0 = MulSIMD(wavespeed, wavespeed);
                   const REAL_SIMD_ARRAY __RHS_exp_0 = vv;
                   const REAL_SIMD_ARRAY __RHS_exp_1 = FusedMulAddSIMD(tmp0, uu_dDD11, FusedMulAddSIMD(tmp0, uu_dDD22, MulSIMD(tmp0, uu_dDD00)));
                   WriteSIMD(&rhs_gfs[IDX4S(UUGF, i0, i1, i2)], __RHS_exp_0);
                   WriteSIMD(&rhs_gfs[IDX4S(VVGF, i0, i1, i2)], __RHS_exp_1);
                }
                
                
            } // END LOOP: for(int i0=NGHOSTS; i0<NGHOSTS+Nxx0; i0+=SIMD_width)
        } // END LOOP: for(int i1=NGHOSTS; i1<NGHOSTS+Nxx1; i1++)
    } // END LOOP: for(int i2=NGHOSTS; i2<NGHOSTS+Nxx2; i2++)
}
