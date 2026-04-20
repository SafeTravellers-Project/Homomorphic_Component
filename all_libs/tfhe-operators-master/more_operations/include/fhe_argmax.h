#ifndef FHE_ARGMAX_H
#define FHE_ARGMAX_H

#include <stdio.h>
#include <iostream>

// Main TFHE
#include "tfhe_gate_bootstrapping_functions.h"
#include "tfhe_gate_bootstrapping_structures.h"
#include "tfhe.h"
// I/O
#include "tfhe_io.h"
#include "tfhe_generic_streams.h"

//template<typename TORUS>
struct fheArgmax
{
  /*
  * Create the delta matrix from the labels matrix
  * - lweSummedLabels : the encryptions of the values from which we want to find the max
  * - d :  the number of classes and the size of the lweSummedLabels array of ciphertexts
  * - boot_key_fft : the bootstrapping key
  * - init_lwe_params : the parameters in which the lweSummedLabels ciphertexts are encrypted
  * - out_lwe_params : the output parameters of the bootstrapping key boot_key_fft
  * - print_prog : a boolean determining whether to print the progression of the computation since it can be quite long
  */

  static LweSample *** compute_DeltaMatrix(LweSample ** lweSummedLabels, int d, LweBootstrappingKeyFFT * boot_key_fft, const LweParams * init_lwe_params, const LweParams * out_lwe_params, int print_prog);

  /*
  * Delete the delta matrix from the labels matrix
  */

  static void delete_DeltaMatrix(LweSample *** lweDeltas, int d);

  /*
  * Compute the argmax given the delta matrix
  */

  static LweSample * argmax_from_DeltaMatrix(LweSample *** lweDeltas, int d, LweBootstrappingKeyFFT * boot_key_fft2, const LweParams * init_lwe_params, const LweParams * out_lwe_params);

};

//template struct fheArgmax<Torus32>;
//template struct fheArgmax<Torus64>;

#endif
