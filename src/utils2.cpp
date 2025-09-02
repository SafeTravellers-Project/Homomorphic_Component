#include "utils.h"
#include "tfhe.h"
#include "seal/seal.h"
using namespace std;
using namespace seal;

// Ciphertext
////template<typename TORUS>
void seal2tfhe::ciphertext_new(TLweSample  * tlweCipher, seal::Ciphertext sealCipher, const seal::SEALContext &sealcontext)
{
  // Extract variables
//  std::cout << "Inside seal2tfhe step 1" << '\n';
  auto &context_data = *sealcontext.key_context_data();
  auto coeff_modulus = context_data.parms().coeff_modulus();
  uint32_t q = coeff_modulus[0].value();
  int N_seal = context_data.parms().poly_modulus_degree();
  //int N_tfhe = tlweCipher->a[0].params->N;
  int N_tfhe = 1024;
  
  
//  uint32_t q = context_data.total_coeff_modulus()[0];
 // std::cout << "Asking of variabels done." << '\n';
  // Temp variables
  Torus32 torusTemp;

  // The equivalent of a
  //const uint64_t * c1 = sealCipher.data(1);
  uint64_t * c2 = sealCipher.data(1);
 // cout << "Here 1 " << '\n';
  uint32_t * c1=(uint32_t*)malloc(sizeof(uint32_t)*N_tfhe);
//cout << "Here 2 " << '\n';
  for (size_t i = 0; i < N_seal; i++)
     c1[i]= c2[i] & 0xFFFFFFFF; // This is to get the last 32 bits
 // cout << "Here 3 " << '\n';
  //for (int i =0 ; i < N ; i++)
  //   cout << " " << c2[i] ;
  //cout << '\n';
  // Fill a
  //std::cout << "Filling a" << '\n';
  //std::cout << "Value of q: " << q <<  '\n';
  uint32_t c3;

  for (size_t i = 0; i < N_tfhe; i++)
  {
    // There is no minus here because the TLWE decryption function was changed
    // This is necessary because of the difference between TFHE and SEAL:
    // between b - sa (TFHE) and b + sa (SEAL)
    // torusTemp = TorusUtils ::modSwitchToTorus64bits(c1[i], q);

    // There is a minus here
    // This is necessary because of the difference between TFHE and SEAL:
    // between b - sa (TFHE) and b + sa (SEAL)
    //torusTemp = modSwitchToTorus32(q-c1[i], q);
    //torusTemp = modSwitchToTorus32(q-c1[i], q);
    //cout << "Here 4 " << '\n';
    c3 = (q-c1[i]);

    torusTemp = modSwitchToTorus32(c3, q);

    //torusTemp = q-c1[i];
    //torusTemp = modSwitchToTorus32(q-c1[i], q);
    (&(tlweCipher->a[0]))->coefsT[i] = torusTemp;
      //cout << "Here 5 in loop if  " << i+1 << " for torus value : " << torusTemp << '\n';
  }
  //std::cout << "Value of q: " << q <<  '\n';
  //uint64_t interv = ((UINT64_C(1)<<63)/q)*2;
  //std::cout << "interv val: " << interv << '\n';
  //std::cout << "Comparing c1 and c2: " << c1[0] << " " << c1[1] << '\n';
  //std::cout << "Comparing c1 and c2 with q: " << q-c1[0] << " " << q-c1[1] << '\n';
  //std::cout << "Value of the TLWE ciphertexts a: " << (&(tlweCipher->a[0]))->coefsT[0] << " " << (&(tlweCipher->a[0]))->coefsT[1] << '\n';
  // Fill b
  //std::cout << "Filling b" << '\n';
  for (size_t i = 0; i < N_tfhe; i++)
  {
    uint64_t tempval1 = sealCipher[i];
    uint32_t tempval2 = tempval1 & 0xFFFFFFFF; // This is to get the last 32 bits
    //std::cout << "Printing in seal2tfhee values: " << tempval1 << " "<< tempval2 << '\n';
    c3 = tempval2;
    torusTemp = modSwitchToTorus32(c3, q);
    //torusTemp = tempval2;
    tlweCipher->b->coefsT[i] = torusTemp;
    //std::cout << "Here 6 in loop of b " << torusTemp << '\n';
  }
  //std::cout << "Value of the TLWE ciphertexts b " << tlweCipher->b->coefsT[0] << " " << tlweCipher->b->coefsT[1] << '\n';
  //std::cout << "Going out of seal2tfhe" << '\n';
  //std::cout << "Value of q: " << q <<  '\n';
  //free(c1);
}

void seal2tfhe::ciphertext_seal_exit(TLweSample  * tlweCipher, seal::Ciphertext sealCipher, const seal::SEALContext &sealcontext)
{
  // Extract variables
  //std::cout << "Inside seal2tfhe step 1" << '\n';
  auto &context_data = *sealcontext.key_context_data();
  auto coeff_modulus = context_data.parms().coeff_modulus();
  uint64_t q = coeff_modulus[0].value();
  int N_seal = context_data.parms().poly_modulus_degree();
  //int N_tfhe = tlweCipher->a[0].params->N;
  int N_tfhe = 1024;
  Torus32 torusTemp;   // Temp variables

  uint64_t * c2 = sealCipher.data(1);
  uint32_t * c1=(uint32_t*)malloc(sizeof(uint32_t)*N_tfhe);
  for (size_t i = 0; i < N_tfhe; i++)
     c1[i]= c2[N_seal- N_tfhe + i] & 0xFFFFFFFF; // This is to get the last 32 bits
  uint32_t c3;

  for (size_t i = 0; i < N_tfhe; i++)
  {
    // There is no minus here because the TLWE decryption function was changed
    // This is necessary because of the difference between TFHE and SEAL:
    // between b - sa (TFHE) and b + sa (SEAL)
    // torusTemp = TorusUtils ::modSwitchToTorus64bits(c1[i], q);

    // There is a minus here
    // This is necessary because of the difference between TFHE and SEAL:
    // between b - sa (TFHE) and b + sa (SEAL)
    //torusTemp = modSwitchToTorus32(q-c1[i], q);
    //torusTemp = modSwitchToTorus32(q-c1[i], q);
    //cout << "Here 4 " << '\n';
    //cout << "Value of c1[i]: " << c1[i] << '\n';
    c3 = (q-c1[i]);
    //cout << "Value of c3: " << c3 << '\n';
    torusTemp = modSwitchToTorus32(c3, q);

    //torusTemp = q-c1[i];
    //torusTemp = modSwitchToTorus32(q-c1[i], q);
    (&(tlweCipher->a[0]))->coefsT[i] = torusTemp;
      //cout << "Here 5 in loop if  " << i+1 << " for torus value : " << torusTemp << '\n';
  }
  //std::cout << "Value of q: " << q <<  '\n';
  //uint64_t interv = ((UINT64_C(1)<<63)/q)*2;
  //std::cout << "interv val: " << interv << '\n';
  //std::cout << "Comparing c1 and c2: " << c1[0] << " " << c1[1] << '\n';
  //std::cout << "Comparing c1 and c2 with q: " << q-c1[0] << " " << q-c1[1] << '\n';
  //std::cout << "Value of the TLWE ciphertexts a: " << (&(tlweCipher->a[0]))->coefsT[0] << " " << (&(tlweCipher->a[0]))->coefsT[1] << '\n';
  // Fill b
  //std::cout << "Filling b" << '\n';
  for (size_t i = 0; i < N_tfhe; i++)
  {
    uint64_t tempval1 = sealCipher[N_seal - N_tfhe + i];
    uint32_t tempval2 = tempval1 & 0xFFFFFFFF; // This is to get the last 32 bits
   // std::cout << "Printing in seal2tfhee values: " << tempval1 << " "<< tempval2 << '\n';
    //c3 = tempval2;
    c3 = tempval2;
    torusTemp = modSwitchToTorus32(c3, q);
    //torusTemp = tempval2;
    tlweCipher->b->coefsT[i] = torusTemp;
    //std::cout << "Here 6 in loop of b " << torusTemp << '\n';
  }
  //std::cout << "Value of the TLWE ciphertexts b " << tlweCipher->b->coefsT[0] << " " << tlweCipher->b->coefsT[1] << '\n';
  //std::cout << "Going out of seal2tfhe" << '\n';
  //std::cout << "Value of q: " << q <<  '\n';
  //free(c1);
}

////template<typename TORUS>
void tfhe2seal::ciphertext(seal::Ciphertext sealCipher, TLweSample * tlweCipher,const seal::SEALContext &sealContext)
{
  // Extract variables
  auto &context_data = *sealContext.key_context_data();
  int N = context_data.parms().poly_modulus_degree();
  uint32_t q = context_data.total_coeff_modulus()[0];

  // Temp variables
  uint32_t intTemp;

  // The equivalent of a
  //uint64_t c1_temp=sealCipher->data(1);
  //uint64_t * c1 = c1_temp & 0xFFFFFFFF;
  uint64_t * c1=sealCipher.data(1);
  // Fill from a
  for (size_t i = 0; i < N; i++)
  {
    // No minus here
    // intTemp = TorusUtils ::modSwitchFromTorus64bits((&(tlweCipher->a[0]))->coefsT[i], q);
    // Minus here
    intTemp = q-modSwitchFromTorus32((&(tlweCipher->a[0]))->coefsT[i], q);

    c1[i] = intTemp;
  }

  // Fill from b
  for (size_t i = 0; i < N/4; i++)
  {
    intTemp = modSwitchFromTorus32(tlweCipher->b->coefsT[i], q);
    // (*sealCipher)[N+i] = intTemp;
    sealCipher[i] = intTemp;
  }
  for (size_t i = N/4+1; i < N; i++)
  {
    intTemp = modSwitchFromTorus32(0, q);
    // (*sealCipher)[N+i] = intTemp;
    sealCipher[i] = intTemp;
  }
}

// Key
////template<typename TORUS>
//This will define a array of tlwe keys, the size of the array will be defined by N_seal/N_tfhe
void seal2tfhe::secretKey(TLweKey * tlweKey, seal::SecretKey sealKey, const seal::SEALContext &sealContext)
{
  // Extract variables
  auto &context_data = *sealContext.key_context_data();
  int N_seal = context_data.parms().poly_modulus_degree();
  auto coeff_modulus = context_data.parms().coeff_modulus();
  uint64_t q = coeff_modulus[0].value();
  int N_tfhe = 1024;
  Plaintext key_poly =  sealKey.data();
 // uint64_t * key_coefs = sealKey.data().data();
  uint64_t *key_coefs = key_poly.data();

  auto small_ntt_tables = context_data.small_ntt_tables();

  // Find the original key
  seal::util::inverse_ntt_negacyclic_harvey_lazy(key_coefs, small_ntt_tables[0]);
  //const uint32_t fix = N_seal - N_tfhe;
  int weird_count = 0;
  // std::cout << "Key Coefs: ";
//  cout << "Value of q is " << q << endl;
  for (size_t i = 0; i < N_tfhe; i++)
   {
        int64_t sum = 0;
        for (size_t j = 0; j < 4; j++)
        {
            size_t index = i + j * N_tfhe;
            if (key_coefs[index] == q - 1)
                sum -= 1;
            else if ((key_coefs[index] == q) || (key_coefs[index] == 0))
                sum += 0;
            else if ((key_coefs[index] == q + 1) || (key_coefs[index] == 1))
                sum += 1;
            //else
                //sum += key_coefs[index];
        }
        if ((sum != 0) || (sum != 1) || (sum != -1))
           { weird_count++;cout << "Weird value in seal key : " << sum <<  endl;}
        tlweKey->key->coefs[i] = sum;
        // int64_t temp=0;
        // if (key_coefs[i] == q - 1)
        //   temp = -1;
        // else if ((key_coefs[i] == q) || (key_coefs[i] == 0))
        //   temp = 0;
        // else if ((key_coefs[i] == q + 1) || (key_coefs[i] == 1))
        //     temp = 1;
        // else
        //  {// cout << "Weird value in seal key : " << key_coefs[i] << " , Value of q is " << q <<  endl;
        //  weird_count++;         
        //  }
        // tlweKey->key->coefs[i] = temp;
    }
  // std::cout << "weird count was " << weird_count << endl;
}

// Just generate the underlying TRLWE key
//template<typename TORUS>
void seal2tfhe::secretTGSWKey(TGswKey  * tgswKey, seal::SecretKey sealKey,const seal::SEALContext &sealContext)
{
  seal2tfhe::secretKey(&(tgswKey->tlwe_key), sealKey, sealContext);
}

