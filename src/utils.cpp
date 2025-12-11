#include "utils.h"
#include "tfhe.h"
#include "seal/seal.h"
using namespace std;
using namespace seal;

// Ciphertext

void seal2tfhe::ciphertext_new(TLweSample  * tlweCipher, seal::Ciphertext sealCipher, const seal::SEALContext &sealcontext)
{
  // Extract variables

  auto &context_data = *sealcontext.key_context_data();
  auto coeff_modulus = context_data.parms().coeff_modulus();
  uint32_t q = coeff_modulus[0].value();
  int N_seal = context_data.parms().poly_modulus_degree();

  int N_tfhe = 4096;
  
  

  // Temp variables
  Torus32 torusTemp;

  // The equivalent of a

  uint64_t * c2 = sealCipher.data(1);

  uint32_t * c1=(uint32_t*)malloc(sizeof(uint32_t)*N_tfhe);

  for (size_t i = 0; i < N_seal; i++)
     c1[i]= c2[i] & 0xFFFFFFFF; // This is to get the last 32 bits

  uint32_t c3;

  for (size_t i = 0; i < N_tfhe; i++)
  {
    c3 = (q-c1[i]);

    torusTemp = modSwitchToTorus32(c3, q);
    (&(tlweCipher->a[0]))->coefsT[i] = torusTemp;

  }
  
  // Fill b

  for (size_t i = 0; i < N_tfhe; i++)
  {
    uint64_t tempval1 = sealCipher[i];
    uint32_t tempval2 = tempval1 & 0xFFFFFFFF; // This is to get the last 32 bits 
    c3 = tempval2;
    torusTemp = modSwitchToTorus32(c3, q); 
    tlweCipher->b->coefsT[i] = torusTemp;   
  }
}

void seal2tfhe::ciphertext_seal_exit(TLweSample  * tlweCipher, seal::Ciphertext sealCipher, const seal::SEALContext &sealcontext)
{
  // Extract variables
  auto &context_data = *sealcontext.key_context_data();
  auto coeff_modulus = context_data.parms().coeff_modulus();
  uint64_t q = coeff_modulus[3].value();
  int N_seal = context_data.parms().poly_modulus_degree();
  int N_tfhe = 4096 ;
  Torus32 torusTemp;   // Temp variables

  uint64_t * c2 = sealCipher.data(1);
  uint32_t * c1=(uint32_t*)malloc(sizeof(uint32_t)*N_tfhe);
  for (size_t i = 0; i < N_tfhe; i++)
     {c1[i]= (c2[N_seal- N_tfhe + i]) & 0xFFFFFFFF; // This is to get the last 32 bits
     }
  uint32_t c3;

  for (size_t i = 0; i < N_tfhe; i++)
  {


    // There is a minus here
    // This is necessary because of the difference between TFHE and SEAL:
    // between b - sa (TFHE) and b + sa (SEAL)
    c3 = (q-c1[i]);
    torusTemp = modSwitchToTorus32(c3, q);
    (&(tlweCipher->a[0]))->coefsT[i] = torusTemp;

  }
  // Fill b

  for (size_t i = 0; i < N_tfhe; i++)
  {
    uint64_t tempval1 = sealCipher[N_seal - N_tfhe + i];
    uint32_t tempval2 = tempval1 & 0xFFFFFFFF; // This is to get the last 32 bits
    c3 = tempval2;
    torusTemp = modSwitchToTorus32(c3, q);
    tlweCipher->b->coefsT[i] = torusTemp;
  }

}


void tfhe2seal::ciphertext(seal::Ciphertext sealCipher, TLweSample * tlweCipher,const seal::SEALContext &sealContext)
{
  // Extract variables
  auto &context_data = *sealContext.key_context_data();
  int N = context_data.parms().poly_modulus_degree();
  uint32_t q = context_data.total_coeff_modulus()[0];

  // Temp variables
  uint32_t intTemp;

  // The equivalent of a

  uint64_t * c1=sealCipher.data(1);
  // Fill from a
  for (size_t i = 0; i < N; i++)
  {
    intTemp = q-modSwitchFromTorus32((&(tlweCipher->a[0]))->coefsT[i], q);
    c1[i] = intTemp;
  }

  // Fill from b
  for (size_t i = 0; i < N/4; i++)
  {
    intTemp = modSwitchFromTorus32(tlweCipher->b->coefsT[i], q);
    sealCipher[i] = intTemp;
  }
  for (size_t i = N/4+1; i < N; i++)
  {
    intTemp = modSwitchFromTorus32(0, q);
    sealCipher[i] = intTemp;
  }
}

// Key

void seal2tfhe::secretKey(TLweKey * tlweKey, seal::SecretKey sealKey, const seal::SEALContext &sealContext)
{
  // Extract variables
  auto &context_data = *sealContext.key_context_data();
  int N_seal = context_data.parms().poly_modulus_degree();
  auto coeff_modulus = context_data.parms().coeff_modulus();
  uint64_t q = coeff_modulus[0].value();
  int N_tfhe = 4096;
  Plaintext key_poly =  sealKey.data();
  uint64_t *key_coefs = key_poly.data();

  auto small_ntt_tables = context_data.small_ntt_tables();

  // Find the original key
  seal::util::inverse_ntt_negacyclic_harvey_lazy(key_coefs, small_ntt_tables[0]);
  int weird_count = 0;
  for (size_t i = 0; i < N_tfhe; i++)
  {
    if (key_coefs[i] == q-1)
      key_coefs[i] = -1;

    if ((key_coefs[i] == q) || (key_coefs[i] == 0))
      key_coefs[i] = 0;

    if ((key_coefs[i] == q + 1) || (key_coefs[i] == 1))
      key_coefs[i] = 1;

    tlweKey->key->coefs[i] = key_coefs[i];
  }
}



// Just generate the underlying TRLWE key

void seal2tfhe::secretTGSWKey(TGswKey  * tgswKey, seal::SecretKey sealKey,const seal::SEALContext &sealContext)
{
  seal2tfhe::secretKey(&(tgswKey->tlwe_key), sealKey, sealContext);
}


void tfhe2seal::secretKey(seal::SecretKey * sealKey, TLweKey * tlweKey, const seal::SEALContext &sealContext)
{
  // Extract encryption parameters.
  auto &context_data = *sealContext.key_context_data();
  auto &parms = context_data.parms();
  auto &coeff_modulus = parms.coeff_modulus();
  size_t coeff_count = parms.poly_modulus_degree();
  size_t coeff_mod_count = coeff_modulus.size();

  int N = context_data.parms().poly_modulus_degree();
  uint32_t q = context_data.total_coeff_modulus()[0];
   
  // Set the secret key
  Plaintext key_poly =  sealKey->data();
  uint64_t * secret_key = sealKey->data().data();

  for (size_t i = 0; i < N/4; i++)
  { 
     for (size_t j = 0; j < 4; j++)
        {
            size_t index = i * 4 + j;
            secret_key[index] = tlweKey->key->coefs[i];
        }
  }

  auto small_ntt_tables = context_data.small_ntt_tables();
  for (size_t i = 0; i < coeff_mod_count; i++)
  {
      // Transform the secret s into NTT representation.
      ntt_negacyclic_harvey(secret_key + (i * coeff_count), small_ntt_tables[i]);
  }

  }


  // TRGSW key from TRLWE key in TFHE
void tfheMod::tlwe2tgswKey(TGswKey  * tgswKey, TLweKey  * tlweKey)
{
  int N = tlweKey->params->N;
  for (size_t i = 0; i < N; i++)
  {
    tgswKey->key[0].coefs[i] = tlweKey->key[0].coefs[i];
    tgswKey->tlwe_key.key[0].coefs[i] = tlweKey->key[0].coefs[i];
  }
}


std::string FileEncrypt::xorEncryptDecrypt(const std::string& data, const std::string& key) {
    std::string result = data;
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] ^= key[i % key.size()];
    }
    return result;
}