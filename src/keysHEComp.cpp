
#include <stdlib.h>
#include <fstream>
#include "keysHEComp.h"
//TFHE
#include "tfhe.h"

using namespace std;

/**
* Initialize TFHE keys for HE Component
*/

void HEKeyGen::generateHEkeys(){

FILE * tgsw_oparams_file = fopen("../data/System Parameters/params_tgsw_out.txt" , "r");
TGswParams * out_tgsw_params = new_tGswParams_fromFile(tgsw_oparams_file);
fclose(tgsw_oparams_file);
FILE * lwe_oparams_file = fopen("../data/System Parameters/params_lwe_out.txt" , "r");
LweParams * out_lwe_params = new_lweParams_fromFile(lwe_oparams_file);
fclose(lwe_oparams_file);

TGswKey * out1_tgsw_key = new_TGswKey(out_tgsw_params);
tGswKeyGen(out1_tgsw_key);
TLweKey * out1_tlwe_key = &(out1_tgsw_key->tlwe_key);
LweKey * out1_lwe_key = new_LweKey(out_lwe_params);
tLweExtractKey(out1_lwe_key, out1_tlwe_key);

FILE * out1_tgswKey_file = fopen("../data/HEComp/tgsw_out1.txt" , "w"); 
  export_tgswKey_toFile(out1_tgswKey_file,out1_tgsw_key);   
  fclose(out1_tgswKey_file);

FILE * out1_lweKey_file = fopen("../data/HEComp/lwe_out1.txt" , "w"); 
  export_lweKey_toFile(out1_lweKey_file,out1_lwe_key);   
  fclose(out1_lweKey_file);

}