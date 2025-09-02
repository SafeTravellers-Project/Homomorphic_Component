#ifndef TEST_VECTOR_H
#define TEST_VECTOR_H

#include <stdio.h>
#include <iostream>
#include <math.h>

// Main TFHE
#include "tfhe_gate_bootstrapping_functions.h"
#include "tfhe_gate_bootstrapping_structures.h"
#include "tfhe.h"
// I/O
#include "tfhe_io.h"
#include "tfhe_generic_streams.h"

// Utils
#include "clear-utils.h"
// Scaled TFHE
#include "scaled_tfhe.h"

/**
* Here we create test vectors for different plaintext sizes and functions
* If one wants to use approximate arithmetic, one can set plaintext_size to N
* FTestVector takes inputs in [0,1/2]
*/


struct FTestVector
{
  // f(x) = x
  static void id(TorusPolynomial* testvect,
      const TLweParams* tlwe_params,
      int plaintext_size,
      int rescale);

  // f(x) = x - 1/4
  static void sub_fourth(TorusPolynomial* testvect,
      const TLweParams* tlwe_params,
      int plaintext_size,
      int rescale);
};


/**
* HTestVector takes inputs in [-1/4,1/4]
*/


struct HTestVector
{
  // f(x) = x
  static void id(TorusPolynomial* testvect,
      const TLweParams* tlwe_params,
      int plaintext_size,
      int rescale);
};




#endif
