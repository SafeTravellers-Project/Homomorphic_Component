#include <cstdio>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>

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

int main(int argc, char *argv[])
{
  // if (argc < 3) {
  //   std::cerr << "Usage: " << argv[0] << " <threshold_value>"  << "<File number 1 or 2<" << std::endl;
  //   return 1;
  // }

  int32_t threshold_val = 2000;//std::stoi(argv[1]);  // Needs to be chaged as per requirements

  srand(time(0));
/*Need to take in the Test biometrics directly from the E-gate, assuming here that it gets stored somewhere in ../data/E-Gate */
    // if (argc < 2) {
    //     std::cerr << "Usage: " << argv[0] << " " << std::endl;
    //     return 1;
    // }


   // std::ifstream test_values_file(test_values_path, std::ios::binary);
   // if (!test_values_file) {
   //     std::cerr << "Error opening file: " << test_values_path << std::endl;
   //     return 1;
   // }

  

  // Read the SEAL parameters from the file

std::ifstream bin_sealparam_handler("../data/System Parameters/sealparam.txt", std::ios::in | std::ios::binary);
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
  bin_sealpubK_handler.open("../data/Traveller/sealpubK.txt", ios::in | ios::binary);
    istream is(&bin_sealpubK_handler);
    init_public_key.load(context,is);
    bin_sealpubK_handler.close();

Encryptor init_encryptor(context, init_public_key);
Evaluator evaluator(context);
/*   Get in all the TFHE params and Keys*/

//FILE * tgsw_oparams_file = fopen("../data/System Parameters/params_tgsw_out.txt" , "r");
//TGswParams * out_tgsw_params = new_tGswParams_fromFile(tgsw_oparams_file);
//fclose(tgsw_oparams_file);
FILE * lwe_oparams_file = fopen("../data/System Parameters/params_lwe_out.txt" , "r");
LweParams * out_lwe_params = new_lweParams_fromFile(lwe_oparams_file);
fclose(lwe_oparams_file);
FILE * tlwe_iparams_file = fopen("../data/System Parameters/params_tlwe_in.txt" , "r");
TLweParams * init_tlwe_params = new_tLweParams_fromFile(tlwe_iparams_file);
fclose(tlwe_iparams_file);
FILE * tlwe_oparams_file = fopen("../data/System Parameters/params_tlwe_out.txt" , "r");
TLweParams * out_tlwe_params = new_tLweParams_fromFile(tlwe_oparams_file);
fclose(tlwe_oparams_file);
FILE * lwe_iparams_file = fopen("../data/System Parameters/params_lwe_in.txt" , "r");
LweParams * init_lwe_params = new_lweParams_fromFile(lwe_iparams_file);
fclose(lwe_iparams_file);
// FILE * boot_key_file = fopen("../data/Traveller/bootK.data" , "r");
// LweBootstrappingKey * boot_key = new_lweBootstrappingKey_fromFile(boot_key_file);
// LweBootstrappingKeyFFT * boot_key_fft = new_LweBootstrappingKeyFFT(boot_key);


/**
  * Set the initial parameterss
  */
  const int N_seal = parms.poly_modulus_degree();
  const uint64_t p = parms.plain_modulus().value();  // This need s to be changed as per requirement
  //cout  << "Value of p: " << p << endl;
  const int precision = 3;

   const int vector_size = 1024;
  if (vector_size > N_seal)
  {
    std::cout << "The size of Poly Modulus Degree has to be greater than that of the Biometrics vectors" << '\n';
    return -1;
  }

// Assuming test values are read into a Plaintext object named 'test'
Plaintext test;
Plaintext test_inverse;
Plaintext testSquare;
Plaintext testCos;

std::string fileName_test = "../data/E-Gate/Test_Biometrics1";// + std::string(argv[2]);
dataIO::readPlaintext(&test, fileName_test, vector_size, precision, p, 1);  
dataIO::readPlaintext(&test_inverse, fileName_test, vector_size, precision, p, 0);
dataIO::readPlaintextSquare(&testSquare, fileName_test, vector_size, precision, p);
dataIO::readPlaintextCosine(&testCos, fileName_test, vector_size, precision, p,1);




Ciphertext testSEALCipher;
Ciphertext InvtestSEALCipher;
Ciphertext testSEALCipherSquare;
//Ciphertext testSEALCipherSquare_auto;
Ciphertext testSEALCosineCipher;

init_encryptor.encrypt(test, testSEALCipher);
init_encryptor.encrypt(test_inverse, InvtestSEALCipher);
init_encryptor.encrypt(testSquare, testSEALCipherSquare);
init_encryptor.encrypt(testCos, testSEALCosineCipher);

//evaluator.square(testSEALCipher, testSEALCipherSquare_auto);

//cout << "The size of the ciphertext tests is " << testSEALCipher.size() << endl;


Ciphertext modelSEALCipher; 
Ciphertext modelSEALCipherSquare; 
Ciphertext modelSEALCosineCipher;


/* *********** Read data from file *******************/
//Here we need to understand how the data are stored in the file? And also how are the tokens written. ? That will determine how I am going to read from the file. While line should i read or from which point should the reading start.

filebuf bin_sealmodelciph_handler;
bin_sealmodelciph_handler.open("../data/CountryDB/modelciph.txt", ios::in | ios::binary);
istream is2(&bin_sealmodelciph_handler);
// Read the token length and token
//size_t token_length;
//is.read(reinterpret_cast<char*>(&token_length), sizeof(token_length));
//std::string token(token_length, '\0');
//is.read(&token[0], token_length); 
modelSEALCipher.load(context,is2);
//modelSEALCipherSquare.load(context,is2);  
bin_sealmodelciph_handler.close();

filebuf bin_sealmodelsqciph_handler;
bin_sealmodelsqciph_handler.open("../data/CountryDB/modelsqciph.txt", ios::in | ios::binary);
istream is3(&bin_sealmodelsqciph_handler);
modelSEALCipherSquare.load(context,is3);  
bin_sealmodelsqciph_handler.close();


filebuf bin_sealmodelcosineciph_handler;
bin_sealmodelcosineciph_handler.open("../data/CountryDB/modelcosineciph.txt", ios::in | ios::binary);
istream is4(&bin_sealmodelcosineciph_handler);
modelSEALCosineCipher.load(context,is4);  
bin_sealmodelcosineciph_handler.close();

/******************************************************* */
Ciphertext distanceVV;
Ciphertext distanceVV1;
Ciphertext distanceVV2;
Ciphertext distanceVV3;
Ciphertext distanceVV4;

Ciphertext distance_diff;
Ciphertext threshold_dist;

Plaintext Threshold_value;
//int32_t threshold_val = 2000 ;
dataIO::makePlaintext(&Threshold_value, threshold_val, N_seal, p);


//cout << "Threshold value taken in... "  << endl;
RelinKeys relin_key;
clock_t bin_start1=clock();
// //Option 1: Using the encrypted test
evaluator.multiply(modelSEALCipher,testSEALCipher,distanceVV);
//cout << "The sie of the ciphertext after multiplying ab is " << distanceVV.size() << endl;
if (parms.poly_modulus_degree() > 2047) 
{
//cout << "Copying Relin keys" << endl;

filebuf bin_sealrelinK_handler;
  bin_sealrelinK_handler.open("../data/Traveller/sealrelinK.txt", ios::in | ios::binary);
    istream is_relin(&bin_sealrelinK_handler);
    relin_key.load(context,is_relin);
  //bin_sealpubK_handler.write((char *) &init_public_key, sizeof(PublicKey));
  bin_sealrelinK_handler.close();
//cout << "Trying to relinearize" << endl;
evaluator.relinearize_inplace(distanceVV, relin_key);
distanceVV4= distanceVV;
//cout << "The size of the ciphertext after multiplying ab with relinearize is " << distanceVV.size() << endl;

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
//cout << "Now computing the diff from Threshold distance " << endl;
evaluator.negate_inplace(threshold_dist);
evaluator.add_plain_inplace(threshold_dist,Threshold_value);
clock_t bin_end1=clock();

clock_t cos_start2 = clock();
/*********Computing the cosine */
Ciphertext distanceCos;
evaluator.multiply(modelSEALCosineCipher,testSEALCosineCipher,distanceCos);
evaluator.relinearize_inplace(distanceCos, relin_key);
clock_t cos_end2 = clock();
// Need to do an identity bootstap here in the tfhe domain. 

auto context_data = context.first_context_data();
while (context_data->next_context_data())
    {
        //cout << " Level (chain index): " << context_data->chain_index() << endl;
        //cout << "      Noise budget at this level: " << decryptor.invariant_noise_budget(encrypted) << " bits" << endl;
        //cout << "\\" << endl;
        //cout << " \\-->";
       // evaluator.mod_switch_to_next_inplace(distanceVV1);
       // evaluator.mod_switch_to_next_inplace(distanceVV2);
       // evaluator.mod_switch_to_next_inplace(distanceVV3);
        evaluator.mod_switch_to_next_inplace(distance_diff);
        evaluator.mod_switch_to_next_inplace(threshold_dist);
        evaluator.mod_switch_to_next_inplace(distanceCos);
        context_data = context_data->next_context_data();
    }
       //cout << " Level (chain index): " << context_data->chain_index() << endl;
 //   cout << "      Noise budget at this level: " << decryptor.invariant_noise_budget(distanceVV1) << " bits" << endl;
    //cout << "\\" << endl;
    //cout << " \\-->";
    //cout << " End of chain reached" << endl << endl;

//Option 2: Using the plaintext test
// evaluator.multiply_plain(modelSEALCipher, test, distanceVV);
// evaluator.add_inplace(distanceVV,distanceVV);
// evaluator.negate_inplace(distanceVV);
// distanceVV1 = distanceVV;
// evaluator.add_plain_inplace(distanceVV,testSquare); 
// //evaluator.sub(distanceVV, distanceVV1, distanceVV2);
// distanceVV2=distanceVV;
// evaluator.add_inplace(distanceVV, modelSEALCipherSquare); 
// distanceVV3=modelSEALCipherSquare;
// distance_diff=distanceVV;

//cout << "Euclidean distances and Cosine similarity computed" << endl;
// Compute if the distance is below the threshold ?

//evaluator.negate_inplace(Threhsold_dist);
//evaluator.add_inplace(distance_diff,Threhsold_dist);

/**** Uncomment later  */

//Convert the SEAL samples to TFHE
// TLweSample * tlwe_temp = new_TLweSample(init_tlwe_params); //need the init_tlwe_params.
// LweSample * lwe_distance_diff = new_LweSample(init_lwe_params);

// cout << "The value of N is " << tlwe_temp->a[0].N << endl;

 //seal2tfhe::ciphertext_seal_exit(tlwe_temp, distance_diff, context); // here we are translating the distance_diff for the time being, but we actually need the diff btw the distance and the threshold.
 //tLweExtractLweSampleIndex(lwe_distance_diff, tlwe_temp, 4096 - 1, init_lwe_params, init_tlwe_params);
 //Here check if it is the correct level, or it is a different key, as there is some keyswtich that is happening. 

 //LweSample * lweDeltaM = new_LweSample(init_lwe_params); // Defining the variable that will store the value of the sign bootstrapping.

// LweSample * lweRotation = new_LweSample(init_lwe_params); // Defining the variable that will store the value of the rotation insude bootstrapping.

/******************************************************************************* */
/* Temporarily donwloading the init_lwe_key for testing purposes....delete later*/
// FILE * init_lweKey_file = fopen("../data/Traveller/lwe_in.txt" , "r");
// LweKey * init_lwe_key = new_lweKey_fromFile(init_lweKey_file);
// fclose(init_lweKey_file);
// FILE * init1_lweKey_file = fopen("../data/Traveller/lwe_in1.txt" , "r");
// LweKey * init1_lwe_key = new_lweKey_fromFile(init1_lweKey_file);
// fclose(init1_lweKey_file);
// FILE * init_tlweKey_file = fopen("../data/Traveller/tlwe_in.txt" , "r");
// TLweKey * init_tlwe_key = new_tlweKey_fromFile(init_tlweKey_file);
// fclose(init_tlweKey_file);
// FILE * out1_tgswKey_file = fopen("../data/HEComp/tgsw_out1.txt" , "r");
// TGswKey * out1_tgsw_key = new_tgswKey_fromFile(out1_tgswKey_file);
// fclose(out1_tgswKey_file);
// TLweKey * out1_tlwe_key = &(out1_tgsw_key->tlwe_key);
//LweKey * temp_lwe_key = new_LweKey(init_lwe_params);
//tLweExtractKey(temp_lwe_key, init_tlwe_key); 

// cout << "Doing some necessary tests" << endl;
// uint32_t p_temp = 100;
// uint32_t N_temp = 4096;
// TorusPolynomial * T_Poly = new_TorusPolynomial(N_temp);
// for (int i = 0; i < N_temp-1; i++)
// {
//   //int tempval = rand() % p;
//   T_Poly->coefsT[i] = modSwitchToTorus32(0, p_temp);
//   if (i > N_temp-6)   // This is to check the last 5 values
//     {std::cout << " " << T_Poly->coefsT[i] ;
//     cout << " Value after translation " << modSwitchFromTorus32(T_Poly->coefsT[i], p_temp) << endl;}
// }
// T_Poly->coefsT[N_temp-1] = modSwitchToTorus32(rand()% p_temp, p_temp);
// std::cout << " " << T_Poly->coefsT[N_temp-1] ;
//     cout << " Value after translation " << modSwitchFromTorus32(T_Poly->coefsT[N_temp-1], p_temp) << endl;
// cout << endl;
// TLweSample * tlwe_temp_test = new_TLweSample(init_tlwe_params);
// LweSample * lwe_temp_test = new_LweSample(init_lwe_params);
// tLweSymEncrypt(tlwe_temp_test, T_Poly, init_tlwe_params->alpha_max , init_tlwe_key);
// TorusPolynomial * T2_Poly = new_TorusPolynomial(N_temp);
// cout << "Trying to decrypt using tlwe key" << endl;
// tLweSymDecrypt(T2_Poly, tlwe_temp_test, init_tlwe_key, p_temp);
// cout << "Done Sym decrypt" << endl;
// Torus32 t_Temp;
// int i_Temp;
// for (int i = N_temp-5; i < N_temp; i++)
// {
//     // This is to check the last 5 values
//     std::cout << T2_Poly->coefsT[i] ;
//     cout << " Value after translation " << modSwitchFromTorus32(T2_Poly->coefsT[i], p_temp) << endl;
//     tLweExtractLweSampleIndex(lwe_temp_test, tlwe_temp_test, i, init_lwe_params, init_tlwe_params);
//     t_Temp = lweSymDecrypt(lwe_temp_test, init_lwe_key, p_temp);
//     i_Temp = modSwitchFromTorus32(t_Temp, p_temp);
// //double doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
// std::cout << "The value stored in the " << i << "th coefficeint is   " << i_Temp <<  '\n';
// }
// tLweExtractLweSampleIndex(lwe_temp_test, tlwe_temp_test, 4096 - 1, init_lwe_params, init_tlwe_params);
// Torus32 t_Temp = lweSymDecrypt(lwe_temp_test, init_lwe_key, p);
// i_Temp = modSwitchFromTorus32(t_Temp, p);
// //double doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
// std::cout << "The value stored in the last coefficeint is   " << i_Temp <<  '\n';

SecretKey init_seal_key;
filebuf bin_sealpvtK_handler;
  bin_sealpvtK_handler.open("../data/Traveller/sealpvtK.txt", ios::in | ios::binary);
    istream is_temp(&bin_sealpvtK_handler);
    init_seal_key.load(context,is_temp);
  //bin_sealpubK_handler.write((char *) &init_public_key, sizeof(PublicKey));
  bin_sealpvtK_handler.close();
Decryptor decryptor(context, init_seal_key);

Plaintext  temp_dec;
decryptor.decrypt(distanceVV1, temp_dec);
decryptor.decrypt(distanceVV2, temp_dec);

/******************************************* */
// uint64_t * c2 = sealCipher.data(1);
//   uint32_t * c1=(uint32_t*)malloc(sizeof(uint32_t)*N_tfhe);
//   for (size_t i = 0; i < N_tfhe; i++)
//      c1[i]= c2[N_seal- N_tfhe + i] & 0xFFFFFFFF; // This is to get the last 32 bits
//   uint32_t c3;



/********************************************** */
static long int pow10[12] =
    {
        1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000
    };

Torus32 torusTemp;
int intTemp;
double doubleTemp;
TLweSample * tlwe_act = new_TLweSample(init_tlwe_params);
LweSample * lwe_act = new_LweSample(init_lwe_params);
TLweSample * tlwe_temp1 = new_TLweSample(init_tlwe_params);
LweSample * lwe_temp1 = new_LweSample(init_lwe_params);
// TLweSample * tlwe_temp2 = new_TLweSample(init_tlwe_params);
// LweSample * lwe_temp2 = new_LweSample(init_lwe_params);
// TLweSample * tlwe_temp3 = new_TLweSample(init_tlwe_params);
// LweSample * lwe_temp3 = new_LweSample(init_lwe_params);

clock_t bin_start2=clock();
seal2tfhe::ciphertext_seal_exit(tlwe_act, threshold_dist, context);

tLweExtractLweSampleIndex(lwe_act, tlwe_act, 4095, init_lwe_params, init_tlwe_params);
// torusTemp = lweSymDecrypt(lwe_act, init_lwe_key, p);
// intTemp = modSwitchFromTorus32(torusTemp, p);
// doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
// std::cout << "The threshold-distance difference was  " << intTemp <<"  :: :: " << doubleTemp <<  '\n';


seal2tfhe::ciphertext_seal_exit(tlwe_temp1, distance_diff, context);
//cout << "Reached here, no segfault" << endl;
tLweExtractLweSampleIndex(lwe_temp1, tlwe_temp1, 4095, init_lwe_params, init_tlwe_params);
clock_t bin_end2=clock();

// //cout << "Reached here, no segfault" << endl;
// torusTemp = lweSymDecrypt(lwe_temp1, init_lwe_key, p);
// intTemp = modSwitchFromTorus32(torusTemp, p);
// doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
// std::cout << "The actual distance was  " << intTemp <<"  :: :: " << doubleTemp <<  '\n';



// seal2tfhe::ciphertext_seal_exit(tlwe_temp2, distanceVV2, context);
// tLweExtractLweSampleIndex(lwe_temp2, tlwe_temp2, 4095, init_lwe_params, init_tlwe_params);
// //cout << "Reached here, no segfault" << endl;

// seal2tfhe::ciphertext_seal_exit(tlwe_temp3, modelSEALCipherSquare, context);
// tLweExtractLweSampleIndex(lwe_temp3, tlwe_temp3,4095, init_lwe_params, init_tlwe_params);
// //cout << "Reached here, no segfault" << endl;
// torusTemp = lweSymDecrypt(lwe_act, init_lwe_key, p);
// intTemp = modSwitchFromTorus32(torusTemp, p);
// doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
// std::cout << "The actual distance  " << intTemp <<"  :: :: " << doubleTemp <<  '\n';
// torusTemp = lweSymDecrypt(lwe_temp1, init_lwe_key, p);
// intTemp = modSwitchFromTorus32(torusTemp, p);
// doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
// std::cout << "The distance -2ab  " << intTemp <<"  :: :: " << doubleTemp <<  '\n';
// torusTemp = lweSymDecrypt(lwe_temp2, init_lwe_key, p);
// intTemp = modSwitchFromTorus32(torusTemp, p);
// doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
// std::cout << "The distance  b^2-2ab  " << intTemp <<"  :: :: " << doubleTemp <<'\n';
// torusTemp = lwePhase(lwe_temp3, init_lwe_key);
// cout << "The phase of the model square is " << torusTemp << endl;
// torusTemp = lweSymDecrypt(lwe_temp3, init_lwe_key, p);
// intTemp = modSwitchFromTorus32(torusTemp, p);
// doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
// std::cout << "The distance  a^2  " << intTemp <<"  :: :: " << doubleTemp <<'\n';

// /**************************************************************** */

// /******************************************************************************** */
FILE * boot_key_file = fopen("../data/Traveller/bootK.data" , "r"); 
LweBootstrappingKey * boot_key = new_lweBootstrappingKey_fromFile(boot_key_file);
LweBootstrappingKeyFFT * boot_key_fft = new_LweBootstrappingKeyFFT(boot_key);


FILE * ks_med_key_file = fopen("../data/Traveller/KSKmed.data" , "r");
LweKeySwitchKey * ks_med_key = new_lweKeySwitchKey_fromFile(ks_med_key_file);
fclose(ks_med_key_file);

//cout << "Reading the KS key for switching from HE key to EGate key" << endl;
FILE * ks_key_file = fopen("../data/E-Gate/KSK.data", "r");
LweKeySwitchKey * ks_key = new_lweKeySwitchKey_fromFile(ks_key_file);
fclose(ks_key_file);


// Check if the lwekey exists in the E-Gate folder
// cout << "Reading the lwekey for EGate" << endl;
  FILE * out2_lwekey_file = fopen("../data/E-Gate/lwe_out2.txt", "r");
//  if (out2_lwekey_file == NULL) {
    // cout << "E-Gate not registered previously not found. Generating E-Gate keys..." << endl;
    // EGateKeyGen::generateEGatekeys();
    // FILE * out2_lwekey_file = fopen("../data/E-Gate/lwe_out2.txt", "r");
    // LweKey * out2_lwe_key = new_lweKey_fromFile(out2_lwekey_file);
    // fclose(out2_lwekey_file);
    // } else {
    LweKey * out2_lwe_key = new_lweKey_fromFile(out2_lwekey_file);
    fclose(out2_lwekey_file);
    // }



 LweSample * lweDeltaM = new_LweSample(out_lwe_params); // Defining the variable that will store the value of the sign bootstrapping.
 LweSample * lweRotation = new_LweSample(out_lwe_params); // Defining the variable that will store the value of the rotation insude bootstrapping.

// int torusDivision = 2;
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
//lweAddTo(lweDeltaM,lweRotation,out_lwe_params);
LweSample * lweresult = new_LweSample(out_lwe_params);


/* Delete this key later, only for test*/
FILE * out1_lweKey_file = fopen("../data/HEComp/lwe_out1.txt" , "r"); 
LweKey * out1_lwe_key= new_lweKey_fromFile(out1_lweKey_file);
fclose(out1_lweKey_file); // Reading the already predefined Lwe keys from file; Predefined while generating keys for HE component.

FILE * ks_inout_key_file = fopen("../data/Traveller/KSKinout.data" , "r");
LweKeySwitchKey * ks_inout_key = new_lweKeySwitchKey_fromFile(ks_inout_key_file);
fclose(ks_inout_key_file);

LweSample * lweDelta_boot = new_LweSample(out_lwe_params);
lweKeySwitch(lweDelta_boot,ks_inout_key,lweDeltaM);

torusTemp = lweSymDecrypt(lweDeltaM, out1_lwe_key, torusDivision);
int sign = modSwitchFromTorus32(torusTemp, torusDivision);
if (sign == 1)
    cout << "The distance is less than the threshold, same person (Accept). " << endl;
else
    cout << "The distance is greater than the threshold, further investigation needed. " << endl;
//std::cout << " Sign is : " << sign <<  "/" << torusDivision << '\n';

cout << " **************************************************************" << endl;
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
clock_t cos_end1 = clock();

double cos_duration = double(cos_end1 - cos_start1 + cos_end2 - cos_start2) / CLOCKS_PER_SEC;

torusTemp = lweSymDecrypt(lweCos_boot, out1_lwe_key, p);
intTemp = modSwitchFromTorus32(torusTemp, p);
doubleTemp = ((double) intTemp - p*(intTemp >= p/2))/pow10[2*(precision-1)];
std::cout << "The cosine similarity value is :: :: " << doubleTemp <<  '\n';
cout << "***************************************************************************" << endl;
cout << "Total time for accept/reject check : " << bin_duration << " seconds" << endl;

std::cout << "Cosine computtion duration: " << cos_duration << " seconds" << std::endl;



    return 0;
}
