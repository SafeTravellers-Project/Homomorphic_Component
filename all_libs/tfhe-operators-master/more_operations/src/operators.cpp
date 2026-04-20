#include "operators.h"

/*
* The identity function
*/

//template<typename TORUS>
void FTOperator::identity(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk1,
    LweBootstrappingKeyFFT * bk2,
    LweSample * lweIn)
{
  // Get parameters
  const TLweParams* accum_params = bk1->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;

  // Variable in which to store the first boot result
  LweSample * lweTemp = new_LweSample(extract_params);

  // The two boot_id
  FTBootFFT::boot_Id(lweTemp, bk1, lweIn);
  FTBootFFT::boot_Id(lweOut, bk2, lweTemp);

  // Clean up
  delete_LweSample(lweTemp);
}

/*
* The absolute value
*/

//template<typename TORUS>
void FTOperator::absoluteValue(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* extract_params = bk->extract_params;
  const LweParams* init_lwe_params = bk->in_out_params;
  const int N=accum_params->N;
  const int Nx4= 4*N;

  // Get g_f(x)
  FTBootFFT::boot_Id_over2(lweOut, bk, lweIn);

  // Get g_f(-x)
  // LweSample * lweNegIn = new_LweSample(init_lwe_params);
  // LweFunctions::Negate(lweNegIn,lweIn,init_lwe_params);
  //
  LweSample * lweTemp = new_LweSample(extract_params);
  // FTBootFFT::boot_Id_over2(lweTemp, bk, lweNegIn);
  FTBootFFT::boot_NegId_over2(lweTemp, bk, lweIn);

  // Add them together: g_f(x) + g_f(-x)
  lweAddTo(lweOut, lweTemp, extract_params);

  // Create 1/4
  LweSample * lweFourth = new_LweSample(extract_params);
  Torus32 torusHalf = modSwitchToTorus32(1, 4);
  lweNoiselessTrivial(lweFourth, torusHalf, extract_params);

  // Add 1/4
  lweAddTo(lweOut, lweFourth, extract_params);

  // Clean up
  delete_LweSample(lweFourth);
  // delete_LweSample(lweNegIn);
  delete_LweSample(lweTemp);
}

/*
* X^3
*/

//template<typename TORUS>
void FTOperator::xcube(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk1,
    LweBootstrappingKeyFFT * bk2,
    LweSample * lweIn)
{
  // Get parameters
  const TLweParams* accum_params = bk1->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;

  // Variable in which to store the first boot result
  LweSample * lweTemp = new_LweSample(extract_params);

  // The two boot_id
  FTBootFFT::boot_Id(lweTemp, bk1, lweIn);
  FTBootFFT::boot_powerOf(lweOut, bk2, lweTemp,3);

  // Clean up
  delete_LweSample(lweTemp);
}

/*
* X^e where e is odd
*/

//template<typename TORUS>
void FTOperator::OddpowerOf(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk1,
    LweBootstrappingKeyFFT * bk2,
    LweSample * lweIn,
    int exponent)
{
  // Get parameters
  const TLweParams* accum_params = bk1->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;

  // Variable in which to store the first boot result
  LweSample * lweTemp = new_LweSample(extract_params);

  // The boot id and the odd function
  FTBootFFT::boot_Id(lweTemp, bk1, lweIn);
  FTBootFFT::boot_powerOf(lweOut, bk2, lweTemp,exponent);

  // Clean up
  delete_LweSample(lweTemp);
}

/*
* X^e where e is even
*/

//template<typename TORUS>
void FTOperator::EvenpowerOf(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk1,
    LweBootstrappingKeyFFT * bk2,
    LweSample * lweIn,
    int exponent)
{
  // Get parameters
  const TLweParams* accum_params = bk1->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;
  const LweParams* init_lwe_params = bk1->in_out_params;

  // Variable in which to store the first boot result
  LweSample * lweTempIn = new_LweSample(init_lwe_params);
  LweSample * lweTempOut = new_LweSample(extract_params);

  // The boot id
  FTBootFFT::boot_Id(lweTempOut, bk1, lweIn);
  // Our function
  FTBootFFT::boot_powerOf(lweTempIn, bk2, lweTempOut,exponent);
  // The absolute value
  FTOperator::absoluteValue(lweOut, bk1, lweTempIn);

  // Clean up
  delete_LweSample(lweTempIn);
  delete_LweSample(lweTempOut);
}

/*
* Sigmoid
*/

//template<typename TORUS>
void FTOperator::Sigmoid(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk1,
    LweBootstrappingKeyFFT * bk2,
    LweSample * lweIn,
    double modulus)
{
  // Get parameters
  const TLweParams* accum_params = bk1->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;

  // Variable in which to store the first boot result
  LweSample * lweTemp = new_LweSample(extract_params);

  // The boot id and the odd function
  FTBootFFT::boot_Id(lweTemp, bk1, lweIn);
  FTBootFFT::boot_Sigmoid(lweOut, bk2, lweTemp, modulus);

  // Clean up
  delete_LweSample(lweTemp);
}

/*
*********************************** HALF TORUS *******************************************
*/

//template<typename TORUS>
void HTOperator::identity(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn)
{
  // Get parameters
  const LweParams* init_lwe_params = bk->in_out_params;


  // Temp LWE for rotation
  LweSample * lweTemp = new_LweSample(init_lwe_params);
  LweSample * lweFourth = new_LweSample(init_lwe_params);

  // Create an LWE 1/4
  double doubleFourth = 0.25;
  Torus32 torusFourth = dtot32(doubleFourth);
  lweNoiselessTrivial(lweFourth, torusFourth, init_lwe_params);

  // Copy the input into temp
  lweCopy(lweTemp, lweIn, init_lwe_params);

  // Rotate by 1/4
  lweAddTo(lweTemp, lweFourth, init_lwe_params);

  // The ht boot_id
  HTBootFFT::boot_Id(lweOut, bk, lweTemp);

  // Clean up
  delete_LweSample(lweTemp);
  delete_LweSample(lweFourth);
}

//template<typename TORUS>
void HTOperator::absoluteValue(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn)
{
  // Get parameters
  const LweParams* init_lwe_params = bk->in_out_params;


  // Temp LWE for rotation
  LweSample * lweTemp = new_LweSample(init_lwe_params);
  LweSample * lweFourth = new_LweSample(init_lwe_params);

  // Create an LWE 1/4
  double doubleFourth = 0.25;
  Torus32 torusFourth = dtot32(doubleFourth);
  lweNoiselessTrivial(lweFourth, torusFourth, init_lwe_params);

  // Copy the input into temp
  lweCopy(lweTemp, lweIn, init_lwe_params);

  // Rotate by 1/4
  lweAddTo(lweTemp, lweFourth, init_lwe_params);

  // The two boot_id
  HTBootFFT::boot_Abs(lweOut, bk, lweTemp);

  // Clean up
  delete_LweSample(lweTemp);
  delete_LweSample(lweFourth);
}

//template<typename TORUS>
void HTOperator::powerOf(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    int exponent)
{
  // Get parameters
  const LweParams* init_lwe_params = bk->in_out_params;


  // Temp LWE for rotation
  LweSample * lweTemp = new_LweSample(init_lwe_params);
  LweSample * lweFourth = new_LweSample(init_lwe_params);

  // Create an LWE 1/4
  double doubleFourth = 0.25;
  Torus32 torusFourth = dtot32(doubleFourth);
  lweNoiselessTrivial(lweFourth, torusFourth, init_lwe_params);

  // Copy the input into temp
  lweCopy(lweTemp, lweIn, init_lwe_params);

  // Rotate by 1/4
  lweAddTo(lweTemp, lweFourth, init_lwe_params);

  // The two boot_id
  HTBootFFT::boot_powerOf(lweOut, bk, lweTemp, exponent);

  // Clean up
  delete_LweSample(lweTemp);
  delete_LweSample(lweFourth);
}

//template<typename TORUS>
void HTOperator::Sigmoid(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    double modulus)
{
  // Get parameters
  const LweParams* init_lwe_params = bk->in_out_params;


  // Temp LWE for rotation
  LweSample * lweTemp = new_LweSample(init_lwe_params);
  LweSample * lweFourth = new_LweSample(init_lwe_params);

  // Create an LWE 1/4
  double doubleFourth = 0.25;
  Torus32 torusFourth = dtot32(doubleFourth);
  lweNoiselessTrivial(lweFourth, torusFourth, init_lwe_params);

  // Copy the input into temp
  lweCopy(lweTemp, lweIn, init_lwe_params);

  // Rotate by 1/4
  lweAddTo(lweTemp, lweFourth, init_lwe_params);

  // The two boot_id
  HTBootFFT::boot_Sigmoid(lweOut, bk, lweTemp, modulus);

  // Clean up
  delete_LweSample(lweTemp);
  delete_LweSample(lweFourth);
}
