#include "extended-bootstrap.h"

/*
*********************************** ANY TORUS *******************************************
*/

/*
* Call a bootstrapping operation given a test vector
*/
void ATBootFFT::extendedBoot(LweSample * lweOut,
    LweBootstrappingKey * bk,
    TorusPolynomial* testvect,
    LweSample * lweIn)
{
  // Get the parameters
  const TGswParams* bk_params = bk->bk_params;
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* in_params = bk->in_out_params;
  const int N=accum_params->N;
  const int Nx2= 2*N;
  const int n = in_params->n;

  // Create bara and barb
  int* bara = new int[N];
  int barb = modSwitchFromTorus32(lweIn->b,Nx2);
  for (int i=0; i<n; i++) {
    bara[i]=modSwitchFromTorus32(lweIn->a[i],Nx2);
  }

  // Bootstrapping rotation and extraction
  tfhe_blindRotateAndExtract(lweOut, testvect, bk->bk, barb, bara, n, bk_params);

  // Clean up
  delete[] bara;

}


//template<typename TORUS>
void ATBootFFT::extendedBoot(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    TorusPolynomial* testvect,
    LweSample * lweIn)
{
  // Get the parameters
  const TGswParams* bk_params = bk->bk_params;
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* in_params = bk->in_out_params;
  const int N=accum_params->N;
  const int Nx2= 2*N;
  const int n = in_params->n;

  // Create bara and barb
  int* bara = new int[N];
  int barb = modSwitchFromTorus32(lweIn->b,Nx2);
  for (int i=0; i<n; i++) {
    bara[i]=modSwitchFromTorus32(lweIn->a[i],Nx2);
  }

  // Bootstrapping rotation and extraction
  tfhe_blindRotateAndExtract_FFT(lweOut, testvect, bk->bkFFT, barb, bara, n, bk_params);

  // Clean up
  delete[] bara;

}

/*
* Call a bootstrapping operation given a test vector and extracting a given coefficient
*/


void ATBootFFT::extendedBoot(LweSample* lweOut,
    LweBootstrappingKeyFFT * bk,
    TorusPolynomial* testvect,
    LweSample * lweIn,
    int extraction_index)
{
  const TLweParams* accum_params = bk->accum_params;
  TLweSample * tlweOut = new_TLweSample(accum_params);

  // Boot without extraction
  ATBootFFT::boot_wo_extract(tlweOut, bk, testvect, lweIn);

  // Extraction
  tLweExtractLweSampleIndex(lweOut, tlweOut, extraction_index, &accum_params->extracted_lweparams , accum_params);

  // Clean up
  delete tlweOut;

}


//template<typename TORUS>
void ATBootFFT::boot_wo_extract(TLweSample* tlweOut,
    LweBootstrappingKeyFFT * bk,
    TorusPolynomial* testvect,
    LweSample * lweIn)
{
  // Get the parameters
  const TGswParams* bk_params = bk->bk_params;
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* in_params = bk->in_out_params;
  const int N=accum_params->N;
  const int Nx2= 2*N;
  const int n = in_params->n;

  // Create bara and barb
  int* bara = new int[N];
  int barb = modSwitchFromTorus32(lweIn->b,Nx2);
  for (int i=0; i<n; i++) {
    bara[i]=modSwitchFromTorus32(lweIn->a[i],Nx2);
  }

  // Multiply the testvector by X^{2N-barb}*v
  TorusPolynomial* testvectbis = new_TorusPolynomial(N);
  if (barb!=0)
    torusPolynomialMulByXai(testvectbis, Nx2-barb, testvect);
  else
    torusPolynomialCopy(testvectbis, testvect);
  tLweNoiselessTrivial(tlweOut, testvectbis, accum_params);

  // Blind rotation
  tfhe_blindRotate_FFT(tlweOut, bk->bkFFT, bara, n, bk_params);

  // Clean up
  delete_TorusPolynomial(testvectbis);
  delete[] bara;
}

/*
*********************************** FULL TORUS *******************************************
*/

//template<typename TORUS>
void FTBootFFT::boot_inverted_sum(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    TorusPolynomial* testvect,
    LweSample * lweIn)
{
  // Parameters
  const TGswParams* bk_params = bk->bk_params;
  const TLweParams* accum_params = bk_params->tlwe_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;
  const int N=accum_params->N;
  const int N_2= N/2;

  // Apply the bootstrapping without the extraction
  TLweSample * acc = new_TLweSample(accum_params);
  ATBootFFT::boot_wo_extract(acc, bk, testvect, lweIn);

  // Extract at x (0) and at -x (N/2)
  LweSample * lweTemp = new_LweSample(extract_params);
  tLweExtractLweSampleIndex(lweOut,acc,0,extract_params,accum_params);
  tLweExtractLweSampleIndex(lweTemp,acc,N_2,extract_params,accum_params);

  // Add the two
  lweAddTo(lweOut, lweTemp, extract_params);

  // Clean up
  delete_TLweSample(acc);
  delete_LweSample(lweTemp);
}

/*
* Call a bootstrapping given an Id function.
*/

//template<typename TORUS>
void FTBootFFT::boot_Id(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2= 2*N;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
    testvect->coefsT[i] = modSwitchToTorus32(i, Nx2);

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Call a bootstrapping given an Id/2 function.
*/

//template<typename TORUS>
void FTBootFFT::boot_Id_over2(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx4= 4*N;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
    testvect->coefsT[i] = modSwitchToTorus32(i, Nx4);

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Call a bootstrapping given the -Id/2 function.
*/

//template<typename TORUS>
void FTBootFFT::boot_NegId_over2(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* init_lwe_params = bk->in_out_params;
  const int N=accum_params->N;
  const int Nx4= 4*N;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
    testvect->coefsT[i] = modSwitchToTorus32(i, Nx4);

  // Negate the input
  LweSample * lweNegIn = new_LweSample(init_lwe_params);
  lweNegate(lweNegIn, lweIn, init_lwe_params);

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweNegIn);

  // CLean up
  delete_TorusPolynomial(testvect);
  delete_LweSample(lweNegIn);
}

/*
* Call a bootstrapping given the x^n function
*/

//template<typename TORUS>
void FTBootFFT::boot_powerOf(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    int exponent)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const uint64_t N = (uint64_t) accum_params->N;

  double doubleOut;
  double N_ex2 = (double) 2*pow(N,exponent);

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleOut = pow(i,exponent)/N_ex2;
    testvect->coefsT[i] = dtot32(doubleOut);
  }
  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Call a bootstrapping given the symmetric sigmoid function
*/

//template<typename TORUS>
void FTBootFFT::boot_Sigmoid(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    double modulus)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const uint64_t N = (uint64_t) accum_params->N;
  const int Nx2= 2*N;

  const double modulusx2 = modulus*2;
  const double modulus_2 = modulus/2;

  const double modsigmoid = oddSigmoid(modulus_2);

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2;
    /*
    * New method here
    * We first apply a sigmoid of all possible int values (in [0,m/2]) therefore getting something in [0,sigmoid(m/2)]
    * Then we divide by 2*sigmoid(m/2) to get something in [0,1/2]
    * At the ouput, you'd just have to multiply by 2*sigmoid(m/2).
    */
    doubleOut = oddSigmoid(doubleIn*modulus);           // Here, all the values in [0,m/2]
    doubleOut = 0.5*doubleOut/modsigmoid;               // Comment here if you don't want a rescaling
    testvect->coefsT[i] = dtot32(doubleOut);
  }
  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* The bit boot: outputs a value "doubleOut" if input is positive and 0 if not
* OBSOLETE but don't wanna remove it for fear of breaking some other code depending on it
*/

//template<typename TORUS>
void FTBootFFT::bitBoot(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    double doubleOut)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  // We want a value of doubleOut/2 in the testVector
  double testVectorCoef = doubleOut/2;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
    testvect->coefsT[i] = dtot32(testVectorCoef);

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // Then we need to add testVectorCoef to it
  LweSample * lweTemp = new_LweSample(extract_params);
  lweNoiselessTrivial(lweTemp, dtot32(testVectorCoef), extract_params);
  lweAddTo(lweOut, lweTemp, extract_params);

  // CLean up
  delete_LweSample(lweTemp);
  delete_TorusPolynomial(testvect);
}


/*
* The heaviside boot: outputs a value "doubleOut" if input is positive and 0 if not
*/

//template<typename TORUS>
void FTBootFFT::boot_Heaviside(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    double doubleOut)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  // We want a value of doubleOut/2 in the testVector
  double testVectorCoef = doubleOut/2;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
    testvect->coefsT[i] = dtot32(testVectorCoef);

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // Then we need to add testVectorCoef to it
  LweSample * lweTemp = new_LweSample(extract_params);
  lweNoiselessTrivial(lweTemp, dtot32(testVectorCoef), extract_params);
  lweAddTo(lweOut, lweTemp, extract_params);

  // CLean up
  delete_LweSample(lweTemp);
  delete_TorusPolynomial(testvect);
}

/*
* The sign boot: outputs a value "positiveOut" if input is positive and "-positiveOut" if not
*/

//template<typename TORUS>
void FTBootFFT::boot_Sign(LweSample * lweOut,
    LweBootstrappingKey * bk,
    LweSample * lweIn,
    double positiveOut)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  // We want a value of positiveOut in the testVector
  double testVectorCoef = positiveOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
    testvect->coefsT[i] = dtot32(testVectorCoef);

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // Clean up
  delete_TorusPolynomial(testvect);
}

void FTBootFFT::boot_Sign(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    double positiveOut)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  // We want a value of positiveOut in the testVector
  double testVectorCoef = positiveOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
    testvect->coefsT[i] = dtot32(testVectorCoef);

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // Clean up
  delete_TorusPolynomial(testvect);
}

/*
*********************************** HALF TORUS *******************************************
*/

/*
* Identity
*/
//template<typename TORUS>
void HTBootFFT::boot_Id(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;     // The input is centered around 0 here.
    doubleOut = doubleIn;                 // The identity here: in [-1/4,1/4]
    // doubleOut = 2*doubleOut;              // So that the output is in [-1/2,1/2]
    doubleOut = doubleOut;              // So that the output is in [-1/4,1/4]
    testvect->coefsT[i] = dtot32(doubleOut);
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Absolute value
*/
//template<typename TORUS>
void HTBootFFT::boot_Abs(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;     // The input is centered around 0 here.
    doubleOut = absolute(doubleIn);       // The absolute value here: in [-1/4,1/4]
    doubleOut = 2*doubleOut;              // So that the output is in [-1/2,1/2]
    testvect->coefsT[i] = dtot32(doubleOut);
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Power of
*/
//template<typename TORUS>
void HTBootFFT::boot_powerOf(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    int exponent)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;             // The input is centered around 0 here.
    doubleOut = pow(doubleIn, exponent);          // The power of here in [-1/4^e,1/4^e]
    doubleOut = doubleOut*pow(2,2*exponent-1);    // So that the output is in [-1/2,1/2] in the even case
    testvect->coefsT[i] = dtot32(doubleOut);
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Sigmoid
* this could use some optimizations I'm sure
*/
//template<typename TORUS>
void HTBootFFT::boot_Sigmoid(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    double modulus)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;
  const double modulusx2 = modulus*2;
  const double modulus_2 = modulus/2;

  const double modsigmoid = oddSigmoid(modulus_2);

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;             // The input is centered around 0 here.
    /*
    * New method here
    * We first apply a sigmoid of all possible int values (in [-m/2,m/2]) therefore getting something in [sigmoid(-m/2),sigmoid(m/2)]
    * Then we divide by 2*sigmoid(m/2) to get something in [-1/2,1/2]
    * At the ouput, you'd just have to multiply by 2*sigmoid(m/2).
    */
    doubleOut = oddSigmoid(doubleIn*modulusx2);     // Here all values in [-m/2,m/2]
    doubleOut = 0.5*doubleOut/modsigmoid;           // Comment here if you don't want a rescaling
    testvect->coefsT[i] = dtot32(doubleOut);
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
*********************************** SCALED HALF TORUS *******************************************
*/

/*
* Identity
* Importantly, the scale is with regard to the [-1/4,1/4] range
*/
//template<typename TORUS>
void SHTBootFFT::boot_Id(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;     // The input is centered around 0 here.
    doubleOut = doubleIn;                 // The identity here: in [-1/4,1/4]
    doubleOut = doubleOut/rescale;        // So that the output is in [-1/4,1/4]/rescale
    testvect->coefsT[i] = dtot32(doubleOut);
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(slweOut, bk, testvect, slweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Relu
* Importantly, the scale is with regard to the [-1/4,1/4] range
*/

void SHTBootFFT::boot_Relu(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;
  const int N_2 = N/2;

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;     // The input is centered around 0 here.
    if (i <= N/2)
      doubleOut = 0;                      // Just 0 for negative values
    else
      doubleOut = doubleIn;               // The identity here: in [-1/4,1/4]

    doubleOut = doubleOut/rescale;        // So that the output is in [-1/4,1/4]/rescale
    testvect->coefsT[i] = dtot32(doubleOut);
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(slweOut, bk, testvect, slweIn, N_2);

  // CLean up
  delete_TorusPolynomial(testvect);
}



/*
* Multiply
* Importantly, the scale is with regard to the [-1/4,1/4] range
*/
//template<typename TORUS>
void SHTBootFFT::boot_Mul(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double mul,
    double rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;     // The input is centered around 0 here.
    doubleOut = doubleIn*mul;             // The multiplication: in [-1/4,1/4]*mul
    doubleOut = doubleOut/rescale;        // So that the output is in [-1/4,1/4]/rescale
    testvect->coefsT[i] = dtot32(doubleOut);
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(slweOut, bk, testvect, slweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Absolute value
*/
//template<typename TORUS>
void SHTBootFFT::boot_Abs(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;     // The input is centered around 0 here.
    doubleOut = absolute(doubleIn);       // The absolute value here: in [-1/4,1/4]
    doubleOut = doubleOut/rescale;        // So that the output is in [-1/4,1/4]/rescale
    testvect->coefsT[i] = dtot32(doubleOut);
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(slweOut, bk, testvect, slweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Power of
*/
//template<typename TORUS>
void SHTBootFFT::boot_powerOf(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    int exponent,
    double rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  double doubleIn;
  double doubleOut;

  double default_rescaling = pow(4,exponent-1);

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;             // The input is centered around 0 here.
    doubleOut = pow(doubleIn, exponent);          // The power of here in [-1/4^e,1/4^e]
    doubleOut = doubleOut*default_rescaling;      // So that the output is in [-1/4,1/4] by default
    doubleOut = doubleOut/rescale;                // So that the output is in [-1/4,1/4]/rescale
    testvect->coefsT[i] = dtot32(doubleOut);
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(slweOut, bk, testvect, slweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}

/*
* Sigmoid
* this could use some optimizations I'm sure
*/
//template<typename TORUS>
void SHTBootFFT::boot_Sigmoid(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  // The max input value and therefore the max sigmoid value
  double max_in_value = 0.25 ;// slweIn->scale;
  double max_out_value = sigmoid(max_in_value);

  double doubleIn;
  double doubleOut;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);
  for (int i=0;i<N;i++)
  {
    doubleIn = (double) i/Nx2 - 0.25;                 // The input is in [-1/4,1/4]
    /*
    * We first apply a sigmoid of all possible input values rescaled back up
    * Then the output is in [0,1] and then in [0,1/4] and we rescale it with the "rescale" value
    */
    doubleOut = sigmoid(doubleIn);      // Here all values in [0,1]
    doubleOut = doubleOut/4;                          // Here all values in [0,1/4]
    doubleOut = doubleOut/rescale;                    // Here all values in [0,1/4rescale]
    testvect->coefsT[i] = dtot32(doubleOut);

    // if (i==0)
    //   std::cout << "Output simoid values:" << '\n';
    // std::cout << doubleOut << " ";
    // if (i==N-1)
    //   std::cout << '\n';
  }

  // Call the bootstrapping with the test vector
  ATBootFFT::extendedBoot(slweOut, bk, testvect, slweIn);

  // CLean up
  delete_TorusPolynomial(testvect);
}
