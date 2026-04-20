#include "scaled_private_operators.h"

/*
*********************************** SCALED PRIVATE HALF TORUS *******************************************
*/

//template<typename TORUS>
void PHTOperator::heaviside(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  TLweKeySwitchKey * key_switch_key,
  LweSample * slweIn,
  LweSample * slweSign)
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
  HTPBootFFT::boot_Sign(slweOut, bk, key_switch_key, slweIn, slweSign);

  // Update the scale
  //slweOut->scale = slweSign->scale;

  // Clean up
  delete_LweSample(slweTemp);
  delete_LweSample(lweFourth);
}

// The output is the values in the test vector with the input in [-1/4,1/4]
// The output of 0 is testvector[0] and so on with the output of 1/4 testVector[N/2]
//template<typename TORUS>
void PHTOperator::unknown(LweSample * slweOut,
  LweBootstrappingKeyFFT * bk,
  TLweSample* testvect,
  LweSample * slweIn)
{
  // The boot_id
  PBootFFT::extendedBoot(slweOut, bk, testvect, slweIn);

  // Update the scale
 // slweOut->scale = testvect->scale;
}
