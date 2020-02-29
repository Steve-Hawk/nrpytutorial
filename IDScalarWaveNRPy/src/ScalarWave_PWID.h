#pragma omp parallel for
for(int i2=1; i2<cctk_lsh[2]-1; i2++) {
    for(int i1=1; i1<cctk_lsh[1]-1; i1++) {
        for(int i0=1; i0<cctk_lsh[0]-1; i0++) {
            {
               /* 
                * NRPy+ Finite Difference Code Generation, Step 1 of 1: Read from main memory and compute finite difference stencils:
                */
               /*
                *  Original SymPy expression:
                */
               const double x = xGF[CCTK_GFINDEX3D(cctkGH, i0,i1,i2)];
               const double y = yGF[CCTK_GFINDEX3D(cctkGH, i0,i1,i2)];
               const double z = zGF[CCTK_GFINDEX3D(cctkGH, i0,i1,i2)];
               /* 
                * NRPy+ Finite Difference Code Generation, Step 2 of 1: Evaluate SymPy expressions and write to main memory:
                */
               /*
                *  Original SymPy expressions:
                *  "[uuGF[CCTK_GFINDEX3D(cctkGH, i0, i1, i2)] = 2 - sin(cctk_time*wavespeed - (kk0*x + kk1*y + kk2*z)/sqrt(kk0**2 + kk1**2 + kk2**2)),
                *    vvGF[CCTK_GFINDEX3D(cctkGH, i0, i1, i2)] = -wavespeed*cos(cctk_time*wavespeed - (kk0*x + kk1*y + kk2*z)/sqrt(kk0**2 + kk1**2 + kk2**2))]"
                */
               const double tmp0 = cctk_time*wavespeed - (kk0*x + kk1*y + kk2*z)/sqrt(((kk0)*(kk0)) + ((kk1)*(kk1)) + ((kk2)*(kk2)));
               uuGF[CCTK_GFINDEX3D(cctkGH, i0, i1, i2)] = 2 - sin(tmp0);
               vvGF[CCTK_GFINDEX3D(cctkGH, i0, i1, i2)] = -wavespeed*cos(tmp0);
            }
            
            
        } // END LOOP: for(int i0=1; i0<cctk_lsh[0]-1; i0++)
    } // END LOOP: for(int i1=1; i1<cctk_lsh[1]-1; i1++)
} // END LOOP: for(int i2=1; i2<cctk_lsh[2]-1; i2++)
