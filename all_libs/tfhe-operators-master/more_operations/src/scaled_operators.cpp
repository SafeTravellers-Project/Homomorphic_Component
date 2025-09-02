#include "scaled_operators.h"

/*
*********************************** SCALED HALF TORUS *******************************************
*/

//template<typename TORUS>
void SHTOperator::identity(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  LweSample * slweIn,
  double rescale)
{
  // Get parameters
  const LweParams * init_lwe_params = bk->in_out_params;

  // Temp LWE for rotation
  LweSample * slweTemp = new_LweSample(init_lwe_params);
  LweSample * lweFourth = new_LweSample(init_lwe_params);

  // Create an LWE 1/4
  Torus32 torusFourth = dtot32(0.25);
  lweNoiselessTrivial(lweFourth, torusFourth, init_lwe_params);

  // Copy the input into temp
  lweCopy(slweTemp, slweIn, init_lwe_params);

  // Rotate by 1/4
  lweAddTo(slweTemp, lweFourth, init_lwe_params);

  // The boot_id
  SHTBootFFT::boot_Id(slweOut, bk, slweTemp, rescale);

  // Update the scale
  //slweOut->scale = slweIn->scale * rescale;

  // Clean up
  delete_LweSample(slweTemp);
  delete_LweSample(lweFourth);
}

void SHTOperator::identity(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  LweSample * slweIn,
  int plaintext_size,
  double rescale)
{
  SHBootFFT::id(slweOut, bk, slweIn, plaintext_size, rescale);
}


void SHTOperator::relu(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  LweSample * slweIn,
  double rescale)
{
  // Get parameters
  const LweParams * init_lwe_params = bk->in_out_params;

  SHTBootFFT::boot_Relu(slweOut, bk, slweIn, rescale);

  // Update the scale
  //slweOut->scale = slweIn->scale * rescale;
}



//template<typename TORUS>
void SHTOperator::multiplication(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  LweSample * slweIn,
  double mul,
  double rescale)
{
  // Get parameters
  const LweParams * init_lwe_params = bk->in_out_params;

  // Temp LWE for rotation
  LweSample * slweTemp = new_LweSample(init_lwe_params);
  LweSample * lweFourth = new_LweSample(init_lwe_params);

  // Create an LWE 1/4
  Torus32 torusFourth = dtot32(0.25);
  lweNoiselessTrivial(lweFourth, torusFourth, init_lwe_params);

  // Copy the input into temp
  lweCopy(slweTemp, slweIn, init_lwe_params);

  // Rotate by 1/4
  lweAddTo(slweTemp, lweFourth, init_lwe_params);

  // The boot_id
  SHTBootFFT::boot_Mul(slweOut, bk, slweTemp, mul, rescale);

  // Update the scale
  //slweOut->scale = slweIn->scale * rescale;

  // Clean up
  delete_LweSample(slweTemp);
  delete_LweSample(lweFourth);
}

//template<typename TORUS>
void SHTOperator::absolute(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  LweSample * slweIn,
  double rescale)
{
  // Get parameters
  const LweParams* init_lwe_params = bk->in_out_params;


  // Temp LWE for rotation
  LweSample * slweTemp = new_LweSample(init_lwe_params);
  LweSample * lweFourth = new_LweSample(init_lwe_params);

  // Create an LWE 1/4
  Torus32 torusFourth = dtot32(0.25);
  lweNoiselessTrivial(lweFourth, torusFourth, init_lwe_params);

  // Copy the input into temp
  lweCopy(slweTemp, slweIn, init_lwe_params);

  // Rotate by 1/4
  lweAddTo(slweTemp, lweFourth, init_lwe_params);

  // The two boot_id
  SHTBootFFT::boot_Abs(slweOut, bk, slweTemp, rescale);

  // Update the scale
  //slweOut->scale = slweIn->scale * rescale;

  // Clean up
  delete_LweSample(slweTemp);
  delete_LweSample(lweFourth);
}

//template<typename TORUS>
void SHTOperator::powerOf(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    int exponent,
    double rescale)
{
  // Get parameters
  const LweParams* init_lwe_params = bk->in_out_params;

  // Temp LWE for rotation
  LweSample * slweTemp = new_LweSample(init_lwe_params);
  LweSample * lweFourth = new_LweSample(init_lwe_params);

  // Create an LWE 1/4
  Torus32 torusFourth = dtot32(0.25);
  lweNoiselessTrivial(lweFourth, torusFourth, init_lwe_params);
  lweCopy(slweTemp, slweIn, init_lwe_params);

  // Rotate by 1/4
  lweAddTo(slweTemp, lweFourth, init_lwe_params);

  // The two boot_id
  SHTBootFFT::boot_powerOf(slweOut, bk, slweTemp, exponent, rescale);

  // Update the scale
  double default_rescaling = pow(4,exponent-1);   // This is a rescaling done by default to keep the values in [-1/4,1/4]

 //  slweOut->scale = pow(slweIn->scale, exponent) * rescale / default_rescaling;

  // slweOut->scale = slweIn->scale / default_rescaling;

  // Clean up
  delete_LweSample(slweTemp);
  delete_LweSample(lweFourth);
}

//template<typename TORUS>
void SHTOperator::Sigmoid(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double rescale)
{
  // Get parameters
  const LweParams* init_lwe_params = bk->in_out_params;


  // Temp LWE for rotation
  LweSample * slweTemp = new_LweSample(init_lwe_params);
  LweSample * lweFourth = new_LweSample(init_lwe_params);

  // Create an LWE 1/4
  double doubleFourth = 0.25;
  Torus32 torusFourth = dtot32(doubleFourth);
  lweNoiselessTrivial(lweFourth, torusFourth, init_lwe_params);

  // Copy the input into temp
  lweCopy(slweTemp, slweIn, init_lwe_params);

  // Rotate by 1/4
  lweAddTo(slweTemp, lweFourth, init_lwe_params);

  // The two boot_id
  SHTBootFFT::boot_Sigmoid(slweOut, bk, slweTemp, rescale);

  // Update the scale
 // slweOut->scale = rescale * 4;

  // Clean up
  delete_LweSample(slweTemp);
  delete_LweSample(lweFourth);
}

/*
*********************************** SCALED FULL TORUS *******************************************
*/

// Will output "positive" if positive and 0 if not
// It is rescaled so that the output if decryted is either 0 or 1.
//template<typename TORUS>
void SFTOperator::heaviside(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  LweSample * slweIn,
  double positive)
{
  // The boot heaviside
  FTBootFFT::boot_Heaviside(slweOut, bk, slweIn, positive);

  // Update the scale
 // slweOut->scale = 1/positive;

}

// Will output "positive" if positive and "-positive" if not
// It is rescaled so that the output if decryted is either -1 or 1.
//template<typename TORUS>
void SFTOperator::sign(LweSample * slweOut,
  LweBootstrappingKey * bk,
  LweSample * slweIn,
  double positive)
{
  // The boot heaviside
  FTBootFFT::boot_Sign(slweOut, bk, slweIn, positive);

  // Update the scale
 // slweOut->scale = 1/positive;
}

void SFTOperator::sign(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  LweSample * slweIn,
  double positive)
{
  // The boot heaviside
  SFTBootFFT::boot_Sign(slweOut, bk, slweIn, positive);

  // Update the scale
  //slweOut->scale = 1/positive;
}



// f(x) = |x|

void SFTOperator::absolute(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  LweSample * slweIn,
  int plaintext_size,
  int rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;

  // Variable in which to store the value to sum at the output of the boot
  LweSample * lweTemp = new_LweSample(extract_params);

  // 1/(4*rescale)
  Torus32 torusTemp = dtot32(0.25/rescale);
  lweNoiselessTrivial(lweTemp, torusTemp, extract_params);

  // x/rescale - 1/(4*rescale)
  BootFFT::sub_fourth(slweOut, bk, slweIn, plaintext_size, rescale);

  // + 1/(4*rescale)
lweAddTo(slweOut, lweTemp, extract_params);

  // Clean up
  delete_LweSample(lweTemp);
}

// f(x) = x

void FTOperator::identity(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk1,
  LweBootstrappingKeyFFT * bk2,
  LweSample * slweIn,
  int plaintext_size,
  int rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk1->accum_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;

  // Variable in which to store the first boot result
  LweSample * slweTemp = new_LweSample(extract_params);

  // The boot where the function is just the id with no rescaling
  BootFFT::id(slweTemp, bk1, slweIn, plaintext_size, 1);

  // The boot where the function is the id with the rescaling
  BootFFT::id(slweOut, bk2, slweTemp, plaintext_size, rescale);

  // Clean up
  delete_LweSample(slweTemp);
}

// f(x) = x if x > 0
//      = 0 if x < 0

void FTOperator::relu(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk1,
  LweBootstrappingKeyFFT * bk2,
  LweSample * slweIn,
  int plaintext_size,
  int rescale)
{

  // Get parameters
  const TLweParams* accum_params = bk1->accum_params;
  const LweParams* lwe_params_2 = &accum_params->extracted_lweparams;
  const LweParams* lwe_params_1 = bk1->in_out_params;

  // Variable in which to store the intermidiate results
  LweSample * slweTemp1 = new LweSample(lwe_params_1);
  LweSample * slweTemp2 = new LweSample(lwe_params_2);

  /*
  * Now for the computation
  */

  // f(x) = |x|/2
  FTOperator::absolute(slweTemp2, bk1, slweIn, plaintext_size, 2*rescale);
  //slweTemp2->scale = slweTemp2->scale/2;    // here the rescaling by two is not a real rescaling we are actually dividing by 2

  // f(x) = x/2
  FTOperator::identity(slweTemp1, bk1, bk2, slweIn, plaintext_size, 2*rescale);
  //slweTemp1->scale = slweTemp1->scale/2;    // here the rescaling by two is not a real rescaling we are actually dividing by 2

  // keyswitch temp1 from 1->2
  lweKeySwitch(slweOut, bk2->ks, slweTemp1);

  // add
  lweAddTo(slweOut, slweTemp2, lwe_params_2);

  // Clean up
  delete_LweSample(slweTemp1);
  delete_LweSample(slweTemp2);
}
