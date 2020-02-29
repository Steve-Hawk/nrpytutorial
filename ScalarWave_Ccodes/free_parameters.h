
// Set free-parameter values.

// Set free-parameter values for the initial data.
params.time = 0.0; params.wavespeed = 1.0;
params.kk0 = 1.0; params.kk1 = 1.0; params.kk2 = 1.0;

const REAL domain_size    = 10.0;

// Override parameter defaults with values based on command line arguments and NGHOSTS.
const int Nx0x1x2 = atoi(argv[1]);
params.Nxx0 = Nx0x1x2;
params.Nxx1 = Nx0x1x2;
params.Nxx2 = Nx0x1x2;
params.Nxx_plus_2NGHOSTS0 = params.Nxx0 + 2*NGHOSTS;
params.Nxx_plus_2NGHOSTS1 = params.Nxx1 + 2*NGHOSTS;
params.Nxx_plus_2NGHOSTS2 = params.Nxx2 + 2*NGHOSTS;
// Step 0d: Set up space and time coordinates
// Step 0d.i: Declare \Delta x^i=dxx{0,1,2} and invdxx{0,1,2}, as well as xxmin[3] and xxmax[3]:
const REAL xxmin[3] = {-10.0,-10.0,-10.0 };
const REAL xxmax[3] = {+10.0,+10.0,+10.0 };

params.dxx0 = (xxmax[0] - xxmin[0]) / ((REAL)params.Nxx0);
params.dxx1 = (xxmax[1] - xxmin[1]) / ((REAL)params.Nxx1);
params.dxx2 = (xxmax[2] - xxmin[2]) / ((REAL)params.Nxx2);
params.invdx0 = 1.0 / params.dxx0;
params.invdx1 = 1.0 / params.dxx1;
params.invdx2 = 1.0 / params.dxx2;

