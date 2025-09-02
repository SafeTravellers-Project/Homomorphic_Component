#include "priv_keyswitch.h"

/*
* Allocate link the ks key to the ks0_raw array
*/
//template<typename TORUS>
PrivKeySwitchKey::PrivKeySwitchKey(
        const int n,
        const int t,
        const int p,
        const int basebit,
        const TLweParams* out_params,
        TLweSample* ks0_raw)
        :
        n(n),
        t(t),
        p(p),
        basebit(basebit),
        base(1<<basebit),
        out_params(out_params),
        ks0_raw(ks0_raw)
{
    new_n = n+1;
    ks = new TLweSample**[new_n];
    for (int i = 0; i < new_n; ++i)
    {
      ks[i] = new TLweSample*[t];
      for (size_t j = 0; j < t; j++)
        ks[i][j] = ks0_raw + p*t*i + p*j;
    }
}

//template<typename TORUS>
PrivKeySwitchKey::~PrivKeySwitchKey() {
    delete[] ks;
}

/**
 * PrivKeySwitchKey constructor function
 */
//template<typename TORUS>
EXPORT void PrivKeySwitchKey::init_obj(PrivKeySwitchKey* obj, int n, int t, int p, int basebit, const TLweParams* out_params) {
    TLweSample* ks0_raw = new_TLweSample_array((n+1)*t*p, out_params);
    new(obj) PrivKeySwitchKey(n,t,p,basebit,out_params, ks0_raw);
}

/**
 * PrivKeySwitchKey destructor
 */
//template<typename TORUS>
EXPORT void PrivKeySwitchKey::destroy_obj(PrivKeySwitchKey* obj) {
    const int n = obj->n;
    const int t = obj->t;
    const int p = obj->p;
    const int new_n = obj->new_n;
    delete_TLweSample_array(new_n*t*p, obj->ks0_raw);
    obj->~PrivKeySwitchKey();
}

/*
* Creating a private key switching key with function f(x) = 1 * x and p=1
*/

//template<typename TORUS>
EXPORT void PrivKeySwitchKey::CreateKeySwitchKey_Id(
  PrivKeySwitchKey * result,
  const LweKey* in_key,
  const TLweKey* out_key)
{
  const int n=result->n;
  const int basebit=result->basebit;
  const int t=result->t;
  const int new_n=result->new_n;
  const double out_alpha = out_key->params->alpha_min;
  TLweSample*** ks = result->ks;   // The actual TRLWE samples
  const int* int_in_key = in_key->key;       // The binary key

  // Verify that we do have p = 1 for this key
  assert(result->p == 1);

  // Create the key for the as
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<t;j++)
    {
      Torus32 x=int_in_key[i]*(1<<(32-(j+1)*basebit));
      tLweSymEncryptT(&ks[i][j][0],x,out_alpha,out_key);    // Here we have p=1 so z=0
    }
  }
  // Create the key for the bs
  for(int j=0;j<t;j++)
  {
    Torus32 x=(-1)*(1<<(32-(j+1)*basebit));        // Writing it like this to make it clear that k_n = -1
    tLweSymEncryptT(&ks[n][j][0],x,out_alpha,out_key);        // Here we have p=1 so z=0
  }
}

/*
* Creating a private key switching key with function f(x) = - K * x and p=1
*/

//template<typename TORUS>
EXPORT void PrivKeySwitchKey::CreateKeySwitchKey_OutKey(
  PrivKeySwitchKey * result,
  const LweKey* in_key,
  const TLweKey* out_key)
{
  const int n=result->n;
  const int basebit=result->basebit;
  const int t=result->t;
  const int new_n=result->new_n;
  const double out_alpha = out_key->params->alpha_min;
  TLweSample*** ks = result->ks;         // The actual TRLWE samples
  const int* int_in_key = in_key->key;          // The binary key
  IntPolynomial * int_out_key = out_key->key;   // The binary polynomial key
  const int out_N = out_key->params->N;

  // Verify that we do have p = 1 for this key
  assert(result->p == 1);

  // The torus polynomial we will encrypt
  TorusPolynomial * xpoly = new_TorusPolynomial(out_N);

  // Create the key for the as
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<t;j++)
    {
      Torus32 x=int_in_key[i]*(1<<(32-(j+1)*basebit));
      // Fill the torus polynomial we encrypt by multiplying x with K, the output key
      for (size_t q = 0; q < out_N; q++)
        xpoly->coefsT[q] = - x * int_out_key->coefs[q];

      tLweSymEncrypt(&ks[i][j][0],xpoly,out_alpha,out_key);    // Here we have p=1 so z=0
    }
  }
  // Create the key for the bs
  for(int j=0;j<t;j++)
  {
    Torus32 x=(-1)*(1<<(32-(j+1)*basebit));        // Writing it like this to make it clear that k_n = -1
    // Fill the torus polynomial we encrypt by multiplying x with K, the output key
    for (size_t q = 0; q < out_N; q++)
      xpoly->coefsT[q] = - x * int_out_key->coefs[q];

    tLweSymEncrypt(&ks[n][j][0],xpoly,out_alpha,out_key);        // Here we have p=1 so z=0
  }
}


/**
* Gives the output of the private keyswitching operation given the decomposed inputs in the array as and bs
*/
//template<typename TORUS>
EXPORT void PrivKeySwitchKey::KeySwitch(
  TLweSample * result,
  PrivKeySwitchKey* privKS,
  const TLweParams * in_params,
  LweSample * samples)
{
  // Get all of the parameters
  int n = privKS->n;
  int t = privKS->t;
  int p = privKS->p;
  const int new_n = privKS->new_n;
  TLweSample *** ks = privKS->ks;

  // Mask and precision
  const int basebit = privKS->basebit;
  const int base=1<<basebit;
  const Torus32 prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;

  // Clear the output
  tLweClear(result,in_params);

  //typedef typename TorusUtils::UTORUS UTORUS;

  /*
  * We create as, an array of exactly n+1 vectors
  * every vector as[i] has the p ith a values from the input samples
  */
  Torus32 ** as = (Torus32 **) malloc(sizeof(Torus32*)*new_n);
  for (int i = 0; i < new_n; i++)
    as[i] = (Torus32*) malloc(sizeof(Torus32)*p);

  // For the as (from 0 to n-1)
  for (int i = 0; i < n; i++)
    for (int z = 0; z < p; z++)
      as[i][z] = (samples+z)->a[i];

  // For the bs (i = n)
  for (int z = 0; z < p; z++)
    as[n][z] = (samples+z)->b;

  /*
  * There are p a vectors (one for each initial ciphertext) values (index z)
  * Each of these vectors has n+1 coefficients (index i) this is because b is considered as a0
  */

  // Do the KS subtractions here
  for (int i = 0; i < new_n; i++)
  {
    for (int z = 0; z < p; z++)
    {
      // Approximate all the coefficients
      const uint32_t aizbar = as[i][z]+prec_offset;

      for (int j = 0; j < t; j++)
      {
        // Here, we further decompose each of the aiz coefficients into t binary values (index j)
        const uint32_t aizj = (aizbar>>(32-(j+1)*basebit)) & mask;

        tLweSubMulTo(result, aizj, &ks[i][j][z], in_params);            // - KS_{i,j,z} * aizj
      }
    }
  }

  // Clean up
  free(as);
}
