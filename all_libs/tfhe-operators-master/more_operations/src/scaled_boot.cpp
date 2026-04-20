#include "scaled_boot.h"

// ------------------------- FULL TORUS -----------------------------------

// f(x) = x

void BootFFT::id(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    int plaintext_size,
    int rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);

  // Fill the test vector
  FTestVector::id(testvect, accum_params, plaintext_size, rescale);

  // Call the bootstrapping with the test vector and extract at N/2*plaintext_size
  int extraction_index = N/(2*plaintext_size);

  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn, extraction_index);

  // Rescale
 // slweOut->scale = slweIn->scale * rescale;

  // Clean up
  delete_TorusPolynomial(testvect);
}

// f(x) = x - 1/4

void BootFFT::sub_fourth(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    int plaintext_size,
    int rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);

  // Fill the test vector
  FTestVector::sub_fourth(testvect, accum_params, plaintext_size, rescale);

  // Call the bootstrapping with the test vector and extract at N/2*plaintext_size
  int extraction_index = N/(2*plaintext_size);

  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn, extraction_index);

  // Rescale
//  slweOut->scale = slweIn->scale * rescale;

  // Clean up
  delete_TorusPolynomial(testvect);
}

// ------------------------- HALF TORUS -----------------------------------

// f(x) = x

void SHBootFFT::id(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    int plaintext_size,
    int rescale)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const int N=accum_params->N;

  // Create the test vector
  TorusPolynomial* testvect = new_TorusPolynomial(N);

  // Fill the test vector
  HTestVector::id(testvect, accum_params, plaintext_size, rescale);

  // Call the bootstrapping with the test vector and extract
  int extraction_index = N/(4*plaintext_size) + N/2;

  ATBootFFT::extendedBoot(lweOut, bk, testvect, lweIn, extraction_index);

  // Rescale
//  slweOut = slweIn * rescale;

  // Clean up
  delete_TorusPolynomial(testvect);
}
