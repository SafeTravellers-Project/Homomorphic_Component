
#include <fstream>
#include <iostream>

// SEAL
#include "seal/seal.h"

// TFHE
// - Main TFHE
//#include "tfhe_gate.h"
//#include "tfhe_functions.h"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <map>

#include "tfhe.h"
#include <ctime>
#include "utils.h"
#include "dataIO.h"
#include "params.h"

using namespace std;
using namespace seal;


void paramsGen::generateSealParams()
{
  EncryptionParameters parms(scheme_type::bfv);
  const int N =4096;
  const uint64_t p = 20000;

  parms.set_poly_modulus_degree(N);  // Set the degree of the polynomial modulus (N is the degree)
  //parms.set_coeff_modulus(CoeffModulus::BFVDefault(N));       // Sets the coefficient modulus (q) for the BFV scheme, which is a large prime number used in encryption
  parms.set_coeff_modulus(CoeffModulus::Create(N, { 30, 20, 20, 30 }));
  parms.set_plain_modulus(p);  // Set the plaintext modulus (p is the modulus used for encoding plaintexts)

  SEALContext context(parms);

  
  std::ofstream bin_sealparam_handler;

  bin_sealparam_handler.open("../data/System Parameters/sealparam.txt", std::ios::out| ios::binary);
  // Check if the file has opened
    if(!bin_sealparam_handler)
    {
        cout << "Seal param file did not open!" << std::endl;
        exit(1);
    }
  auto size = parms.save(bin_sealparam_handler);
  cout << "Size of the SEAL parameters: " << size << endl;
  //bin_sealparam_handler.write((char *) &parms, sizeof(EncryptionParameters));
  bin_sealparam_handler.close();
  if(!bin_sealparam_handler.good())
   {
       cout << "Error occurred during writing the seal param binary file!" << endl;
       exit(2);
   }
  
}

void paramsGen::generateTFHEParams()
{
const int boot_l = 6;
const int boot_Bgbits =6;

// Initial parameters
const double init_N = 4096;
const double init_alpha = pow(2.,-25);//3.2e-10;//1.11e-16;

const TLweParams * init_tlwe_params = new_TLweParams(init_N, 1, init_alpha, init_alpha);
const TGswParams * init_tgsw_params = new_TGswParams(boot_l, boot_Bgbits, init_tlwe_params);
const LweParams * init_lwe_params = &(init_tlwe_params->extracted_lweparams);

// Output parameters for after bootstrapping
const double out_N = 4096;
const double out_alpha = pow(2.,-25);//3.2e-10;//1e-9;//3.2e-10;//1.11e-16;

const TLweParams * out_tlwe_params = new_TLweParams(out_N, 1, out_alpha, out_alpha);
const TGswParams * out_tgsw_params = new_TGswParams(boot_l, boot_Bgbits, out_tlwe_params);
const LweParams * out_lwe_params = &(out_tlwe_params->extracted_lweparams);



  FILE * tgsw_iparams_file = fopen("../data/System Parameters/params_tgsw_in.txt" , "w");
  if (tgsw_iparams_file == NULL) {
      std::cerr << "Error opening file ../data/System Parameters/params_tgsw_in.txt" << std::endl;
        exit(1);}
  export_tGswParams_toFile(tgsw_iparams_file,init_tgsw_params);
  fclose(tgsw_iparams_file);


  FILE * tlwe_iparams_file = fopen("../data/System Parameters/params_tlwe_in.txt" , "w");
  if (tlwe_iparams_file == NULL) {
      std::cerr << "Error opening file ../data/System Parameters/params_tlwe_in.txt" << std::endl;
        exit(1);}
  export_tLweParams_toFile(tlwe_iparams_file,init_tlwe_params);
  fclose(tlwe_iparams_file);
  
  FILE * lwe_iparams_file = fopen("../data/System Parameters/params_lwe_in.txt" , "w");
  if (lwe_iparams_file == NULL) {
      std::cerr << "Error opening file params_lwe_in.txt" << std::endl;
      exit(1);
  }
  export_lweParams_toFile(lwe_iparams_file,init_lwe_params);
  fclose(lwe_iparams_file);

  FILE * tgsw_oparams_file = fopen("../data/System Parameters/params_tgsw_out.txt" , "w");
  if (tgsw_oparams_file == NULL) {
      std::cerr << "Error opening file ../data/System Parameters/params_tgsw_out.txt" << std::endl;
        exit(1);}
  export_tGswParams_toFile(tgsw_oparams_file,out_tgsw_params);
  fclose(tgsw_oparams_file);


  FILE * tlwe_oparams_file = fopen("../data/System Parameters/params_tlwe_out.txt" , "w");
  if (tlwe_oparams_file == NULL) {
      std::cerr << "Error opening file ../data/System Parameters/params_tlwe_out.txt" << std::endl;
        exit(1);}
  export_tLweParams_toFile(tlwe_oparams_file,out_tlwe_params);
  fclose(tlwe_oparams_file);



  FILE * lwe_oparams_file = fopen("../data/System Parameters/params_lwe_out.txt" , "w");
  if (lwe_oparams_file == NULL) {
      std::cerr << "Error opening file ../data/System Parameters/params_lwe_out.txt" << std::endl;
      exit(1);
  }
  export_lweParams_toFile(lwe_oparams_file,out_lwe_params); // this is the lwe_params_lvl1
  fclose(lwe_oparams_file);
   

  //FILE * tgsw_iparams_file = fopen("../data/System Parameters/params_tgsw_in.txt" , "w");
  //export_tGswParams_toFile(tgsw_iparams_file, init_tgsw_params);
  //fclose(tgsw_iparams_file);
  //FILE * tlwe_oparams_file = fopen("../data/System Parameters/params_tlwe_out.txt" , "w");
  //export_tLweParams_toFile(tlwe_oparams_file,out_tlwe_params);

}


 
 