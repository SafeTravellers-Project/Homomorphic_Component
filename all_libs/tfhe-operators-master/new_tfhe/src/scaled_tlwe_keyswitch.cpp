#include "scaled_tlwe_keyswitch.h"


//template<typename TORUS>
TLweKeySwitchKey::TLweKeySwitchKey(
        const int n,
        const int t,
        const int basebit,
        const TLweParams* out_params,
        TLweSample* ks0_raw)
        :
        n(n),
        t(t),
        basebit(basebit),
        base(1<<basebit),
        out_params(out_params),
        ks0_raw(ks0_raw)
{
    ks = new TLweSample*[n];
    for (int i = 0; i < n; ++i)
        ks[i] = ks0_raw + t*i;
}

//template<typename TORUS>
TLweKeySwitchKey::~TLweKeySwitchKey() {
    delete[] ks;
}

/**
 * TLweKeySwitchKey constructor function
 */
//template<typename TORUS>
EXPORT void TLweKeySwitchKey::init_obj(TLweKeySwitchKey* obj, int n, int t, int basebit, const TLweParams* out_params) {
    TLweSample* ks0_raw = new_TLweSample_array(n*t, out_params);
    new(obj) TLweKeySwitchKey(n,t,basebit,out_params, ks0_raw);
}

/**
 * TLweKeySwitchKey destructor
 */
//template<typename TORUS>
EXPORT void TLweKeySwitchKey::destroy_obj(TLweKeySwitchKey* obj) {
    const int n = obj->n;
    const int t = obj->t;
    delete_TLweSample_array(n*t, obj->ks0_raw);
    obj->~TLweKeySwitchKey();
}

/*
* Public functional key switch key creation
*/

//template<typename TORUS>
