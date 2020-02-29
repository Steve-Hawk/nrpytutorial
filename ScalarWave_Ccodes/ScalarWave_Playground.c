
// Part P0: Import NGHOSTS, which is based on FD_CENTDERIVS_ORDER
#include "ScalarWave_NGHOSTS.h"
// Part P0a: set REAL=double, so that all floating point numbers are stored to at least ~16 significant digits.
#define REAL double

#include "declare_Cparameters_struct.h"

// All SIMD intrinsics used in SIMD-enabled C code loops are defined here:
#include "../SIMD/SIMD_intrinsics.h"

const int NSKIP_0D_OUTPUT = 1;
const int NSKIP_2D_OUTPUT = 10;

// Part P1: Import needed header files
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

// Part P2: Add needed #define's to set data type, the IDX4S() macro, and the gridfunctions
// Part P2a: Declare the IDX4S(gf,i,j,k) macro, which enables us to store 4-dimensions of
//           data in a 1D array. In this case, consecutive values of "i" 
//           (all other indices held to a fixed value) are consecutive in memory, where 
//           consecutive values of "j" (fixing all other indices) are separated by 
//           Nxx_plus_2NGHOSTS0 elements in memory. Similarly, consecutive values of
//           "k" are separated by Nxx_plus_2NGHOSTS0*Nxx_plus_2NGHOSTS1 in memory, etc.
#define IDX4S(g,i,j,k) \
( (i) + Nxx_plus_2NGHOSTS0 * ( (j) + Nxx_plus_2NGHOSTS1 * ( (k) + Nxx_plus_2NGHOSTS2 * (g) ) ) )
#define LOOP_ALL_GFS_GPS(ii) _Pragma("omp parallel for") \
  for(int (ii)=0;(ii)<Nxx_plus_2NGHOSTS_tot*NUM_EVOL_GFS;(ii)++)
// Part P2b: Set UUGF and VVGF macros
#define NUM_EVOL_GFS 2
#define UUGF 0
#define VVGF 1

// Part P3: Declare the function for the exact solution at a single point. time==0 corresponds to the initial data.
#include "exact_solution_single_point.h"

// Part P4: Declare the function for the exact solution at all points. time==0 corresponds to the initial data.
#include "exact_solution_all_points.h"

// Part P5: Declare the function to evaluate the scalar wave RHSs
#include "rhs_eval.h"

// Part P6: Boundary condition driver routine: Apply BCs to all six
//          boundary faces of the cube, filling in the innermost
//          ghost zone first, and moving outward.
#include "apply_bcs.h"

// Part P7: 2D File output routine, for comparing numerical results to exact solution
void output_2D(const paramstruct *restrict params, const int iter, REAL *xx[3],
               const REAL *numerical_gridfunction_data,REAL *gridfunction_to_store_exact) {
#include "set_Cparameters.h"
    // Step 2a: Validation: Output to 2D data files numerical and exact solutions
    char filename[100];
    sprintf(filename,"out2D__resolution_%dx%dx%d__iter_%d.txt",Nxx0,Nxx1,Nxx2,iter);
    FILE *out2D = fopen(filename, "w");
    const int i2  = (int)((Nxx2+ 2*NGHOSTS)*0.5);
    const REAL xx2 = xx[2][i2];
    for(int i0=0;i0<Nxx0+2*NGHOSTS;i0++) {
        for(int i1=0;i1<Nxx1+2*NGHOSTS;i1++) {
            if(i0> (Nxx0+2*NGHOSTS)*.25 && i0< (Nxx0+2*NGHOSTS)*.75 &&
               i1> (Nxx1+2*NGHOSTS)*.25 && i1< (Nxx1+2*NGHOSTS)*.75) { 
                const REAL xx0 = xx[0][i0];
                const REAL xx1 = xx[1][i1];
                REAL uu_exact,vv_exact; exact_solution_single_point(xx0,xx1,xx2,params, &uu_exact,&vv_exact);
                fprintf(out2D,"%e %e %e %e\n", xx0,xx1,
                        numerical_gridfunction_data[IDX4S(0,i0,i1, (int)((Nxx2+ 2*NGHOSTS)*0.5))], uu_exact);
            }
        }
    }
    fclose(out2D);
}

// main() function:
// Step 0: Read command-line input, set up grid structure, allocate memory for gridfunctions, set up coordinates
// Step 1: Set up scalar wave initial data
// Step 2: Evolve scalar wave initial data forward in time using Method of Lines with RK4 algorithm,
//         applying quadratic extrapolation outer boundary conditions.
// Step 3: Output relative error between numerical and exact solution.
// Step 4: Free all allocated memory
int main(int argc, const char *argv[]) {
    paramstruct params;
#include "set_Cparameters_default.h"
    // Step 0a: Read command-line input, error out if nonconformant
    if(argc != 2 || atoi(argv[1]) < NGHOSTS) {
      printf("Error: Expected one command-line argument: ./ScalarWave_Playground [Nx(=Ny=Nz)],\n");
      printf("where Nx is the number of grid points in the x,y, and z directions.\n");
      printf("Nx MUST BE larger than NGHOSTS (= %d)\n",NGHOSTS);
      exit(1);
    }
    // Step 0c: Set free parameters, overwriting Cparameters defaults 
    //          by hand or with command-line input, as desired.
#include "free_parameters.h"
#include "set_Cparameters-nopointer.h"

    //          ... and then set up the numerical grid structure in time:
    const REAL t_final = xxmax[0]*0.8; /* Final time is set so that at t=t_final, 
                                        data at the origin have not been corrupted 
                                        by the approximate outer boundary condition */
    const REAL CFL_FACTOR = 0.5; // Set the CFL Factor

    // Step 0c: Allocate memory for gridfunctions
    const int Nxx_plus_2NGHOSTS_tot = Nxx_plus_2NGHOSTS0*Nxx_plus_2NGHOSTS1*Nxx_plus_2NGHOSTS2;
    // Step 0k: Allocate memory for gridfunctions
#include "MoLtimestepping/RK_Allocate_Memory.h"

    // Step 0d: Set up coordinates: Set dx, and then dt based on dx_min and CFL condition
    #define MIN(A, B) ( ((A) < (B)) ? (A) : (B) )
    REAL dt = CFL_FACTOR * MIN(dxx0,MIN(dxx1,dxx2)); // CFL condition
    int Nt = (int)(t_final / dt + 0.5); // The number of points in time.
                                        //Add 0.5 to account for C rounding down integers.

    // Step 0e: Set up cell-centered Cartesian coordinate grids
    REAL *xx[3];
    xx[0] = (REAL *)malloc(sizeof(REAL)*Nxx_plus_2NGHOSTS0);
    xx[1] = (REAL *)malloc(sizeof(REAL)*Nxx_plus_2NGHOSTS1);
    xx[2] = (REAL *)malloc(sizeof(REAL)*Nxx_plus_2NGHOSTS2);
    for(int j=0;j<Nxx_plus_2NGHOSTS0;j++) xx[0][j] = xxmin[0] + (j-NGHOSTS)*dxx0;
    for(int j=0;j<Nxx_plus_2NGHOSTS1;j++) xx[1][j] = xxmin[1] + (j-NGHOSTS)*dxx1;
    for(int j=0;j<Nxx_plus_2NGHOSTS2;j++) xx[2][j] = xxmin[2] + (j-NGHOSTS)*dxx2;

    // Step 1: Set up initial data to be exact solution at time=0:
    params.time = 0.0; exact_solution_all_points(&params, xx, y_n_gfs);

    for(int n=0;n<=Nt;n++) { // Main loop to progress forward in time.
        // Step 1a: Set current time to correct value & compute exact solution
        params.time = ((REAL)n)*dt;

        /* Step 2: Validation: Output relative error between numerical and exact solution, */
        // Step 2b: Output to 2D grid (region of x-y plane near origin) 
        //          every NSKIP_2D_OUTPUT iterations.
        if((n)%NSKIP_2D_OUTPUT ==0) {
            // Step 2a: Evaluate exact solution at current time, (n)*dt. Store to diagnostic_output_gfs.
            output_2D(&params, n, xx, y_n_gfs, diagnostic_output_gfs);
        }

        if((n)%NSKIP_0D_OUTPUT ==0) {
            // Step 2c: Output relative error between exact & numerical at center of grid.
            const int i0mid=Nxx_plus_2NGHOSTS0/2;
            const int i1mid=Nxx_plus_2NGHOSTS1/2;
            const int i2mid=Nxx_plus_2NGHOSTS2/2;
            REAL uu_exact,vv_exact; exact_solution_single_point(xx[0][i0mid],xx[1][i1mid],xx[2][i2mid],&params, 
                                                                &uu_exact,&vv_exact);
                                                                 
            const REAL numerical = y_n_gfs[IDX4S(UUGF,i0mid,i1mid,i2mid)];
            const REAL relative_error = fabs((uu_exact-numerical)/uu_exact);
            printf("%e %e || %e %e %e: %e %e\n",params.time, log10(relative_error),
                   xx[0][i0mid],xx[1][i1mid],xx[2][i2mid], numerical,uu_exact);
        }
                            
        // Step 3: Evolve scalar wave initial data forward in time using Method of Lines with RK4 algorithm,
        //         applying quadratic extrapolation outer boundary conditions.
        // Step 3.b: Step forward one timestep (t -> t+dt) in time using 
        //           chosen RK-like MoL timestepping algorithm
#include "MoLtimestepping/RK_MoL.h"
    } // End main loop to progress forward in time.

    // Step 4: Free all allocated memory
#include "MoLtimestepping/RK_Free_Memory.h"
    for(int i=0;i<3;i++) free(xx[i]);
    return 0;
}
