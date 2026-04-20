#ifndef SCALED_TLWE_KEYSWITCH_H
#define SCALED_TLWE_KEYSWITCH_H

#include "tfhe_gate_bootstrapping_functions.h"
#include "tfhe_gate_bootstrapping_structures.h"
#include "tfhe.h"
#include "scaled_alloc.h"
#include "tfhe_generic_templates.h"
//#include "scaled_tlwe_functions.h"

//template<typename TORUS>
struct TLweKeySwitchKey{
    int32_t n; ///< length of the input key: s'
    int32_t t; ///< decomposition length
    int32_t basebit; ///< log_2(base)
    int32_t base; ///< decomposition base: a power of 2
    const TLweParams* out_params; ///< params of the output key s
    TLweSample* ks0_raw;  //array that contains all the TLwe samples of size n.t
    TLweSample** ks; 
    ///< the keyswitch elements: an n.t matrix
    // of size n pointing to ks0, an array where the elements are spaced by t positions

    TLweKeySwitchKey(int32_t n, int32_t t, int32_t basebit, const TLweParams* out_params, TLweSample* ks0_raw);
    ~TLweKeySwitchKey();
    TLweKeySwitchKey(const TLweKeySwitchKey&) = delete;
    void operator=(const TLweKeySwitchKey&) = delete;

    static void init_obj(
        TLweKeySwitchKey* obj,
        int32_t n,
        int32_t t,
        int32_t basebit,
        const TLweParams* out_params);

    static void destroy_obj(TLweKeySwitchKey* obj);

    // void write(Ostream& out_stream) const;
    // static TLweKeySwitchKey* read_new(Istream& inp_stream, const TLweParams* params);

    /**
    * Creates a Key Switching Key between the two keys
    */

};

typedef struct TLweKeySwitchKey TLweKeySwitchKey;

EXPORT inline TLweKeySwitchKey* new_TLweKeySwitchKey_array(int nbelts, int32_t n, int32_t t, int32_t basebit, const TLweParams* out_params) {
  TLweKeySwitchKey * obj = alloc_obj_array<TLweKeySwitchKey>(nbelts);
  for (int ii=0; ii<nbelts; ++ii)
    TLweKeySwitchKey::init_obj(obj+ii, n , t , basebit, out_params);
  return obj;
}

//template struct TLweKeySwitchKey<Torus32>;
//template struct TLweKeySwitchKey<Torus64>;

#endif
