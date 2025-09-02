#include "fhe_distance.h"

/*
* Polynomial external scalar product
*/

//template<typename TORUS>
void fheScalarProduct::externalPolyMethod(
  LweSample * lweOut,
  TLweSample * tlweIn,
  int * integerIn,
  const TLweParams * tlwe_params,
  int array_size)
{
  // Set parameters
  const int N = tlwe_params->N;
  const LweParams * lwe_params = &tlwe_params->extracted_lweparams;

  // Create an int polynomial from the int inputs
  IntPolynomial * polyIn = new_IntPolynomial(N);
  for (size_t i = 0; i < N; i++)
  {
    if (i < array_size)
      polyIn->coefs[i] = integerIn[array_size-1-i];   // Invert the inputs
    else
      polyIn->coefs[i] = 0;
  }

  // Multiply
  TLweSample * tlweTemp = new_TLweSample(tlwe_params);
  Funxtensions::PolyExternMul(tlweTemp, polyIn, tlweIn, tlwe_params);

  // Extract
  tLweExtractLweSampleIndex(lweOut, tlweTemp, array_size-1, lwe_params, tlwe_params);

  // Clean up
  delete_IntPolynomial(polyIn);
  delete_TLweSample(tlweTemp);
}

/*
* Scalar external scalar product
*/

//template<typename TORUS>
void fheScalarProduct::externalScalarMethod(
  LweSample * slweOut,
  LweSample * slweIn,
  int * sintIn,
  const LweParams * lwe_params,
  int array_size)
{
  // Initialize the output
  lweAddMulTo(slweOut, sintIn[0], slweIn, lwe_params);

  for (size_t i = 1; i < array_size; i++)
    lweAddMulTo(slweOut, sintIn[i], slweIn+i, lwe_params);
}

/*
* Internal scalar product
*/

//template<typename TORUS>
void fheScalarProduct::internalScalar(
  LweSample * slweOut,
  TGswSampleFFT * stgswIn,
  TLweSample * stlweIn,
  const TGswParams * tgsw_params)
{
	// Parameters
	const TLweParams * tlwe_params = tgsw_params->tlwe_params;
	const LweParams * lwe_params = &(tlwe_params->extracted_lweparams);
	const int N = tlwe_params->N;

  // Check that both polynomials have the same size. This can only work if they do
  // if (stgswIn->poly_degree != stlweIn->poly_degree)
  // {
  //   std::cout << "Polynomials must have the same size for the internal scalar product: " << stgswIn->poly_degree << " (TRGSW) != " << stlweIn->poly_degree << " (TRLWE)" << '\n';
  //   return;
  // }

//  int poly_degree = stgswIn->poly_degree;

  // Initialize temp TRLWE variable
	TLweSample * stlweOut = new_TLweSample(tlwe_params);

//   std::cout << "Before external mult: " << stlweIn->scale << " and " << stgswIn->scale << '\n';

	// External product
  stlweOut =stlweIn;
  tGswFFTExternMulToTLwe(stlweOut, stgswIn, tgsw_params);
//   std::cout << "After external mult: " << stlweOut->scale << '\n';

	// Extract
	tLweExtractLweSampleIndex(slweOut, stlweOut, N, lwe_params, tlwe_params);
  // std::cout << "After extraction: " << slweOut->scale << '\n';

	// Clean up
	delete_TLweSample(stlweOut);
}
