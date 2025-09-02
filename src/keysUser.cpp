/**
 * @file keys_user.cpp
 * @brief This file contains the main function for generating and initializing cryptographic keys using SEAL and TFHE libraries.
 *
 * The main function performs the following tasks:
 * - Reads SEAL encryption parameters from a binary file.
 * - Initializes SEAL context and generates SEAL keys (public and secret keys).
 * - Reads TFHE parameters from text files.
 * - Initializes TFHE keys and bootstrapping keys.
 * - Generates a bootstrapping key unique to each user and exports it to a file.
 *
 * @return 0 on success, 1 if the SEAL parameter file could not be opened, 2 if an error occurred during reading the SEAL parameter file.
 */

// SEAL
#include "seal/seal.h"

// TFHE
#include "tfhe.h"
#include <ctime>
#include "utils.h"
#include "dataIO.h"
#include "token.h"
#include "keysUser.h"
using namespace std;
using namespace seal;

inline void printSecretKeyCoefficients(const SecretKey &secret_key, const SEALContext &context) {
    auto &context_data = *context.key_context_data();
    int N = context_data.parms().poly_modulus_degree();
    uint64_t q = context_data.total_coeff_modulus()[0];

    // Extract the key coefficients
    Plaintext key_poly = secret_key.data();
    uint64_t *key_coefs = key_poly.data();

    // Print the first few coefficients
    cout << "Secret key coefficients (first 10): ";
    for (size_t i = 0; i < 10; i++) {
        cout << key_coefs[i] << " ";
    }
    cout << endl;

    // Check the range of coefficients
    for (size_t i = 0; i < N; i++) {
        if (key_coefs[i] >= q) {
            cout << "Coefficient out of range: " << key_coefs[i] << endl;
        }
    }
}


inline void print_parameters(const seal::SEALContext &context)
{
    auto &context_data = *context.key_context_data();

    /*
    Which scheme are we using?
    */
    std::string scheme_name;
    switch (context_data.parms().scheme())
    {
    case seal::scheme_type::bfv:
        scheme_name = "BFV";
        break;
    case seal::scheme_type::ckks:
        scheme_name = "CKKS";
        break;
    case seal::scheme_type::bgv:
        scheme_name = "BGV";
        break;
    default:
        throw std::invalid_argument("unsupported scheme");
    }
    std::cout << "/" << std::endl;
    std::cout << "| Encryption parameters :" << std::endl;
    std::cout << "|   scheme: " << scheme_name << std::endl;
    std::cout << "|   poly_modulus_degree: " << context_data.parms().poly_modulus_degree() << std::endl;
    
    /*
    Print the size of the true (product) coefficient modulus.
    */
    std::cout << "|   coeff_modulus size: ";
    std::cout << context_data.total_coeff_modulus_bit_count() << " (";
    auto coeff_modulus = context_data.parms().coeff_modulus();
    std::size_t coeff_modulus_size = coeff_modulus.size();
    for (std::size_t i = 0; i < coeff_modulus_size - 1; i++)
    {
        std::cout << coeff_modulus[i].bit_count() << " + ";
    }

    std::cout << coeff_modulus.back().bit_count();
    std::cout << ") bits" << std::endl;
  
    //std::cout << "|   coeff_modulus: " << coeff_modulus << std::endl;

    /*
    For the BFV scheme print the plain_modulus parameter.
    */
    if (context_data.parms().scheme() == seal::scheme_type::bfv)
    {
        std::cout << "|   plain_modulus: " << context_data.parms().plain_modulus().value() << std::endl;
    }

    std::cout << "\\" << std::endl;
}

void customlweCreateKeySwitchKey(LweKeySwitchKey* result, const LweKey* in_key, const LweKey* out_key) {
    const int32_t n_in = in_key->params->n;
    const int32_t n_out = out_key->params->n;
    const int32_t t = result->t;
    const int32_t basebit = result->basebit;
    const int32_t base = 1 << basebit;
    const double alpha = out_key->params->alpha_min;
    const int32_t sizeks = n_out * t * (base - 1);

    cout << "Insude custonlweCreateKeySwitchKey" << endl;
    cout << " Value of base = " << base << endl;
    cout << " Value of KS size = " << sizeks << endl;

    double err = 0;

    // Choose a random vector of Gaussian noises
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0., alpha);
    double* noise = new double[sizeks];
    for (int32_t i = 0; i < sizeks; ++i) {
        noise[i] = distribution(generator);
        err += noise[i];
    }

    cout << "Noise generated" << endl;

    // Recenter the noises
    err = err / sizeks;
    for (int32_t i = 0; i < sizeks; ++i) noise[i] -= err;

    cout << "Noise recentered" << endl;

    // Generate the key-switching key
    int32_t index = 0;
    for (int32_t i = 0; i < n_in; ++i) {
        for (int32_t j = 0; j < t; ++j) {

            // term h=0 as trivial encryption of 0 (it will not be used in the KeySwitching)
            lweNoiselessTrivial(&result->ks[i][j][0], 0, out_key->params);

            for (int32_t h = 1; h < base; ++h) {
                // noiseless encrryption 
                cout << "Inside the base loop" << endl;
                result->ks[i][j][h].b = (in_key->key[i] * h)* (1 << (32 - (j + 1) * basebit));
                for (int32_t p = 0; p < n_out; ++p) {
                    cout << "Inside the n_out loop" << endl;
                    result->ks[i][j][h].a[p] = uniformTorus32_distrib(generator);
                    cout << " Uniform distrinbution generated" << endl;
                    result->ks[i][j][h].b += result->ks[i][j][h].a[p] * out_key->key[p];
                    cout << " The b part has been filled with a*s" << endl;
                }
                result->ks[i][j][h].b += dtot32(noise[index]);
                cout << " The b part has been filled with a*s + noise" << endl;
                // Torus32 mess = (in_key->key[i] * h) * (1 << (32 - (j + 1) * basebit));
                // lweSymEncryptWithExternalNoise(&result->ks[i][j][h], mess, noise[index], alpha, out_key);
                index += 1;
            }
        }
    }

    delete[] noise;
}
void UserKeyGen::generateUserKeys()
{
/***************************************************************************** */
// Open the file containing the parameters
    std::ifstream bin_sealparam_handler("../data/System Parameters/sealparam.txt", std::ios::in | std::ios::binary);
    if (!bin_sealparam_handler) {
        //std::cerr << "Seal param file did not open!" << std::endl;
        throw std::runtime_error("Seal param file did not open!");
    }

    // Read the parameters from the file
    EncryptionParameters parms(scheme_type::bfv);
    //bin_sealparam_handler.read(reinterpret_cast<char*>(&parms), sizeof(EncryptionParameters));
    parms.load(bin_sealparam_handler);
    bin_sealparam_handler.close();

    if (!bin_sealparam_handler.good()) {
        throw std::runtime_error("Seal param file did not close properly!");
    }
SEALContext context(parms);
//print_parameters(context);
KeyGenerator init_keygen(context);

clock_t start_usr_kgen1 = clock();

SecretKey init_seal_key = init_keygen.secret_key(); // need to send this to the user

PublicKey init_public_key ;
 init_keygen.create_public_key(init_public_key);  // must be stored at the DB level so as to be used later. 

if (!is_valid_for(init_public_key,context)){
  throw std::runtime_error("Public key is not valid for this context ");
}
 filebuf bin_sealpubK_handler;
  bin_sealpubK_handler.open("../data/Traveller/sealpubK.txt", ios::out | ios::binary);
    ostream os(&bin_sealpubK_handler);
    init_public_key.save(os);
  //bin_sealpubK_handler.write((char *) &init_public_key, sizeof(PublicKey));
  bin_sealpubK_handler.close();

  filebuf bin_sealpvtK_handler;
  bin_sealpvtK_handler.open("../data/Traveller/sealpvtK.txt", ios::out | ios::binary);
    ostream os2(&bin_sealpvtK_handler);
    init_seal_key.save(os2);
  //bin_sealpubK_handler.write((char *) &init_public_key, sizeof(PublicKey));
  bin_sealpvtK_handler.close();

if (parms.poly_modulus_degree() > 2048 )
{
  RelinKeys relin_keys;
  init_keygen.create_relin_keys(relin_keys);
  filebuf bin_sealrelinK_handler;
  bin_sealrelinK_handler.open("../data/Traveller/sealrelinK.txt", ios::out | ios::binary);
    ostream os3(&bin_sealrelinK_handler);
    relin_keys.save(os3);
  //bin_sealpubK_handler.write((char *) &init_public_key, sizeof(PublicKey));
  bin_sealrelinK_handler.close();

}
clock_t end_usr_kgen1 = clock();
/************************************************************************************/
/**
* Initialize TFHE keys & Bootstapping Keys
*/

// Take in the tfhe params
FILE * tgsw_iparams_file = fopen("../data/System Parameters/params_tgsw_in.txt" , "r");
FILE * lwe_iparams_file = fopen("../data/System Parameters/params_lwe_in.txt" , "r");
//FILE * tlwe_oparams_file = fopen("../data/System Parameters/params_tlwe_out.txt" , "r");
FILE * tgsw_oparams_file = fopen("../data/System Parameters/params_tgsw_out.txt" , "r");
FILE * lwe_oparams_file = fopen("../data/System Parameters/params_lwe_out.txt" , "r");


TGswParams * init_tgsw_params = new_tGswParams_fromFile(tgsw_iparams_file);
LweParams * init_lwe_params = new_lweParams_fromFile(lwe_iparams_file);
TGswParams * out_tgsw_params = new_tGswParams_fromFile(tgsw_oparams_file);
LweParams * out_lwe_params = new_lweParams_fromFile(lwe_oparams_file);
//TLweParams * out_tlwe_params = new_tLweParams_fromFile(tlwe_oparams_file);

clock_t start_usr_kgen2 = clock();
TGswKey * init_tgsw_key = new_TGswKey(init_tgsw_params);
seal2tfhe::secretTGSWKey(init_tgsw_key, init_seal_key, context);
TLweKey * init_tlwe_key = &(init_tgsw_key->tlwe_key);
LweKey * init_lwe_key = new_LweKey(init_lwe_params);
tLweExtractKey(init_lwe_key, init_tlwe_key);  // This is the key that will be used to generate the bootstrapping key. Otherwise it is not used anywhere else. 

TGswKey * init1_tgsw_key = new_TGswKey(init_tgsw_params);
tGswKeyGen(init1_tgsw_key);
TLweKey * init1_tlwe_key = &(init1_tgsw_key->tlwe_key);
LweKey * init1_lwe_key = new_LweKey(init_lwe_params);
tLweExtractKey(init1_lwe_key, init1_tlwe_key);

// // Generate the keyswitch key from the Lwe key of the HE component to the Lwe key of the E-Gate and send to E-Gate.
const int switch_t = 4;//7
const int switch_base = 4;//4 
LweKeySwitchKey * ks_med_key = new_LweKeySwitchKey(4096, switch_t, switch_base, init_lwe_params);
lweCreateKeySwitchKey(ks_med_key, init_lwe_key, init1_lwe_key);
clock_t end_usr_kgen2 = clock();

/****************************************************************** */
/* This following is only for testing purposes...delete in the final version*/
FILE * init_lweKey_file = fopen("../data/Traveller/lwe_in.txt" , "w");
export_lweKey_toFile(init_lweKey_file,init_lwe_key);   
fclose(init_lweKey_file);
FILE * init1_lweKey_file = fopen("../data/Traveller/lwe_in1.txt" , "w");
export_lweKey_toFile(init1_lweKey_file,init1_lwe_key);   
fclose(init1_lweKey_file);
FILE * init_tlweKey_file = fopen("../data/Traveller/tlwe_in.txt" , "w");
export_tlweKey_toFile(init_tlweKey_file,init_tlwe_key);   
fclose(init_tlweKey_file);
/******************************************************************** */
FILE * ks_med_key_file = fopen("../data/Traveller/KSKmed.data" , "w");
export_lweKeySwitchKey_toFile(ks_med_key_file, ks_med_key);
fclose(ks_med_key_file);

FILE * out1_tgswKey_file = fopen("../data/HEComp/tgsw_out1.txt" , "r"); 
TGswKey * out1_tgsw_key= new_tgswKey_fromFile(out1_tgswKey_file);
fclose(out1_tgswKey_file); // Reading the already predefined Tgsw keys from file; Predefined while generating keys for HE component.

TLweKey * out1_tlwe_key = &(out1_tgsw_key->tlwe_key);
LweKey * out1_lwe_key = new_LweKey(out_lwe_params);
tLweExtractKey(out1_lwe_key, out1_tlwe_key);

// TLweSample * ks_raw_inout = new_TLweSample_array(4096*switch_t, out_tlwe_params);
// TLweKeySwitchKey * key_switch_inout = new TLweKeySwitchKey(init_lwe_params->n, switch_t, switch_base, out_tlwe_params, ks_raw_inout);
// TLweKeySwitchKeyFunctions::CreateKeySwitchKey(key_switch_inout, init1_lwe_key, out1_tlwe_key);

//cout << "Generating keyswitch key" << endl;
clock_t start_usr_kgen3 = clock();
LweKeySwitchKey * ks_inout_key = new_LweKeySwitchKey(4096, switch_t, switch_base, out_lwe_params);
//cout << "Key switch key initiated" << endl;
lweCreateKeySwitchKey(ks_inout_key, init1_lwe_key, out1_lwe_key);
//cout << "Key switch key generated" << endl;
clock_t end_usr_kgen3 = clock();
FILE * ks_inout_key_file = fopen("../data/Traveller/KSKinout.data" , "w");
export_lweKeySwitchKey_toFile(ks_inout_key_file, ks_inout_key);
fclose(ks_inout_key_file);


// Bootstrapping key generation; Unique to each user.
// const int switch_t = 8;
// const int switch_base = 2;
clock_t start_usr_kgen4 = clock();
LweBootstrappingKey * boot_key = new_LweBootstrappingKey(switch_t , switch_base, init_lwe_params, out_tgsw_params);
tfhe_createLweBootstrappingKey(boot_key, init1_lwe_key, out1_tgsw_key);
clock_t end_usr_kgen4 = clock();
//LweBootstrappingKeyFFT * boot_key_fft = new_LweBootstrappingKeyFFT(boot_key);
// Export the bootstrapping key to a file
FILE * boot_key_file = fopen("../data/Traveller/bootK.data" , "w");
export_lweBootstrappingKey_toFile(boot_key_file,boot_key);
fclose(boot_key_file);



//need to destroy the tlwe key and the tlwe sample
fclose(tgsw_iparams_file);
fclose(lwe_iparams_file);
fclose(tgsw_oparams_file);
fclose(lwe_oparams_file);
//fclose(tlwe_oparams_file);
double elapsed_secs = double(end_usr_kgen4 - start_usr_kgen4 + end_usr_kgen3 - start_usr_kgen3 + end_usr_kgen2 - start_usr_kgen2 + end_usr_kgen1 - start_usr_kgen1 ) / CLOCKS_PER_SEC;
cout << "********************************************" << endl;
cout << "*     Successfully Generated User Keys     *" << endl;
cout << "********************************************" << endl;
cout << "Time for User Key generation: " << elapsed_secs << " seconds" << endl;
}

std::string generateRandomId(size_t length) {
    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string random_id;
    for (size_t i = 0; i < length; ++i) {
        random_id += characters[distribution(generator)];
    }
    return random_id;
}

// void UserID::generateId() {
//     const size_t id_length = 16; // Length of the random ID
//     std::string user_id = generateRandomId(id_length);

//     // Ensure the ID is unique in the database
//     while (database.contains(user_id)) {
//         user_id = generateRandomId(id_length);
//     }

//     this->id = user_id;
// }
