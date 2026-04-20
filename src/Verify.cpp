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
#include "constants.h"


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
FILE * lwe_bparams_file = fopen("../data/System_Parameters/params_lwe_b.txt" , "r");
LweParams * b_lwe_params = new_lweParams_fromFile(lwe_bparams_file);
fclose(lwe_bparams_file);
FILE * tlwe_bparams_file = fopen("../data/System_Parameters/params_tlwe_b.txt" , "r");
TLweParams * b_tlwe_params = new_tLweParams_fromFile(tlwe_bparams_file);
fclose(tlwe_bparams_file);

/**
  * Set the initial parameterss
  */
const int N_seal = parms.poly_modulus_degree();
const uint64_t p = parms.plain_modulus().value();  // This need s to be changed as per requirement
const int precision = Constants::PRECISION;

const int vector_size = Constants::VECTOR_SIZE;
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



/* Copy the keys that are going to be used again and again*/
RelinKeys relin_key;
filebuf bin_sealrelinK_handler;
bin_sealrelinK_handler.open("../data/Traveller/sealrelinK.txt", ios::in | ios::binary);
istream is_relin(&bin_sealrelinK_handler);
relin_key.load(context,is_relin);
bin_sealrelinK_handler.close();

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
FILE * out1_lwekey_file = fopen("../data/HEComp/lwe_out1.txt", "r"); 
LweKey * out1_lwe_key = new_lweKey_fromFile(out1_lwekey_file);
fclose(out1_lwekey_file);

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

/* */
 
std::string folderName_model_new = std::string(argv[2]);
std::string folderName_model_old = std::string(argv[3]);

// Check if input folder exists
  if (!fs::exists(folderName_model_new)) {
    std::cerr << "Error: Input folder does not exist: " << folderName_model_new << std::endl;
    return 1;
  }
  
  // Read all files from the input folder
  // std::vector<std::string> biometric_files;
  // for (const auto & entry : fs::directory_iterator(folderName_model_new)) {
  //   if (fs::is_regular_file(entry)) {
  //     biometric_files.push_back(entry.path().filename().string());
  //   }
  // }
  
  // if (biometric_files.empty()) {
  //   std::cerr << "Error: No files found in input folder: " << folderName_model_new << std::endl;
  //   return 1;
  // }

 //for (const auto & biofile : biometric_files) {
    //string fileName_model_new = folderName_model_new + "/" + biofile;
    dataIO::Read_F(folderName_model_new, context, testSEALCipher, testSEALCipherSquare, testSEALCosineCipher);
    //string fileName_model_old = folderName_model_old + "/" + biofile;
    dataIO::Read_F(folderName_model_old, context, modelSEALCipher, modelSEALCipherSquare, modelSEALCosineCipher);


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


clock_t bin_start1=clock();

/* Computing Euclidean distance */
cout << "Computing Euclidean distance..." << endl;
evaluator.multiply(modelSEALCipher,testSEALCipher,distanceVV);
if (parms.poly_modulus_degree() > 2047) 
{
evaluator.relinearize_inplace(distanceVV, relin_key);
distanceVV4= distanceVV;
}

evaluator.add_inplace(distanceVV,distanceVV);
evaluator.negate_inplace(distanceVV);
distanceVV1 = distanceVV;  // this is -2ab
evaluator.add_inplace(distanceVV, testSEALCipherSquare); 
distanceVV2= testSEALCipherSquare; // this is b^2
evaluator.add_inplace(distanceVV, modelSEALCipherSquare);
distanceVV3= modelSEALCipherSquare; //this is a^2
distance_diff=distanceVV; // this is a^2 - 2ab + b^2 
threshold_dist= distance_diff;
evaluator.negate_inplace(threshold_dist);
evaluator.add_plain_inplace(threshold_dist,Threshold_value); //this is threshold - (a^2 - 2ab + b^2)
clock_t bin_end1=clock();

clock_t cos_start2 = clock();
cout << "Computing cosine distance..." << endl;
/* Computing the cosine */
Ciphertext distanceCos;
evaluator.multiply(modelSEALCosineCipher,testSEALCosineCipher,distanceCos);
evaluator.relinearize_inplace(distanceCos, relin_key);
clock_t cos_end2 = clock();

/***********************ONly part of additional test  */
// cout << "Testing the encryption and decryption of a sample value using SEAL" << endl;
// int32_t test_value = 1234;          // any integer within your plain modulus
// Plaintext pt;
// dataIO::makePlaintext(&pt, test_value, N_seal, p);   // same helper you already use

// // encrypt it with SEAL
// Ciphertext ct;
// init_encryptor.encrypt(pt, ct);
/***************** end of addtitional test part********** */


auto context_data = context.first_context_data();
while (context_data->next_context_data())
    {
    //    evaluator.mod_switch_to_next_inplace(distanceVV1);
    //    evaluator.mod_switch_to_next_inplace(distanceVV2);
    //      evaluator.mod_switch_to_next_inplace(distanceVV3);
    //     evaluator.mod_switch_to_next_inplace(distance_diff);
        evaluator.mod_switch_to_next_inplace(threshold_dist);
        evaluator.mod_switch_to_next_inplace(distanceCos);
//        evaluator.mod_switch_to_next_inplace(ct);
        context_data = context_data->next_context_data();
    }

TLweSample * tlwe_act = new_TLweSample(b_tlwe_params);
LweSample * lwe_act = new_LweSample(b_lwe_params);

/* Now converting the SEAL ciphertexts into TFHE ciphertexts */
clock_t bin_start2=clock();
seal2tfhe::ciphertext_seal_exit(tlwe_act, threshold_dist, context);
tLweExtractLweSampleIndex(lwe_act, tlwe_act, 4095, b_lwe_params, b_tlwe_params);
clock_t bin_end2=clock();

// /******************************  Only test *****************************/
// TLweSample * tlwe_t = new_TLweSample(b_tlwe_params);
// LweSample * lwe_t = new_LweSample(b_lwe_params);
// seal2tfhe::ciphertext_seal_exit(tlwe_t, distance_diff, context);
// tLweExtractLweSampleIndex(lwe_t, tlwe_t, 4095, b_lwe_params, b_tlwe_params);
// // cout << "Here 1" << endl;
// TLweSample * tlwe_t1 = new_TLweSample(b_tlwe_params);
// LweSample * lwe_t1 = new_LweSample(b_lwe_params);
// seal2tfhe::ciphertext_seal_exit(tlwe_t1, distanceVV1, context);
// tLweExtractLweSampleIndex(lwe_t1, tlwe_t1, 4095, b_lwe_params, b_tlwe_params);
// cout << "Here 2" << endl;
// TLweSample * tlwe_t2 = new_TLweSample(b_tlwe_params);
// LweSample * lwe_t2 = new_LweSample(b_lwe_params);
// seal2tfhe::ciphertext_seal_exit(tlwe_t2, distanceVV2, context);
// tLweExtractLweSampleIndex(lwe_t2, tlwe_t2, 4095, b_lwe_params, b_tlwe_params);
// cout << "Here 3" << endl;
// TLweSample * tlwe_t3 = new_TLweSample(b_tlwe_params);
// LweSample * lwe_t3 = new_LweSample(b_lwe_params);
// seal2tfhe::ciphertext_seal_exit(tlwe_t3, distanceVV3, context);
// tLweExtractLweSampleIndex(lwe_t3, tlwe_t3, 4095, b_lwe_params, b_tlwe_params);
// // cout << "Here 4" << endl;
// delete_TLweSample(tlwe_t);
// delete_TLweSample(tlwe_t1);
// delete_TLweSample(tlwe_t2);
// delete_TLweSample(tlwe_t3);


//  cout << "Here 292" << endl;
// // FILE * b_lweKey_file = fopen("../data/Traveller/lwe_b.txt" , "r");
// // LweKey * b_lwe_key = new_lweKey_fromFile(b_lweKey_file);
// // fclose(b_lweKey_file);
//  FILE * init_lweKey_file = fopen("../data/Traveller/lwe_in.txt" , "r");
// LweKey * init_lwe_key = new_lweKey_fromFile(init_lweKey_file);
// fclose(init_lweKey_file);
// // // cout << "Here 296" << endl;
// static long int t_pow10[12] =
//     {
//         1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000
//     };
// // // for (int i = 1; i < 5; i++)
// // // {
// // //   cout << "precision : " << i << " : " <<  t_pow10[i-1] << " || " <<  t_pow10[2*(i-1)] << '\n';
// // // }    
// Torus32 t_torusTemp,t_torusTemp_act, t_torusTemp1,t_torusTemp2,t_torusTemp3;
// int t_intTemp,t_intTemp_act,t_intTemp1,t_intTemp2,t_intTemp3;
// double t_doubleTemp,t_doubleTemp_act,t_doubleTemp1,t_doubleTemp2,t_doubleTemp3;
// LweSample * lwe_t_ks = new_LweSample(init_lwe_params);
// lweKeySwitch(lwe_t_ks, ks_med_key, lwe_t);
// LweSample * lwe_act_ks = new_LweSample(init_lwe_params);
// lweKeySwitch(lwe_act_ks, ks_med_key, lwe_act);
// // Decrypt the keyswitched distance_diff sample (not threshold_dist).
// t_torusTemp_act = lweSymDecrypt(lwe_act_ks, init_lwe_key, p);
// t_intTemp_act = modSwitchFromTorus32(t_torusTemp_act, p);
// t_doubleTemp_act = ((double) t_intTemp_act - p*(t_intTemp_act >= p/2))/t_pow10[2*(precision-1)];
// t_torusTemp = lweSymDecrypt(lwe_t_ks, init_lwe_key, p);
// t_intTemp = modSwitchFromTorus32(t_torusTemp, p);
// t_doubleTemp = ((double) t_intTemp - p*(t_intTemp >= p/2))/t_pow10[2*(precision-1)];
// LweSample * lwe_t_ks1 = new_LweSample(init_lwe_params);
// lweKeySwitch(lwe_t_ks1, ks_med_key, lwe_t1);
// t_torusTemp1 = lweSymDecrypt(lwe_t_ks1, init_lwe_key, p);
// t_intTemp1 = modSwitchFromTorus32(t_torusTemp1, p);
// t_doubleTemp1 = ((double) t_intTemp1 - p*(t_intTemp1 >= p/2))/t_pow10[2*(precision-1)];
// LweSample * lwe_t_ks2 = new_LweSample(init_lwe_params);
// lweKeySwitch(lwe_t_ks2, ks_med_key, lwe_t2);
// t_torusTemp2 = lweSymDecrypt(lwe_t_ks2, init_lwe_key, p);
// t_intTemp2 = modSwitchFromTorus32(t_torusTemp2, p);
// t_doubleTemp2 = ((double) t_intTemp2 - p*(t_intTemp2 >= p/2))/t_pow10[2*(precision-1)];
// LweSample * lwe_t_ks3 = new_LweSample(init_lwe_params);
// lweKeySwitch(lwe_t_ks3, ks_med_key, lwe_t3);
// t_torusTemp3 = lweSymDecrypt(lwe_t_ks3, init_lwe_key, p);
// t_intTemp3 = modSwitchFromTorus32(t_torusTemp3, p);
// t_doubleTemp3 = ((double) t_intTemp3 - p*(t_intTemp3 >= p/2))/t_pow10[2*(precision-1)];
// std::cout << "The distance threshold diff was  " << t_intTemp_act <<"  :: :: " << t_doubleTemp_act <<  '\n';
// std::cout << "The distance difference was  " << t_intTemp <<"  :: :: " << t_doubleTemp <<  '\n';
// std::cout << "2ab was  " << t_intTemp1 <<"  :: :: " << t_doubleTemp1 <<  '\n';
// std::cout << "b^2 was  " << t_intTemp2 <<"  :: :: " << t_doubleTemp2 <<  '\n';
// std::cout << "a^2 was  " << t_intTemp3 <<"  :: :: " << t_doubleTemp3 <<  '\n';
// cout << "Value of plaintext modulus p is: " << p << endl;
// delete_LweSample(lwe_t);
// delete_LweSample(lwe_t1);
// delete_LweSample(lwe_t2);
// delete_LweSample(lwe_t3);
// delete_LweSample(lwe_t_ks);
// delete_LweSample(lwe_t_ks1);
// delete_LweSample(lwe_t_ks2);
// delete_LweSample(lwe_t_ks3);
// /**************************************************************** */




/*Additional tests*************************/
// COMMENTED OUT - This seal2tfhe::ciphertext_seal_exit call also causes heap corruption

// SecretKey secret_key;
// filebuf bin_sealsecK_handler;
// bin_sealsecK_handler.open("../data/Traveller/sealpvtK.txt", ios::in | ios::binary);
// istream is_sec(&bin_sealsecK_handler);
// secret_key.load(context, is_sec);
// bin_sealsecK_handler.close();

// // Decrypt
// Decryptor decryptor(context, secret_key);
// Plaintext pt_decrypted;
// decryptor.decrypt(ct, pt_decrypted);

// uint64_t *decoded = pt_decrypted.data();
// int32_t recovered2 = decoded[4095];

// std::cout << "original=" << test_value << "  recovered=" << recovered2 << std::endl;

// // convert SEAL ciphertext -> TFHE TLweSample
// TLweSample * tlwe_test = new_TLweSample(b_tlwe_params);
//  seal2tfhe::ciphertext_seal_exit(tlwe_test, ct, context);
// //seal2tfhe::ciphertext_new(tlwe_test, ct, context);
// // extract an LweSample (you already use index 4095 elsewhere)
// cout << "Extracting an LweSample from the TLweSample and decrypting it with TFHE" << endl;
// LweSample * lwe_test = new_LweSample(b_lwe_params);
// tLweExtractLweSampleIndex(lwe_test, tlwe_test, 4095,
//                           b_lwe_params, b_tlwe_params);
// LweSample * lwe_test_ks = new_LweSample(init_lwe_params);
// lweKeySwitch(lwe_test_ks, ks_med_key, lwe_test);
// // now decrypt with the same TFHE key you use later (out2_lwe_key in your code)
// Torus32 torus =
//     lweSymDecrypt(lwe_test_ks, init_lwe_key, p);
// int32_t recovered = modSwitchFromTorus32(torus, p);
// Torus32 torus2 =
//     lweSymDecrypt(lwe_test, b_lwe_key, p);
// int32_t recovered3 = modSwitchFromTorus32(torus2, p);
// std::cout << "original=" << test_value
//           << "  recovered2=" << recovered3 << std::endl;

// // clean up
//delete_LweKey(init_lwe_key);
//  delete_TLweSample(tlwe_test);
//  delete_LweSample(lwe_test);
//  delete_LweSample(lwe_test_ks);   
/*    *****************************************/

 LweSample * lweDeltaM = new_LweSample(out_lwe_params); // Defining the variable that will store the value of the sign bootstrapping.
 LweSample * lweRotation = new_LweSample(out_lwe_params); // Defining the variable that will store the value of the rotation insude bootstrapping.


int torusDivision = 4;
Torus32 positive = modSwitchToTorus32(1, torusDivision);
Torus32 negative = modSwitchToTorus32(torusDivision - 1, torusDivision);

auto decodeSignFromRaw = [torusDivision](int raw) {
  raw %= torusDivision;
  if (raw < 0) raw += torusDivision;
  auto ringDist = [torusDivision](int a, int b) {
    int d = std::abs(a - b);
    return std::min(d, torusDivision - d);
  };
  int distPos = ringDist(raw, 1);
  int distNeg = ringDist(raw, torusDivision - 1);
  return (distPos <= distNeg) ? 1 : -1;
};
//Torus32 positive_2 = modSwitchToTorus32(1, 2*torusDivision);
//Torus32 negative_2 = modSwitchToTorus32(-1, 2*torusDivision);
//lweNoiselessTrivial(lweRotation, positive, out_lwe_params); // Necessary for the correcting the post-bootstrapping result. 

clock_t bin_start3=clock();

/******* Testing Bootstrapping */
/*
LweSample * lwe_boot_test = new_LweSample(init_lwe_params);
LweSample * lweDeltaM_test = new_LweSample(out_lwe_params);
Torus32 testTorus,torusTemp1_test;
int sign1_test;
const int signTestDivision = 32;
for (int k = -12; k <= 12; k++)
{
  if (k == 0) continue;
  int msg = (k >= 0) ? k : (signTestDivision + k);
  testTorus = modSwitchToTorus32(msg, signTestDivision);
    lweSymEncrypt(lwe_boot_test, testTorus, init_lwe_params->alpha_min, init_lwe_key);
    tfhe_bootstrap_woKS_FFT(lweDeltaM_test, boot_key_fft, positive, lwe_boot_test);
    torusTemp1_test = lweSymDecrypt(lweDeltaM_test, out1_lwe_key, torusDivision);
  sign1_test = decodeSignFromRaw(modSwitchFromTorus32(torusTemp1_test, torusDivision));
  int expectedSign = (k > 0) ? 1 : -1;
  cout << "Sign bootstrap test phase " << k << "/" << signTestDivision
     << " -> expected " << expectedSign << ", got " << sign1_test << endl;
}

delete_LweSample(lwe_boot_test);
delete_LweSample(lweDeltaM_test);
//*/
/******************************************************** */
//Compute the bootstrapping of the Threshold distance -  Distance
LweSample * lwe_act1 = new_LweSample(init_lwe_params);
lweKeySwitch(lwe_act1, ks_med_key, lwe_act);
// Force mapping: lower half -> -1/torusDivision, upper half -> +1/torusDivision.
tfhe_bootstrap_woKS_FFT(lweDeltaM, boot_key_fft, positive, lwe_act1);
clock_t bin_end3=clock();
double bin_duration3 = double(bin_end3 - bin_start3) / CLOCKS_PER_SEC;
//LweSample * lweresult = new_LweSample(out_lwe_params);

//LweSample * lweDelta_boot = new_LweSample(out_lwe_params);
//lweKeySwitch(lweDelta_boot,ks_inout_key,lweDeltaM);
//Torus32 torusTemp1 = lweSymDecrypt(lweDeltaM, out1_lwe_key, torusDivision);
//int sign1 = decodeSignFromRaw(modSwitchFromTorus32(torusTemp1, torusDivision));


LweSample * lweDelta_boot_final = new_LweSample(out_lwe_params);
lweKeySwitch(lweDelta_boot_final,ks_key,lweDeltaM);
// Torus32 phase = lwePhase(lweDelta_boot_final, out2_lwe_key);

// if (phase > 0)
//     printf("positive sign. Accept\n");
// else
//     printf("negative sign, reject\n");
Torus32 torusTemp = lweSymDecrypt(lweDelta_boot_final, out2_lwe_key, torusDivision);
int sign = decodeSignFromRaw(modSwitchFromTorus32(torusTemp, torusDivision));
cout << "The sign of the distance difference is :  " << sign << endl;
string flag;
if (sign > 0)
    //cout << "The distance is less than the threshold, same person (Accept). " << endl;
    flag = "Accept";
else
    flag = "Reject";//cout << "The distance is greater than the threshold, further investigation needed. " << endl;

double bin_duration = double(bin_end3 - bin_start3 + bin_end2 - bin_start2 + bin_end1 - bin_start1) / CLOCKS_PER_SEC;


//Compute the identity bootstrapping of the Cosine distance
TLweSample * tlwe_act_cos = new_TLweSample(b_tlwe_params);
LweSample * lwe_act_cos = new_LweSample(b_lwe_params);
LweSample * lwe_act_cos_ks = new_LweSample(init_lwe_params); // needed for the artifical key switch 



clock_t cos_start1 = clock();
seal2tfhe::ciphertext_seal_exit(tlwe_act_cos, distanceCos, context);
tLweExtractLweSampleIndex(lwe_act_cos, tlwe_act_cos, 4095, b_lwe_params, b_tlwe_params);
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
//cout << " Result given as - > Accept/Reject decision || Cosine Similarity Value " << endl;
std::cout << folderName_model_new <<  "||" << flag << " || " << doubleTemp <<  '\n';
cout << "***************************************************************************" << endl;
cout << "Total time for accept/reject check : " << bin_duration << " seconds" << endl;

std::cout << "Cosine computtion duration: " << cos_duration << " seconds" << std::endl;
 //}
// delete_LweSample(lwe_act);
// delete_LweSample(lweRotation);
// delete_LweSample(lweDeltaM);
// delete_LweSample(lweresult);
// delete_LweSample(lweDelta_boot_final);
//delete_LweSample(lweDelta_boot);
// delete_LweSample(lwe_act1);
// delete_LweSample(lwe_t_ks);
// delete_LweSample(lwe_t_ks1);
//delete_LweKey(init_lwe_key);    

    return 0;
}
