#ifndef SCALED_BOOT_H
#define SCALED_BOOT_H

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

// Scaled TFHE
#include "scaled_tfhe.h"
// Test vector creations
#include "test_vector.h"
// Extended bootstraps
#include "extended-bootstrap.h"

/**
* These functions call the bootstrapping operation after creating the appropriate test vector
* In order to used approximate arithmetic or if one just doesn't care, plaintext_size should be equal to N
*
*/

struct BootFFT
{
  // f(x) = x
  static void id(LweSample * slweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * slweIn,
      int plaintext_size,
      int rescale);

  // f(x) = x - 1/4
  static void sub_fourth(LweSample * slweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * slweIn,
      int plaintext_size,
      int rescale);
};



/**
* These functions call the bootstrapping operation after creating the appropriate test vector
* In order to used approximate arithmetic or if one just doesn't care, plaintext_size should be equal to N
*
* Half torus
*/
struct SHBootFFT
{
  // f(x) = x
  static void id(LweSample * slweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * slweIn,
      int plaintext_size,
      int rescale);
};



#endif
