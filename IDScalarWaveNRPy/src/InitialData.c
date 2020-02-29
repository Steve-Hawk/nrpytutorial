#include <math.h>
#include <stdio.h>

#include "cctk.h"
#include "cctk_Parameters.h"
#include "cctk_Arguments.h"

void IDScalarWaveNRPy_param_check(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  if (kk0 == 0 && kk1 == 0 && kk2 == 0) {
     CCTK_WARN(0,"kk0==kk1==kk2==0: Zero wave vector cannot be normalized. Set one of the kk's to be != 0.");
  }
}

void IDScalarWaveNRPy_InitialData(CCTK_ARGUMENTS)
{
  DECLARE_CCTK_ARGUMENTS
  DECLARE_CCTK_PARAMETERS
  
  const CCTK_REAL *xGF = x;
  const CCTK_REAL *yGF = y;
  const CCTK_REAL *zGF = z;
#include "ScalarWave_PWID.h"
}
