#include "more_tfhe_io.h"

/*
* Read and write LWE parameters
*/
/*
template<typename Torus32>
LweParams* MoreTfheIO::new_LWE_Params_fromFile(FILE* f)
{
  CIstream temp = to_Istream(f);
  LweParams* params = IOFunctions::read_new_lweParams(temp);
  return params;
}

template<typename Torus32>
void MoreTfheIO::write_LWE_Params_toFile(const LweParams* lwe_params, FILE* f)
{
  COstream temp = to_Ostream(f);
  IOFunctions::write_lweParams(temp, lwe_params);
}
*/
/*
* Read and write LWE public parameters (same but with an m on top for the public encryption)
*/

//template<typename Torus32>
/*
LwePublicParams* MoreTfheIO::new_LWE_PublicParams_fromFile(FILE * f)
{
  CIstream temp = to_Istream(f);
  TextModeProperties* props = new_TextModeProperties_fromIstream(temp);

  if (props->getTypeTitle() != std::string("PUBLIC_LWEPARAMS")) abort();
  int n = props->getProperty_long("n");
  double alpha_min = props->getProperty_double("alpha_min");
  double alpha_max = props->getProperty_double("alpha_max");
  int m = props->getProperty_long("m");
  delete_TextModeProperties(props);

  LweParams* lweparams = new_LweParams(n,alpha_min,alpha_max);
  LwePublicParams* lwe_public_params = new_LwePublicParams(m, lweparams);

  return lwe_public_params;
}

//template<typename Torus32>
void MoreTfheIO::write_PublicLWE_Params_toFile(const LwePublicParams* lwe_public_params, FILE * f)
{
  const LweParams* lweparams = lwe_public_params->params;
  const int m = lwe_public_params->m;

  COstream temp = to_Ostream(f);

  TextModeProperties* props = new_TextModeProperties_blank();
  props->setTypeTitle("PUBLIC_LWEPARAMS");
  props->setProperty_long("n", lweparams->n);
  props->setProperty_double("alpha_min", lweparams->alpha_min);
  props->setProperty_double("alpha_max", lweparams->alpha_max);
  props->setProperty_long("m", lwe_public_params->m);

  print_TextModeProperties_toOStream(temp, props);
  delete_TextModeProperties(props);
}

/*
* Read and write TGSW parameters
* (the other parameters can be extracted from these)
*/
/*
template<typename Torus32>
TGswParams * MoreTfheIO::new_TGSW_Params_fromFile(FILE* f)
{
  CIstream temp = to_Istream(f);
  TGswParams * params = IOFunctions::read_new_tGswParams(temp);
  return params;
}

template<typename Torus32>
void MoreTfheIO::write_TGSW_Params_toFile(const TGswParams * trgsw_params, FILE* f)
{
  COstream temp = to_Ostream(f);
  IOFunctions::write_tGswParams(temp, trgsw_params);
}

/*
* Read and write the TGSW key
* (the other keys should be extracted from these)
*/
/*
template<typename Torus32>
TGswKey * MoreTfheIO::new_TGSW_Key_fromFile(FILE* f)
{
  CIstream temp = to_Istream(f);
  TGswKey * key = IOFunctions::read_new_tGswKey(temp);
  return key;
}

template<typename Torus32>
void MoreTfheIO::write_TGSW_Key_toFile(const TGswKey * trgsw_key, FILE* f)
{
  COstream temp = to_Ostream(f);
  IOFunctions::write_tGswKey(temp, trgsw_key);
}

/*
* Read and write the LWE secret key
*/
/*
template<typename Torus32>
LweKey* MoreTfheIO::new_LWE_Key_fromFile(FILE* f)
{
  CIstream temp = to_Istream(f);
  LweKey* key = IOFunctions::read_new_lweKey(temp);
  return key;
}

template<typename Torus32>
void MoreTfheIO::write_LWE_Key_toFile(const LweKey* lwe_key, FILE* f)
{
  COstream temp = to_Ostream(f);
  IOFunctions::write_lweKey(temp, lwe_key);
}

/*
* Read and write the LWE public key
*/
/*
//template<typename Torus32>
void MoreTfheIO::write_LWE_PublicKey_toFile(const LwePublicKey * lwe_public_key, FILE* f)
{
  const LwePublicParams * params = lwe_public_key->publicParams;
  const int m = params->m;
  const int n = params->params->n;

  LweSample * key = lwe_public_key->key;    // This is actually a pointer to an m-sized array

  COstream temp = to_Ostream(f);

  // temp.fwrite(&PUBLIC_LWE_KEY_TYPE_UID, sizeof(int32_t));
  for (size_t i = 0; i < m; i++)
  {
    temp.fwrite((key+i)->a, sizeof(Torus32)*n);
    temp.fwrite(&(key+i)->b, sizeof(Torus32));
  }
}

//template<typename Torus32>
LwePublicKey * MoreTfheIO::new_LWE_PublicKey_fromFile(FILE* f, const LwePublicParams * params)
{
  const int m = params->m;
  const int n = params->params->n;

  LwePublicKey * pubkey = new_LwePublicKey(params);

  CIstream temp = to_Istream(f);

  // int32_t type_uid;
  // temp.fread(&type_uid, sizeof(int32_t));
  // if (type_uid != PUBLIC_LWE_KEY_TYPE_UID) abort();
  for (size_t i = 0; i < m; i++)
  {
    temp.fread((&pubkey->key[i])->a, sizeof(Torus32)*n);
    temp.fread(&(&pubkey->key[i])->b, sizeof(Torus32));
  }
  return pubkey;
}

/*
* Read and write the bootstrapping key (non-FFT)
*/
/*
template<typename Torus32>
LweBootstrappingKey * MoreTfheIO::new_Bootstrapping_Key_fromFile(FILE* f, const LweParams* init_lwe_params, const TGswParams* out_tgsw_params)
{
  CIstream boot_key_string = to_Istream(f);
  LweBootstrappingKey * boot_key = IOFunctions::read_new_lweBootstrappingKey(boot_key_string, init_lwe_params, out_tgsw_params);
  return boot_key;
}

template<typename Torus32>
void MoreTfheIO::write_Bootstrapping_Key_toFile(LweBootstrappingKey * boot_key, FILE * f)
{
  COstream boot_key_string = to_Ostream(f);
  IOFunctions::write_lweBootstrappingKey(boot_key_string, boot_key, false, false);  // Don't write the parameters as well
}

/*
* Read and write a single TLWE sample
*/
/*
template<typename Torus32>
void MoreTfheIO::read_TLWE_sample_fromFile(FILE* f, TLweSample * tlweSample, const TLweParams * tlwe_params)
{
  CIstream tlwe_sample_string = to_Istream(f);
  IOFunctions::read_tLweSample(tlwe_sample_string, tlweSample, tlwe_params);
}

template<typename Torus32>
void MoreTfheIO::write_TLWE_sample_toFile(TLweSample * tlweSample, const TLweParams * tlwe_params, FILE* f)
{
  COstream tlwe_sample_string = to_Ostream(f);
  IOFunctions::write_tLweSample(tlwe_sample_string, tlweSample, tlwe_params);
}

/*
* Read and write a single LWE sample
*/
/*
template<typename Torus32>
LweSample * MoreTfheIO::new_LWE_sample_fromFile(FILE* f, const LweParams * lwe_params)
{
  LweSample * lweSample = new_LweSample(lwe_params);
  CIstream lweSample_string = to_Istream(f);
  IOFunctions::read_lweSample(lweSample_string, lweSample, lwe_params);
  return lweSample;
}

template<typename Torus32>
void MoreTfheIO::write_LWE_sample_toFile(LweSample * lweSample, const LweParams * lwe_params, FILE* f)
{
  COstream lweSample_string = to_Ostream(f);
  IOFunctions::write_lweSample(lweSample_string, lweSample, lwe_params);
}

/*
* Read and write an array of LWE samples
*/

//template<typename Torus32>
LweSample * MoreTfheIO::new_LWE_array_fromFile(FILE* f, const LweParams * lwe_params, int array_size)
{
  LweSample * lweSample = new_LweSample_array(array_size, lwe_params);
  CIstream temp = to_Istream(f);

  const int n = lwe_params->n;

  for (size_t i = 0; i < array_size; i++)
  {
    temp.fread((lweSample+i)->a, sizeof(Torus32)*n);
    temp.fread(&(lweSample+i)->b, sizeof(Torus32));
  }

  return lweSample;
}

//template<typename Torus32>
void MoreTfheIO::write_LWE_array_toFile(LweSample * lweSample, int array_size, const LweParams * lwe_params, FILE* f)
{
  const int n = lwe_params->n;

  COstream temp = to_Ostream(f);

  for (size_t i = 0; i < array_size; i++)
  {
    temp.fwrite((lweSample+i)->a, sizeof(Torus32)*n);
    temp.fwrite(&(lweSample+i)->b, sizeof(Torus32));
  }
}

/*
* Read and write a single TGSW sample
*/
/*
template<typename Torus32>
void MoreTfheIO::read_TGSW_sample_fromFile(FILE* f, TGswSample * tgswSample, const TGswParams * tgsw_params)
{
  CIstream tgswSample_string = to_Istream(f);
  IOFunctions::read_tGswSample(tgswSample_string, tgswSample, tgsw_params);
}

template<typename Torus32>
void MoreTfheIO::write_TGSW_sample_toFile(TGswSample * tgswSample, const TGswParams * tgsw_params, FILE* f)
{
  COstream tgswSample_string = to_Ostream(f);
  IOFunctions::write_tGswSample(tgswSample_string, tgswSample, tgsw_params);
}

/*
* Read and write the key-switching key
*/

// // Write the key-switching key
// // The file should have been opened as such: FILE * f = fopen("<path_to_file>", "w");
// template<typename Torus32>
// void MoreTfheIO::writeKeySwitchKey(const TLweKeySwitchKey * key_switch_key, FILE *f)
// {
//   COstream keyswitch_key_string = to_Ostream(f);
//   if (f == NULL)
//   {
//     std::cout << "ERROR --- Problem opening file for key-switching key writing" << '\n';
//     return;
//   }
//   key_switch_key->TLweKeySwitchKey::write(keyswitch_key_string);
// }
//
// template<typename Torus32>
// TLweKeySwitchKey * MoreTfheIO::readKeySwitchKey(FILE * f, TLweParams * init_tlwe_params)
// {
//   CIstream keyswitch_key_string = to_Istream(f);
//   if (f == NULL)
//   {
//     std::cout << "ERROR --- Problem opening file ../data/samples/keyswitch_key" << '\n';
//     // return;
//   }
//   TLweKeySwitchKey * key_switch_key = TLweKeySwitchKey::read_new_from_file(file, init_tlwe_params);
//   return key_switch_key;
// //   fclose(file);
// // }
