# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# define INF 100
# define MIN(x, y) (((x) < (y)) ? (x) : (y))

double** Make2DdoubleArray(double arraySizeX, double arraySizeY) {
    double** theArray;
    theArray = (double**) malloc(arraySizeX*sizeof(double*));
    for (int i = 0; i < arraySizeX; i++)
        theArray[i] = (double*) malloc(arraySizeY*sizeof(double));
    return theArray;
} 

double** Floyd_APSP(double** D, double N){
    int i,j,k;

    for(k=0; k<N; k++)
        for(i=0; i<N; i++)
            for(j=0; j<N; j++){
                D[i][j] = MIN(D[i][j], D[i][k]+D[k][j]);
            }
    return D;

}


int main(){
	//build the graph of lecture11 slide23
	int N; //number of vertex;
	int i, j, k;

	N=4;

    double** D = Make2DdoubleArray(N, N);	
	for (i=0; i<N; i++){
    	for (j=0; j<N; j++){
        D[i][j]=INF;
    	}
    }

    for (i=0; i<N; i++){
    	D[i][i] = 0;
    }

    //build the edges
    D[0][2]=3;
    D[1][0]=2;
    D[2][1]=7;
    D[2][3]=1;
    D[3][0]=6;

    D = Floyd_APSP(D, N);


    //check
	for (i=0; i<N; i++){
    	for (j=0; j<N; j++){
        printf("%.0f ", D[i][j]);
    	}
    	printf("\n");
    }


}