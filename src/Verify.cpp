#include <cstdio>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <system_error>

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

static bool looks_like_enc_dir(const fs::path &p)
{
  std::error_code ec;
  if (!fs::is_directory(p, ec) || ec) return false;

  // dataIO::Read_F expects these exact files to exist under the directory
  return fs::exists(p / "ciph", ec) && fs::exists(p / "sqciph", ec) && fs::exists(p / "cosineciph", ec);
}

static FILE* fopen_or_die(const fs::path &p, const char *mode)
{
  FILE *f = fopen(p.c_str(), mode);
  if (!f) {
    throw std::runtime_error("Cannot open file: " + p.string());
  }
  return f;
}

int main(int argc, char *argv[])
{
  try {
    if (argc < 4) {
      std::cerr << "Usage: " << argv[0]
      << " <threshold_value> <Test Biometric folder> <Stored Biometric folder>\n";
      return 1;
    }

    int32_t threshold_val = std::stoi(argv[1]);  // e.g. 2000
    srand((unsigned)time(nullptr));

    // Resolve data directory relative to executable location (portable; no cwd assumptions)
    fs::path exe_path = fs::weakly_canonical(fs::path(argv[0]));
    fs::path exe_dir  = exe_path.parent_path();
    fs::path data_dir = fs::weakly_canonical(exe_dir / ".." / "data");

    // ---------------- SEAL params & keys ----------------
    std::cout << "Reading SEAL parameters from file..." << std::endl;

    fs::path sealparam_path = data_dir / "System_Parameters" / "sealparam.txt";
    std::ifstream bin_sealparam_handler(sealparam_path, std::ios::in | std::ios::binary);
    if (!bin_sealparam_handler) {
      std::cerr << "Seal param file did not open: " << sealparam_path << std::endl;
      return 1;
    }

    EncryptionParameters parms(scheme_type::bfv);
    parms.load(bin_sealparam_handler);
    bin_sealparam_handler.close();

    SEALContext context(parms);

    PublicKey init_public_key;
    fs::path sealpubk_path = data_dir / "Traveller" / "sealpubK.txt";
    std::filebuf bin_sealpubK_handler;
    if (!bin_sealpubK_handler.open(sealpubk_path, ios::in | ios::binary)) {
      std::cerr << "Seal public key did not open: " << sealpubk_path << std::endl;
      return 1;
    }
    {
      istream is(&bin_sealpubK_handler);
      init_public_key.load(context, is);
    }
    bin_sealpubK_handler.close();

    Encryptor init_encryptor(context, init_public_key);
    Evaluator evaluator(context);

    // ---------------- TFHE params & keys ----------------
    FILE * lwe_oparams_file  = fopen_or_die(data_dir / "System_Parameters" / "params_lwe_out.txt", "r");
    LweParams * out_lwe_params = new_lweParams_fromFile(lwe_oparams_file);
    fclose(lwe_oparams_file);

    FILE * tlwe_iparams_file = fopen_or_die(data_dir / "System_Parameters" / "params_tlwe_in.txt", "r");
    TLweParams * init_tlwe_params = new_tLweParams_fromFile(tlwe_iparams_file);
    fclose(tlwe_iparams_file);

    FILE * tlwe_oparams_file = fopen_or_die(data_dir / "System_Parameters" / "params_tlwe_out.txt", "r");
    TLweParams * out_tlwe_params = new_tLweParams_fromFile(tlwe_oparams_file);
    fclose(tlwe_oparams_file);

    FILE * lwe_iparams_file  = fopen_or_die(data_dir / "System_Parameters" / "params_lwe_in.txt", "r");
    LweParams * init_lwe_params = new_lweParams_fromFile(lwe_iparams_file);
    fclose(lwe_iparams_file);

    // Biometrics / encoding parameters
    const int N_seal = (int)parms.poly_modulus_degree();
    const uint64_t p = parms.plain_modulus().value();
    const int precision = 3;
    const int vector_size = 1024;
    if (vector_size > N_seal) {
      std::cout << "The poly_modulus_degree must be >= biometric vector size\n";
      return -1;
    }

    // Ciphertexts
    Ciphertext testSEALCipher, testSEALCipherSquare, testSEALCosineCipher;
    Ciphertext modelSEALCipher, modelSEALCipherSquare, modelSEALCosineCipher;

    // Relin keys
    RelinKeys relin_key;
    fs::path relin_path = data_dir / "Traveller" / "sealrelinK.txt";
    std::filebuf bin_sealrelinK_handler;
    if (!bin_sealrelinK_handler.open(relin_path, ios::in | ios::binary)) {
      std::cerr << "Relin key did not open: " << relin_path << std::endl;
      return 1;
    }
    {
      istream is_relin(&bin_sealrelinK_handler);
      relin_key.load(context, is_relin);
    }
    bin_sealrelinK_handler.close();

    // Bootstrapping / KS keys
    FILE * boot_key_file = fopen_or_die(data_dir / "Traveller" / "bootK.data", "r");
    LweBootstrappingKey * boot_key = new_lweBootstrappingKey_fromFile(boot_key_file);
    LweBootstrappingKeyFFT * boot_key_fft = new_LweBootstrappingKeyFFT(boot_key);
    fclose(boot_key_file);

    FILE * ks_med_key_file = fopen_or_die(data_dir / "Traveller" / "KSKmed.data", "r");
    LweKeySwitchKey * ks_med_key = new_lweKeySwitchKey_fromFile(ks_med_key_file);
    fclose(ks_med_key_file);

    FILE * ks_inout_key_file = fopen_or_die(data_dir / "Traveller" / "KSKinout.data", "r");
    LweKeySwitchKey * ks_inout_key = new_lweKeySwitchKey_fromFile(ks_inout_key_file);
    fclose(ks_inout_key_file);

    FILE * ks_key_file = fopen_or_die(data_dir / "E-Gate" / "KSK.data", "r");
    LweKeySwitchKey * ks_key = new_lweKeySwitchKey_fromFile(ks_key_file);
    fclose(ks_key_file);

    FILE * out2_lwekey_file = fopen_or_die(data_dir / "E-Gate" / "lwe_out2.txt", "r");
    LweKey * out2_lwe_key = new_lweKey_fromFile(out2_lwekey_file);
    fclose(out2_lwekey_file);

    // ---------------- Input folders ----------------
    fs::path folderName_model_new = fs::path(argv[2]);
    fs::path folderName_model_old = fs::path(argv[3]);

    if (!fs::exists(folderName_model_new) || !fs::is_directory(folderName_model_new)) {
      std::cerr << "Error: Test folder does not exist or is not a directory: " << folderName_model_new << std::endl;
      return 1;
    }
    if (!fs::exists(folderName_model_old) || !fs::is_directory(folderName_model_old)) {
      std::cerr << "Error: Stored folder does not exist or is not a directory: " << folderName_model_old << std::endl;
      return 1;
    }

    // Discover encrypted biometric directories under test folder
    std::vector<fs::path> biometric_dirs;
    for (const auto &entry : fs::directory_iterator(folderName_model_new)) {
      if (looks_like_enc_dir(entry.path())) {
        biometric_dirs.push_back(entry.path().filename()); // keep just the name
      }
    }

    if (biometric_dirs.empty()) {
      std::cerr << "Error: No encrypted biometric directories found in test folder: " << folderName_model_new << "\n";
      std::cerr << "Expected subdirectories containing {ciph,sqciph,cosineciph}\n";
      return 1;
    }

    for (const auto &bio_dir_name : biometric_dirs) {
      fs::path test_dir  = folderName_model_new / bio_dir_name;
      fs::path stored_dir = folderName_model_old / bio_dir_name;

      if (!looks_like_enc_dir(test_dir)) {
        std::cerr << "Skipping invalid test entry: " << test_dir << "\n";
        continue;
      }
      if (!looks_like_enc_dir(stored_dir)) {
        std::cerr << "Missing stored biometric directory: " << stored_dir << "\n";
        continue;
      }

      // Read ciphertext bundles
      dataIO::Read_F(test_dir.string(), context, testSEALCipher, testSEALCipherSquare, testSEALCosineCipher);
      dataIO::Read_F(stored_dir.string(), context, modelSEALCipher, modelSEALCipherSquare, modelSEALCosineCipher);

      // ------------------------------------------------------------
      // Compute distance + threshold compare (existing logic preserved)
      Ciphertext distanceVV, distanceVV1, distanceVV2, distanceVV3, distanceVV4;
      Ciphertext distance_diff, threshold_dist;
      Plaintext Threshold_value;

      dataIO::makePlaintext(&Threshold_value, threshold_val, N_seal, (uint32_t)p);
      cout << "Threshold value taken in, set to " << threshold_val << endl;

      clock_t bin_start1 = clock();

      evaluator.multiply(modelSEALCipher, testSEALCipher, distanceVV);
      if (parms.poly_modulus_degree() > 2047) {
        evaluator.relinearize_inplace(distanceVV, relin_key);
        distanceVV4 = distanceVV;
      }

      evaluator.add_inplace(distanceVV, distanceVV);
      evaluator.negate_inplace(distanceVV);
      distanceVV1 = distanceVV;
      evaluator.add_inplace(distanceVV, testSEALCipherSquare);
      distanceVV2 = testSEALCipherSquare;
      evaluator.add_inplace(distanceVV, modelSEALCipherSquare);
      distanceVV3 = modelSEALCipherSquare;
      distance_diff = distanceVV;

      threshold_dist = distance_diff;
      evaluator.negate_inplace(threshold_dist);
      evaluator.add_plain_inplace(threshold_dist, Threshold_value);

      clock_t bin_end1 = clock();

      clock_t cos_start2 = clock();
      Ciphertext distanceCos;
      evaluator.multiply(modelSEALCosineCipher, testSEALCosineCipher, distanceCos);
      evaluator.relinearize_inplace(distanceCos, relin_key);
      clock_t cos_end2 = clock();

      auto context_data = context.first_context_data();
      while (context_data->next_context_data()) {
        evaluator.mod_switch_to_next_inplace(distance_diff);
        evaluator.mod_switch_to_next_inplace(threshold_dist);
        evaluator.mod_switch_to_next_inplace(distanceCos);
        context_data = context_data->next_context_data();
      }

      TLweSample * tlwe_act = new_TLweSample(init_tlwe_params);
      LweSample * lwe_act = new_LweSample(init_lwe_params);

      clock_t bin_start2 = clock();
      seal2tfhe::ciphertext_seal_exit(tlwe_act, threshold_dist, context);
      tLweExtractLweSampleIndex(lwe_act, tlwe_act, 4095, init_lwe_params, init_tlwe_params);
      clock_t bin_end2 = clock();

      LweSample * lweDeltaM = new_LweSample(out_lwe_params);
      LweSample * lweRotation = new_LweSample(out_lwe_params);

      int torusDivision = 16;
      Torus32 positive = modSwitchToTorus32(1, torusDivision);
      lweNoiselessTrivial(lweRotation, positive, out_lwe_params);

      clock_t bin_start3 = clock();
      LweSample * lwe_act1 = new_LweSample(init_lwe_params);
      lweKeySwitch(lwe_act1, ks_med_key, lwe_act);
      tfhe_bootstrap_woKS_FFT(lweDeltaM, boot_key_fft, positive, lwe_act1);
      clock_t bin_end3 = clock();

      // Switch to E-Gate key and decrypt sign
      LweSample * lweDelta_boot = new_LweSample(out_lwe_params);
      lweKeySwitch(lweDelta_boot, ks_inout_key, lweDeltaM);
      LweSample * lweDelta_boot_final = new_LweSample(out_lwe_params);
      lweKeySwitch(lweDelta_boot_final, ks_key, lweDelta_boot);

      Torus32 torusTemp = lweSymDecrypt(lweDelta_boot_final, out2_lwe_key, torusDivision);
      int sign = modSwitchFromTorus32(torusTemp, torusDivision);

      string flag = (sign == 1) ? "Accept" : "Reject";

      double bin_duration = double(bin_end3 - bin_start3 + bin_end2 - bin_start2 + bin_end1 - bin_start1) / CLOCKS_PER_SEC;

      // Cosine decrypt
      TLweSample * tlwe_act_cos = new_TLweSample(init_tlwe_params);
      LweSample * lwe_act_cos = new_LweSample(init_lwe_params);
      LweSample * lwe_act_cos_ks = new_LweSample(init_lwe_params);

      clock_t cos_start1 = clock();
      seal2tfhe::ciphertext_seal_exit(tlwe_act_cos, distanceCos, context);
      tLweExtractLweSampleIndex(lwe_act_cos, tlwe_act_cos, 4095, init_lwe_params, init_tlwe_params);
      lweKeySwitch(lwe_act_cos_ks, ks_med_key, lwe_act_cos);

      LweSample * lweCos_boot = new_LweSample(out_lwe_params);
      lweKeySwitch(lweCos_boot, ks_inout_key, lwe_act_cos_ks);
      LweSample * lweCos_boot_final = new_LweSample(out_lwe_params);
      lweKeySwitch(lweCos_boot_final, ks_key, lweCos_boot);
      clock_t cos_end1 = clock();

      double cos_duration = double(cos_end1 - cos_start1 + cos_end2 - cos_start2) / CLOCKS_PER_SEC;

      static long int pow10[12] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000 };
      torusTemp = lweSymDecrypt(lweCos_boot_final, out2_lwe_key, (int)p);
      int intTemp = modSwitchFromTorus32(torusTemp, (int)p);
      double doubleTemp = ((double)intTemp - p * (intTemp >= (int)p/2)) / pow10[2*(precision-1)];

      cout << " **************************************************************" << endl;
      std::cout << bio_dir_name.string() << " || " << flag << " || " << doubleTemp << '\n';
      cout << "***************************************************************************" << endl;
      cout << "Total time for accept/reject check : " << bin_duration << " seconds" << endl;
      std::cout << "Cosine computation duration: " << cos_duration << " seconds" << std::endl;
    }

    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Fatal error: " << e.what() << "\n";
    return 1;
  }
}
