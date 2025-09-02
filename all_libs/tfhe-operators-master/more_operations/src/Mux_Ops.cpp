#include "mux_ops.h"
#include "private-bootstrap.h"
#include "scaled_operators.h"
#include "scaled_tfhe.h"


// void MuxOperations::CMux_NS_LVL1(TLweSample * stlweOut, LweSample * samp1, LweSample * samp2,LweSample * sind1, LweSample * sind2,  TLweKeySwitchKey * key_switch_21_array, int gamma, TLweParams * tlwe_params_1,const LweParams * lwe_params_1, LweBootstrappingKeyFFT * boot_key LweKey * lwe_key_1, TLweKey * tlwe_key_1)
// {

// int N_1 = lwe_params_1->n;
// double doubleRot = (double) 1/8;
// double doubleCorr = (double)(-1)/(4*N_1);

// int array_len = 4;
// LweSample * slweTestvect = new_LweSample_array(gamma, lwe_params_1);
// TLweSample * stlweTestvect = new TLweSample(tlwe_params_1);

// lweCopy(slweTestvect,samp1,lwe_params_1);
// lweCopy(slweTestvect+1,samp2,lwe_params_1);
// lweCopy(slweTestvect+2,sind1,lwe_params_1);
// lweCopy(slweTestvect+3,sind2,lwe_params_1);

// /*lweKeySwitch(slweTestvect, key_switch_12, samp1);
// lweKeySwitch(slweTestvect+1, key_switch_12, samp2);
// lweKeySwitch(slweTestvect+2, key_switch_12, sind1);
// lweKeySwitch(slweTestvect+3, key_switch_12, sind2);
// */

// /* ########### use this if you want only 1 KS key to be used  #########
// */

// //STLweFunctions::KeySwitch_Id_Ind2(stlweTestvect, key_switch_21, slweTestvect, gamma,array_len);

// /* ############# use this if you want the freedom of using a function that allows 4 KS keys (2 for distance and 2 for indices) ####################
// */

// //STLweFunctions::KeySwitch_Id_Ind3(stlweTestvect, key_switch_21_1,key_switch_21_2,key_switch_21_3,key_switch_21_4, slweTestvect, gamma,array_len);

// /* ################## use this if you want the freedom of using a function that allows a variable number of KS keys. ####################
// */

//   STLweFunctions::KeySwitch_Id_Ind4(stlweTestvect, key_switch_21_array, slweTestvect, gamma,array_len);

// double scaleIn3 = (double) 16;

// lweCopy(slweDist1, slweTestvect,lwe_params_2);
// lweCopy(slweDist2, slweTestvect+1,lwe_params_2);

// LweSample * slweDiff = new LweSample(lwe_params_1);

// int scaleIn2 = slwe2Dist1->scale;

// //lweNoiselessTrivial(slweRot, doubleSignx2, 1, lwe_params_1);
// lweCopy(slweDiff, samp1, lwe_params_1);    // d_1
// lweSubTo(slweDiff, samp2, lwe_params_1);   // d_1 - d_2


// // The rotation of 1/8
// LweSample * slweDiffRot = new LweSample(lwe_params_1);
// lweNoiselessTrivial(slweDiffRot, doubleRot, 1, lwe_params_1);
// lweAddTo(slweDiff, slweDiffRot, lwe_params_1);


// //The roation of -1/4N
// LweSample * slweDiffRot2 = new LweSample(lwe_params_1);
// lweNoiselessTrivial(slweDiffRot2, doubleCorr, 1, lwe_params_1);
// lweAddTo(slweDiff, slweDiffRot2, lwe_params_1);


// PBootFFT::boot_wo_extract(stlweOut->tlwe_sample,boot_key,stlweTestvect->tlwe_sample,slweSign->lwe_sample);
// stlweOut->poly_degree = lwe_params_1->n;

// delete_TLweSample(stlweTestvect);
// delete_LweSample(slwe2Dist1);
// delete_LweSample(slwe2Dist2);
// delete_LweSample(slweDiff2);
// delete_LweSample(slweRot);
// delete_LweSample(slweDiffRot);
// delete_LweSample_array(gamma,slweTestvect);

// }


void MuxOperations::CMux_NS(TLweSample * stlweOut, LweSample * samp1, LweSample * samp2,LweSample * sind1, LweSample * sind2,  TLweKeySwitchKey * key_switch_21_array, int gamma, TLweParams * tlwe_params_1,const LweParams * lwe_params_1, const LweParams * lwe_params_2, LweKeySwitchKey * key_switch_12, LweBootstrappingKeyFFT * boot_key_12, LweBootstrappingKeyFFT * boot_key_21, LweKey * lwe_key_1, LweKey * lwe_key_2,TLweKey * tlwe_key_1)
{

int N_1 = lwe_params_1->n;
double doubleSign = (double) gamma/(16*N_1);
double doubleSignx2 = doubleSign*2;
double doubleRot = (double) 1/8;
double doubleCorr = (double)(-1)/(4*N_1);

int array_len = 4;
LweSample * slweTestvect = new_LweSample_array(gamma, lwe_params_2);
TLweSample * stlweTestvect = new TLweSample(tlwe_params_1);

lweKeySwitch(slweTestvect, key_switch_12, samp1);
lweKeySwitch(slweTestvect+1, key_switch_12, samp2);
lweKeySwitch(slweTestvect+2, key_switch_12, sind1);
lweKeySwitch(slweTestvect+3, key_switch_12, sind2);


/* ########### use this if you want only 1 KS key to be used  #########
*/

//STLweFunctions::KeySwitch_Id_Ind2(stlweTestvect, key_switch_21, slweTestvect, gamma,array_len);

/* ############# use this if you want the freedom of using a function that allows 4 KS keys (2 for distance and 2 for indices) ####################
*/

//STLweFunctions::KeySwitch_Id_Ind3(stlweTestvect, key_switch_21_1,key_switch_21_2,key_switch_21_3,key_switch_21_4, slweTestvect, gamma,array_len);

/* ################## use this if you want the freedom of using a function that allows a variable number of KS keys. ####################
*/

  TLweFunctions::KeySwitch_Id_Ind4(stlweTestvect, key_switch_21_array, slweTestvect, gamma,array_len);

/**
* Need to use the keyswitched distances to compute the difference
**/
LweSample * slwe2Dist1 = new LweSample(lwe_params_2);
LweSample * slwe2Dist2 = new LweSample(lwe_params_2);

double scaleIn3 = (double) 16;

lweCopy(slwe2Dist1, slweTestvect,lwe_params_2);
lweCopy(slwe2Dist2, slweTestvect+1,lwe_params_2);

LweSample * slweDiff2 = new LweSample(lwe_params_2);
LweSample * slweRot = new LweSample(lwe_params_2);
//int scaleIn2 = slwe2Dist1->scale;

lweNoiselessTrivial(slweRot, doubleSignx2, lwe_params_2);
lweCopy(slweDiff2, slwe2Dist1, lwe_params_2);    // d_1
lweSubTo(slweDiff2, slwe2Dist2, lwe_params_2);   // d_1 - d_2

LweSample * slweSign = new LweSample(lwe_params_2);
//slweSign->scale = scaleIn3;
slweSign = slweDiff2;

// The rotation of 1/8
LweSample * slweDiffRot = new LweSample(lwe_params_2);
lweNoiselessTrivial(slweDiffRot, doubleRot, lwe_params_2);
lweAddTo(slweDiff2, slweDiffRot, lwe_params_2);


//The roation of -1/4N
LweSample * slweDiffRot2 = new LweSample(lwe_params_2);
lweNoiselessTrivial(slweDiffRot2, doubleCorr, lwe_params_2);
lweAddTo(slweSign, slweDiffRot2, lwe_params_2);


PBootFFT::boot_wo_extract(stlweOut,boot_key_21,stlweTestvect,slweSign);
//stlweOut->poly_degree = lwe_params_1->n;

delete_TLweSample(stlweTestvect);
delete_LweSample(slwe2Dist1);
delete_LweSample(slwe2Dist2);
delete_LweSample(slweDiff2);
delete_LweSample(slweRot);
delete_LweSample(slweDiffRot);
delete_LweSample_array(gamma,slweTestvect);

}


void MuxOperations::CMux_AS(TLweSample * stlweOut, LweSample * samp1, LweSample * samp2,LweSample * sind1, LweSample * sind2, TLweKeySwitchKey * key_switch_21_array, int gamma, TLweParams * tlwe_params_1,const LweParams * lwe_params_1,const  LweParams * lwe_params_2, LweKeySwitchKey * key_switch_12, LweBootstrappingKeyFFT * boot_key_12, LweBootstrappingKeyFFT * boot_key_21, LweKey * lwe_key_1, LweKey * lwe_key_2,TLweKey * tlwe_key_1)
{

int N_1 = lwe_params_1->n;
double doubleSign = (double) gamma/(16*N_1);
double doubleSignx2 = doubleSign*2;
double doubleCorr = (double)(-1)/(4*N_1);

int array_len = 4;
LweSample * slweTestvect = new_LweSample_array(gamma, lwe_params_2);
TLweSample * stlweTestvect = new TLweSample(tlwe_params_1);
lweKeySwitch(slweTestvect, key_switch_12, samp1);
lweKeySwitch(slweTestvect+1, key_switch_12, samp2);
lweKeySwitch(slweTestvect+2, key_switch_12, sind1);
lweKeySwitch(slweTestvect+3, key_switch_12, sind2);

// ########### use this if you want only 1 KS key to be used  #########

//STLweFunctions::KeySwitch_Id_Ind2(stlweTestvect, key_switch_21, slweTestvect, gamma,array_len);

// ############# use this if you want the freedom of using a function that allows 4 KS keys (2 for distance and 2 for indices) ####################

//STLweFunctions::KeySwitch_Id_Ind3(stlweTestvect, key_switch_21_1,key_switch_21_2,key_switch_21_3,key_switch_21_4, slweTestvect, gamma,array_len);

// ################## use this if you want the freedom of using a function that allows a variable number of KS keys. ####################3

TLweFunctions::KeySwitch_Id_Ind4(stlweTestvect, key_switch_21_array, slweTestvect, gamma,array_len);

LweSample * slweDiff = new LweSample(lwe_params_1);
lweCopy(slweDiff, samp1, lwe_params_1);    // d_1
lweSubTo(slweDiff, samp2, lwe_params_1);   // d_1 - d_2
LweSample * slweSign = new LweSample(lwe_params_2);

FTOperator::sign(slweSign, boot_key_12, slweDiff, doubleSign);

LweSample * slweDiffRot = new LweSample(lwe_params_2);
lweNoiselessTrivial(slweDiffRot, doubleSignx2, lwe_params_2);
lweAddTo(slweSign, slweDiffRot, lwe_params_2);
LweSample * slweDiffRot2 = new LweSample(lwe_params_2);
lweNoiselessTrivial(slweDiffRot2, doubleCorr, lwe_params_2);
lweAddTo(slweSign, slweDiffRot2, lwe_params_2);

PBootFFT::boot_wo_extract(stlweOut,boot_key_21,stlweTestvect,slweSign);

//stlweOut->poly_degree = lwe_params_1->n;

//delete values
// free(doubleSign);
// free(doubleSignx2);
// free(doubleCorr);
delete_TLweSample(stlweTestvect);
delete_LweSample(slweDiff);
delete_LweSample(slweSign);
delete_LweSample(slweDiffRot);
//delete_LweSample(slweCorr);
delete_LweSample_array(gamma,slweTestvect);
}
