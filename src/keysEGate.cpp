#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <map>


// TFHE
#include "tfhe.h"

#include "keysEGate.h"
//#include <ctime>
#include "utils.h"
#include "dataIO.h"
using namespace std;



void EGateKeyGen::generateEGatekeys(){
FILE * lwe_oparams_file = fopen("../data/System Parameters/params_lwe_out.txt" , "r");
LweParams * out_lwe_params = new_lweParams_fromFile(lwe_oparams_file);
fclose(lwe_oparams_file);
FILE * tgsw_oparams_file = fopen("../data/System Parameters/params_tgsw_out.txt" , "r");
TGswParams * out_tgsw_params = new_tGswParams_fromFile(tgsw_oparams_file);
fclose(tgsw_oparams_file);

TGswKey * out2_tgsw_key = new_TGswKey(out_tgsw_params);
tGswKeyGen(out2_tgsw_key);
TLweKey * out2_tlwe_key = &(out2_tgsw_key->tlwe_key);
LweKey * out2_lwe_key = new_LweKey(out_lwe_params);
tLweExtractKey(out2_lwe_key, out2_tlwe_key);

FILE * out2_lweKey_file = fopen("../data/E-Gate/lwe_out2.txt" , "w"); 
  export_lweKey_toFile(out2_lweKey_file,out2_lwe_key);   
  fclose(out2_lweKey_file);


FILE * out1_lweKey_file = fopen("../data/HEComp/lwe_out1.txt" , "r"); 
LweKey * out1_lwe_key= new_lweKey_fromFile(out1_lweKey_file);
fclose(out1_lweKey_file); // Reading the already predefined Lwe keys from file; Predefined while generating keys for HE component.

// Generate the keyswitch key from the Lwe key of the HE component to the Lwe key of the E-Gate and send to E-Gate.
const int switch_t = 4;//8;//7
const int switch_base = 4;//2;//4 
LweKeySwitchKey * ks_key = new_LweKeySwitchKey(out_lwe_params->n, switch_t, switch_base, out_lwe_params);
lweCreateKeySwitchKey(ks_key, out1_lwe_key, out2_lwe_key);

FILE * ks_key_file = fopen("../data/E-Gate/KSK.data" , "w");
export_lweKeySwitchKey_toFile(ks_key_file, ks_key);
fclose(ks_key_file);

cout << "********************************************" << endl;
cout << "*     Successfully Generated E-Gate Keys     *" << endl;
cout << "********************************************" << endl;


}