# include <omp.h>
# include <iostream>
# include <cmath>
# include <cassert>
# include <chrono>

int main(int argc, char* argv[])
{

unsigned long int N=pow(2,20); //problem size
unsigned long int i; // for iteration
double s=0.0; // hold the sum
double* v; // dynamically allocated array  

v = new double [N];
assert (v != NULL); // in case of inadequate memory 

// initialize to 1.0
for (i=0; i<N; i++){
    v[i]=1.0; }

using namespace std::chrono;
high_resolution_clock::time_point t1 = high_resolution_clock::now();

// summing up the vector
#pragma omp parallel for default(none) \
    shared(N,v) private(i) reduction(+:s)
for (i=0; i<N; i++){
        s += v[i];
} //-- End of parallel region --
high_resolution_clock::time_point t2 = high_resolution_clock::now();

duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

using namespace std;
cout << "result = " << s << "\n";
cout << "time use = " << time_span.count()*1000.0 << "ms \n";

return 0;

} //-- End of the main program --
