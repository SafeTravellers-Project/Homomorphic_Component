#ifndef MORE_TFHE_STRUCTURES_H
#define MORE_TFHE_STRUCTURES_H

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
struct LweArray {
    const LweParams * params;
    LweSample * samples;

    LweArray(const LwePublicParams* publicParams);
    ~LwePublicKey();
    LweArray(const LwePublicKey&) = delete; //forbidden
    LweArray* operator=(const LwePublicKey&) = delete; //forbidden
};

/** allocates and initializes (constructor) a single LweArray
 * @param params the LWE public parameters to use
 */
//template<typename TORUS>
inline LwePublicKey* new_LwePublicKey(const LweParams* params) {
  return new_obj<LwePublicKey>(publicParams);
}

//template struct LweArray<Torus32>;
//template struct LweArray<Torus64>;

#endif
