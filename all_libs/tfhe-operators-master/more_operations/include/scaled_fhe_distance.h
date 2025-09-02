#ifndef SFHE_DISTANCE_H
#define SFHE_DISTANCE_H

#include <stdio.h>
#include <iostream>

// Main TFHE
#include "tfhe_gate_bootstrapping_functions.h"
#include "tfhe_gate_bootstrapping_structures.h"
#include "tfhe.h"
// I/O
#include "tfhe_io.h"
#include "tfhe_generic_streams.h"

// Scaled TFHE
#include "scaled_tfhe.h"

//template<typename TORUS>
struct fheScalarProduct
{
  /*
  * Compute an external scalar product: between a TLWE sample and a clear int polynomial
  * The array size is there to know which coefficient to extract from the product (array_size-1)
  * WARNING: this can only work if: array_size \leq N
  */
  static void externalPolyMethod(LweSample * lweOut, TLweSample * tlweIn, int * integerIn, const TLweParams * tlwe_params, int array_size);

  /*
  * Compute an external scalar product: between an LWE array and a clear int array
  * The two arrays should obviously have the same size
  */
  static void externalScalarMethod(LweSample * lweOut, LweSample * lweIn, int * integerIn, const LweParams * lwe_params, int array_size);

};

//template struct fheScalarProduct<Torus32>;
//template struct fheScalarProduct<Torus64>;

#endif
