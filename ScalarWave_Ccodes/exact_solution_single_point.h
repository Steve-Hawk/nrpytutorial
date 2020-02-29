/*
Part P3: Declare the function for the exact solution at a single point. time==0 corresponds to the initial data.
 */
void exact_solution_single_point(const REAL xx0,const REAL xx1,const REAL xx2,const paramstruct *restrict params,REAL *uu_exact,REAL *vv_exact) {
#include "set_Cparameters.h"

{
   /* 
    * NRPy+ Finite Difference Code Generation, Step 1 of 1: Evaluate SymPy expressions and write to main memory:
    */
   /*
    *  Original SymPy expressions:
    *  "[*uu_exact = 2 - sin(time*wavespeed - (kk0*xx0 + kk1*xx1 + kk2*xx2)/sqrt(kk0**2 + kk1**2 + kk2**2)),
    *    *vv_exact = -wavespeed*cos(time*wavespeed - (kk0*xx0 + kk1*xx1 + kk2*xx2)/sqrt(kk0**2 + kk1**2 + kk2**2))]"
    */
   const double tmp0 = time*wavespeed - (kk0*xx0 + kk1*xx1 + kk2*xx2)/sqrt(((kk0)*(kk0)) + ((kk1)*(kk1)) + ((kk2)*(kk2)));
   *uu_exact = 2 - sin(tmp0);
   *vv_exact = -wavespeed*cos(tmp0);
}

}
