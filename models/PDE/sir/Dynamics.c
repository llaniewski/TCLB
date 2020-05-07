// Model for solving the diffusion equation:
// phi' = c*lap(phi)
// phi' = c * (phi_xx + phi_yy)

CudaDeviceFunction float2 Color() {
  float2 ret;
  ret.x = getInfected();
  ret.y = 1;
  return ret;
}

CudaDeviceFunction real_t getInfected() {
  return I;
}

CudaDeviceFunction real_t getRecovered() {
  return R;
}

CudaDeviceFunction void Init() {
  N = Population;
  I = Sick;
  R = 0;
}

CudaDeviceFunction void Run() { 


  real_t a,b;
  a = sqrt(N)*0.25;
  b = N - a*4.0;

  I = I + Beta  * (1-I-R)*(b*fI + a*(fI(1,0)+fI(0,1)+fI(-1,0)+fI(0,-1)));
  R = R + Alpha * I;
  I = I - Alpha * I;
  if (N < 1.0) {
    fI = 0;
  } else {
    fI = floor(I*N)/N;
  }
  AddToTotalPopulation(N);
  AddToTotalSick(N*I);
  AddToTotalRecovered(N*R);
}

