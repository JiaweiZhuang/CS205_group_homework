# include "../shared/timing.h"
# include <stdlib.h>
# include <stdio.h>
# include <math.h>

// for dynamically allocating 2D array in pure-C environment
double** Make2DDoubleArray(int arraySizeX, int arraySizeY) {
    double** theArray;
    theArray = (double**) malloc(arraySizeX*sizeof(double*));
    for (int i = 0; i < arraySizeX; i++)
        theArray[i] = (double*) malloc(arraySizeY*sizeof(double));
    return theArray;
} 

int main() {

    int N=pow(2,10); //problem size
    int i,j,k; // interative

    // Initialize arrays
    // Note:
    // 1) For transfering data from host to device, it's more 
    // convenient to use pure-C arrays than C++ objects.
    // 2) Can't use A[N][N] even if N is a pure number. This will
    // cause stack overflow for large N.
    // 3) OpenACC uses the same variable for both host and devide.
    double** A = Make2DDoubleArray(N, N);
    double** B = Make2DDoubleArray(N, N);
    double** C = Make2DDoubleArray(N, N);

    // Initial condition
    for (i=0; i<N; i++){
    for (j=0; j<N; j++){
        A[i][j]=1.0;
        B[i][j]=1.0;
        C[i][j]=0.0;
    }
    }

    // timing MxM
    double iStart, iElaps;
    iStart = seconds();

    #pragma acc parallel loop collapse(2)
    for(i=0; i<N; i++){
        for(j=0; j<N; j++){

            for(k=0; k<N; k++){
                C[i][j]+=A[i][k] * B[k][j];

            }
        }
    }

    iElaps = seconds() - iStart;

    // print the results
    printf("First element =  %f \n", C[0][0]);
    printf("Time elapse = %f ms \n", iElaps*1000.0);

    return 0;

}



