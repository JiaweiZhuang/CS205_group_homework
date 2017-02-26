# include <omp.h>
# include <iostream>
# include <cmath>
# include <cassert>
# include <chrono>

int main(int argc, char* argv[])
{

unsigned long int N=pow(2,10); //problem size
unsigned long int i,j; // for iteration
double* x,* y; // dynamically allocated array  
double** A; // dynamically allocated matrix

x = new double [N];
assert (x != NULL);

y = new double [N];
assert (y != NULL); 

A = new double* [N];
assert (A != NULL); 
for (int i=0; i<N; i++){
    A[i] = new double [N]; 
    assert (A[i] != NULL);
}

// initial condition 
for (i=0; i<N; i++){
    x[i]=1.0;
    y[i]=0.0; 
}

for (i=0; i<N; i++){
    for (j=0; j<N; j++){
        A[i][j]=1.0;
    }   
}

using namespace std::chrono;
high_resolution_clock::time_point t1 = high_resolution_clock::now();

// Matrix times vector 
#pragma omp parallel for default(none) schedule(static) \
    shared(N,A,x,y) private(i,j)
for (i=0; i<N; i++){
    for (j=0; j<N; j++){
        y[i] += A[i][j]*x[j];
    }
} //-- End of parallel region --
high_resolution_clock::time_point t2 = high_resolution_clock::now();

duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

using namespace std;
cout << "first element = " << y[0] << "\n"; //should equal to N
cout << "time use = " << time_span.count()*1000.0 << "ms \n";

return 0;

} //-- End of the main program --
