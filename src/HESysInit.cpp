#include "tfhe.h"
#include "params.h"
#include "keysHEComp.h"
#include <iostream>
#include "seal/seal.h"
#include <ctime>
using namespace std;
using namespace seal;
 
int main()
{

clock_t start = clock();    
paramsGen::generateSealParams();
paramsGen::generateTFHEParams();
HEKeyGen::generateHEkeys();

clock_t end = clock();
double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;

cout << "********************************************" << endl;
cout << "* Successfully Generated System Parameters *" << endl;
cout << "********************************************" << endl;
cout << "Time for System Parameter generation: " << elapsed_secs << " seconds" << endl;
return 0;
}