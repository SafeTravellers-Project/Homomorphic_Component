#include <cstdio>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <filesystem>

// SEAL
#include "seal/seal.h"

// TFHE
#include "tfhe.h"

// TFHE OPS
#include "scaled_tfhe.h"
#include "more_tfhe.h"

#include "utils.h"
#include "dataIO.h"
#include "token.h"
#include "keysEGate.h"


using namespace std;
using namespace seal;
namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <threshold_value>"  << " <Test Biometric folder> " << " <Stored Biometric folder> " << std::endl;
    return 1;
  }

  int32_t threshold_val = std::stoi(argv[1]);  // Needs to be chaged as per requirements  // For example 2000

  srand(time(0));


  // Read the SEAL parameters from the file

std::ifstream bin_sealparam_handler("../data/System_Parameters/sealparam.txt", std::ios::in | std::ios::binary);
    if (!bin_sealparam_handler) {
        std::cerr << "Seal param file did not open!" << std::endl;
        return 1;
    }

   // Read the parameters from the file
    EncryptionParameters parms(scheme_type::bfv);
    parms.load(bin_sealparam_handler);
    bin_sealparam_handler.close();

    if (!bin_sealparam_handler.good()) {
        std::cerr << "Error occurred during reading the seal param binary file!" << std::endl;
        return 2;
    }
SEALContext context(parms);

PublicKey init_public_key;
filebuf bin_sealpubK_handler;
bin_sealpubK_handler.open("../data/Traveller/sealpubK.txt", ios::in | ios::binary); // this needs to be changed for every traveller. For initial testing we are using a predefined key.
istream is(&bin_sealpubK_handler);
init_public_key.load(context,is);
bin_sealpubK_handler.close();

Encryptor init_encryptor(context, init_public_key);
Evaluator evaluator(context);


/*   Get in all the TFHE params and Keys*/

FILE * lwe_oparams_file = fopen("../data/System_Parameters/params_lwe_out.txt" , "r");
LweParams * out_lwe_params = new_lweParams_fromFile(lwe_oparams_file);
fclose(lwe_oparams_file);
FILE * tlwe_iparams_file = fopen("../data/System_Parameters/params_tlwe_in.txt" , "r");
TLweParams * init_tlwe_params = new_tLweParams_fromFile(tlwe_iparams_file);
fclose(tlwe_iparams_file);
FILE * tlwe_oparams_file = fopen("../data/System_Parameters/params_tlwe_out.txt" , "r");
TLweParams * out_tlwe_params = new_tLweParams_fromFile(tlwe_oparams_file);
fclose(tlwe_oparams_file);
FILE * lwe_iparams_file = fopen("../data/System_Parameters/params_lwe_in.txt" , "r");
LweParams * init_lwe_params = new_lweParams_fromFile(lwe_iparams_file);
fclose(lwe_iparams_file);

/**
  * Set the initial parameterss
  */
const int N_seal = parms.poly_modulus_degree();
const uint64_t p = parms.plain_modulus().value();  // This need s to be changed as per requirement
const int precision = 3;

const int vector_size = 1024;
if (vector_size > N_seal)
{
std::cout << "The size of Poly Modulus Degree has to be greater than that of the Biometrics vectors" << '\n';
  return -1;
}

/* Now going to read the ciphertexts of the Test Biometrics and then the Stored Biometrics  */

Ciphertext testSEALCipher;
Ciphertext testSEALCipherSquare;
Ciphertext testSEALCosineCipher;
Ciphertext modelSEALCipher; 
Ciphertext modelSEALCipherSquare; 
Ciphertext modelSEALCosineCipher;
 
std::string folderName_model_new = std::string(argv[1]);
std::string folderName_model_old = std::string(argv[2]);

// Check if input folder exists
  if (!fs::exists(folderName_model_new)) {
    std::cerr << "Error: Input folder does not exist: " << folderName_model_new << std::endl;
    return 1;
  }
  
  // Read all files from the input folder
  std::vector<std::string> biometric_files;
  for (const auto & entry : fs::directory_iterator(folderName_model_new)) {
    if (fs::is_regular_file(entry)) {
      biometric_files.push_back(entry.path().filename().string());
    }
  }
  
  if (biometric_files.empty()) {
    std::cerr << "Error: No files found in input folder: " << folderName_model_new << std::endl;
    return 1;
  }


 for (const auto & fileName_model_new : biometric_files) {
    dataIO::Read_F(fileName_model_new, context, testSEALCipher, testSEALCipherSquare, testSEALCosineCipher);
    string fileName_model_old = folderName_model_old + "/" + fileName_model_new;
    dataIO::Read_F(fileName_model_old, context, modelSEALCipher, modelSEALCipherSquare, modelSEALCosineCipher);


/******************************************************* */

Ciphertext distanceVV;
Ciphertext distanceVV1;
Ciphertext distanceVV2;
Ciphertext distanceVV3;
Ciphertext distanceVV4;

Ciphertext distance_diff;
Ciphertext threshold_dist;

Plaintext Threshold_value;


dataIO::makePlaintext(&Threshold_value, threshold_val, N_seal, p);
cout << "Threshold value taken in, set to  " << threshold_val << endl;

RelinKeys relin_key;
clock_t bin_start1=clock();

/* Computing Euclidean distance */

evaluator.multiply(modelSEALCipher,testSEALCipher,distanceVV);
if (parms.poly_modulus_degree() > 2047) 
{
filebuf bin_sealrelinK_handler;
bin_sealrelinK_handler.open("../data/Traveller/sealrelinK.txt", ios::in | ios::binary);
istream is_relin(&bin_sealrelinK_handler);
relin_key.load(context,is_relin);
bin_sealrelinK_handler.close();
evaluator.relinearize_inplace(distanceVV, relin_key);
distanceVV4= distanceVV;
}

evaluator.add_inplace(distanceVV,distanceVV);
evaluator.negate_inplace(distanceVV);
distanceVV1 = distanceVV;
evaluator.add_inplace(distanceVV, testSEALCipherSquare); 
distanceVV2= testSEALCipherSquare;
evaluator.add_inplace(distanceVV, modelSEALCipherSquare);
distanceVV3= modelSEALCipherSquare;
distance_diff=distanceVV;
threshold_dist= distance_diff;
evaluator.negate_inplace(threshold_dist);
evaluator.add_plain_inplace(threshold_dist,Threshold_value);
clock_t bin_end1=clock();

clock_t cos_start2 = clock();

/* Computing the cosine */
Ciphertext distanceCos;
evaluator.multiply(modelSEALCosineCipher,testSEALCosineCipher,distanceCos);
evaluator.relinearize_inplace(distanceCos, relin_key);
clock_t cos_end2 = clock();

auto context_data = context.first_context_data();
while (context_data->next_context_data())
    {
        evaluator.mod_switch_to_next_inplace(distance_diff);
        evaluator.mod_switch_to_next_inplace(threshold_dist);
        evaluator.mod_switch_to_next_inplace(distanceCos);
        context_data = context_data->next_context_data();
    }

TLweSample * tlwe_act = new_TLweSample(init_tlwe_params);
LweSample * lwe_act = new_LweSample(init_lwe_params);

/* Now converting the SEAL ciphertexts into TFHE ciphertexts */
clock_t bin_start2=clock();
seal2tfhe::ciphertext_seal_exit(tlwe_act, threshold_dist, context);
tLweExtractLweSampleIndex(lwe_act, tlwe_act, 4095, init_lwe_params, init_tlwe_params);
clock_t bin_end2=clock();

// /******************************  Only test *****************************/
// FILE * init_lweKey_file = fopen("../data/Traveller/lwe_in.txt" , "r");
// LweKey * init_lwe_key = new_lweKey_fromFile(init_lweKey_file);
// fclose(init_lweKey_file);
// static long int pow10[12] =
//     {
//         1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000
//     };
// Torus32 torusTemp;
// int intTemp;
// double doubleTemp;
// torusTemp = lweSymDecrypt(lwe_act, init_lwe_key, p);
// intTemp = modSwitchFromTorus32(torusTemp, p);
// doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
// std::cout << "The threshold-distance difference was  " << intTemp <<"  :: :: " << doubleTemp <<  '\n';
// /**************************************************************** */

/* ********************Reading BootKey and KS keys *********************************************************** */

FILE * boot_key_file = fopen("../data/Traveller/bootK.data" , "r"); 
LweBootstrappingKey * boot_key = new_lweBootstrappingKey_fromFile(boot_key_file);
LweBootstrappingKeyFFT * boot_key_fft = new_LweBootstrappingKeyFFT(boot_key);
fclose(boot_key_file);
//Key 1 -> Middle Key
FILE * ks_med_key_file = fopen("../data/Traveller/KSKmed.data" , "r");
LweKeySwitchKey * ks_med_key = new_lweKeySwitchKey_fromFile(ks_med_key_file);
fclose(ks_med_key_file);
// Middle Key -> HE Key
FILE * ks_inout_key_file = fopen("../data/Traveller/KSKinout.data" , "r");
LweKeySwitchKey * ks_inout_key = new_lweKeySwitchKey_fromFile(ks_inout_key_file);
fclose(ks_inout_key_file);
// HE Key -> E-Gate Key
 FILE * ks_key_file = fopen("../data/E-Gate/KSK.data", "r");
 LweKeySwitchKey * ks_key = new_lweKeySwitchKey_fromFile(ks_key_file);
 fclose(ks_key_file);

/* Check if the lwekey exists in the E-Gate folder, if yes, copy it otherwise generate new keys  */ 
  FILE * out2_lwekey_file = fopen("../data/E-Gate/lwe_out2.txt", "r");
  // if (out2_lwekey_file == NULL) {
  //   cout << "E-Gate not registered previously not found. Generating E-Gate keys..." << endl;
  //   EGateKeyGen::generateEGatekeys();
  //   FILE * out2_lwekey_file = fopen("../data/E-Gate/lwe_out2.txt", "r");
  //   LweKey * out2_lwe_key = new_lweKey_fromFile(out2_lwekey_file);
  //   fclose(out2_lwekey_file);
  //   } else {
    LweKey * out2_lwe_key = new_lweKey_fromFile(out2_lwekey_file);
    fclose(out2_lwekey_file);
 //    }

 LweSample * lweDeltaM = new_LweSample(out_lwe_params); // Defining the variable that will store the value of the sign bootstrapping.
 LweSample * lweRotation = new_LweSample(out_lwe_params); // Defining the variable that will store the value of the rotation insude bootstrapping.


int torusDivision = 16;
Torus32 positive = modSwitchToTorus32(1, torusDivision);
Torus32 negative = modSwitchToTorus32(-1, torusDivision);
Torus32 positive_2 = modSwitchToTorus32(1, 2*torusDivision);
Torus32 negative_2 = modSwitchToTorus32(-1, 2*torusDivision);
lweNoiselessTrivial(lweRotation, positive, out_lwe_params); // Necessary for the correcting the post-bootstrapping result. 

clock_t bin_start3=clock();

//Compute the bootstrapping of the Threshold distance -  Distance
LweSample * lwe_act1 = new_LweSample(init_lwe_params);
lweKeySwitch(lwe_act1, ks_med_key, lwe_act);
tfhe_bootstrap_woKS_FFT(lweDeltaM, boot_key_fft, positive, lwe_act1);
clock_t bin_end3=clock();
double bin_duration3 = double(bin_end3 - bin_start3) / CLOCKS_PER_SEC;
LweSample * lweresult = new_LweSample(out_lwe_params);

LweSample * lweDelta_boot = new_LweSample(out_lwe_params);
lweKeySwitch(lweDelta_boot,ks_inout_key,lweDeltaM);
LweSample * lweDelta_boot_final = new_LweSample(out_lwe_params);
lweKeySwitch(lweDelta_boot_final,ks_key,lweDelta_boot);
Torus32 torusTemp = lweSymDecrypt(lweDelta_boot_final, out2_lwe_key, torusDivision);
int sign = modSwitchFromTorus32(torusTemp, torusDivision);
string flag;
if (sign == 1)
    //cout << "The distance is less than the threshold, same person (Accept). " << endl;
    flag = "Accept";
else
    flag = "Reject";//cout << "The distance is greater than the threshold, further investigation needed. " << endl;

double bin_duration = double(bin_end3 - bin_start3 + bin_end2 - bin_start2 + bin_end1 - bin_start1) / CLOCKS_PER_SEC;


//Compute the identity bootstrapping of the Cosine distance
TLweSample * tlwe_act_cos = new_TLweSample(init_tlwe_params);
LweSample * lwe_act_cos = new_LweSample(init_lwe_params);
LweSample * lwe_act_cos_ks = new_LweSample(init_lwe_params); // needed for the artifical key switch 



clock_t cos_start1 = clock();
seal2tfhe::ciphertext_seal_exit(tlwe_act_cos, distanceCos, context);
tLweExtractLweSampleIndex(lwe_act_cos, tlwe_act_cos, 4095, init_lwe_params, init_tlwe_params);
lweKeySwitch(lwe_act_cos_ks, ks_med_key, lwe_act_cos);
LweSample * lweCos_boot = new_LweSample(out_lwe_params);
lweKeySwitch(lweCos_boot,ks_inout_key,lwe_act_cos_ks);
LweSample * lweCos_boot_final = new_LweSample(out_lwe_params);
lweKeySwitch(lweCos_boot_final,ks_key,lweCos_boot);
clock_t cos_end1 = clock();

double cos_duration = double(cos_end1 - cos_start1 + cos_end2 - cos_start2) / CLOCKS_PER_SEC;


static long int pow10[12] =
    {
        1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000
    };
torusTemp = lweSymDecrypt(lweCos_boot_final, out2_lwe_key, p);
int intTemp = modSwitchFromTorus32(torusTemp, p);
double doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];

cout << " **************************************************************" << endl;
cout << " Result given as - > Accept/Reject decision || Cosine Similarity Value " << endl;
std::cout << flag << " || " << doubleTemp <<  '\n';
cout << "***************************************************************************" << endl;
cout << "Total time for accept/reject check : " << bin_duration << " seconds" << endl;

std::cout << "Cosine computtion duration: " << cos_duration << " seconds" << std::endl;



    return 0;
}
