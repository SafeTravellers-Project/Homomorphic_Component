#ifndef FUNXTENSIONS_H
#define FUNXTENSIONS_H

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

/*
* A mix between functions and extensions. You get a stroke when you try to pronounce it
*/

//template<typename TORUS>
struct Funxtensions
{
  /*
  * Compute an external mutiplication between a TLWE sample and an int polynomial.
  * For some reason this function does not exist in the original library is basically "AddMulRTo"
  * TODO : Here we implement it naively but it can be optimized by rewriting the functions in the TFHE library on torus polynomial multiplication
  */
  static void PolyExternMul(TLweSample * tlweOut,
    IntPolynomial * polyIn,
    TLweSample * tlweIn,
    const TLweParams * tlwe_params);

  /*
  * Compute a scaled external mutiplication between a TLWE sample and an int polynomial.
  * TODO : created a scaled IntPolynomial class
  */
};

//template struct Funxtensions<Torus32>;
//template struct Funxtensions<Torus64>;

#endif
