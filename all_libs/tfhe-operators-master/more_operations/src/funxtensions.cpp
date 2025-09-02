#include "funxtensions.h"

/*
* The external multiplication (the naive way)
*/

//template<typename TORUS>
void Funxtensions::PolyExternMul(
  TLweSample * tlweOut,
  IntPolynomial * polyIn,
  TLweSample * tlweIn,
  const TLweParams* tlwe_params)
{
  // Parameters
  const int N = tlwe_params->N;

  // Set tlweOut to be an encryption of 0
  TorusPolynomial * polyZero = new_TorusPolynomial(N);
  torusPolynomialClear(polyZero);                                   // Set it to 0
  tLweNoiselessTrivial(tlweOut, polyZero, tlwe_params);

  // Apply the "AddMulRTo" function
  tLweAddMulRTo(tlweOut, polyIn, tlweIn, tlwe_params);

  // Clean up
  delete_TorusPolynomial(polyZero);
}

/*
* The scaled external multiplication (the naive way)
*/

//template<typename TORUS>

