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

//A: n*p, B: p*m
double** matrixMultiply(double** A, double** B, int n, int m, int p){
	int i, j, k;
	double** C = Make2DdoubleArray(n,m);
	for (i=0; i<n; i++){
		for (j=0; j<m; j++){
	    C[i][j]=INF;
		}
	}

    for(i=0; i<n; i++){
	    for(j=0; j<m; j++){
	        for(k=0; k<p; k++){
	            C[i][j]= MIN(A[i][k] + B[k][j], C[i][j]);

	        }
	    }
    }

    return C;

}

double** matrixAdd(double** A, double** B, double** C, int n, int m){
	int i, j, k;
	for (i=0; i<n; i++){
		for (j=0; j<m; j++){
    		C[i][j]=MIN(A[i][j], B[i][j]);
		}
    }
    return C;
}


double** DC_APSP(double** A, int n){
	int i,j;
	if(n==1) return A;

	//partition
	double** A11 = Make2DdoubleArray(n/2, n/2);
	double** A12 = Make2DdoubleArray(n/2, n/2);
	double** A21 = Make2DdoubleArray(n/2, n/2);
	double** A22 = Make2DdoubleArray(n/2, n/2);

	for(i=0; i<n/2; i++)
		for(j=0; j<n/2; j++){
			A11[i][j] = A[i][j];
			A12[i][j] = A[i][j+n/2];
			A21[i][j] = A[i+n/2][j];
			A22[i][j] = A[i+n/2][j+n/2];
		}


	A11 = DC_APSP(A11, n/2);
	A12 = matrixMultiply(A11, A12, n/2, n/2, n/2);
	A21 = matrixMultiply(A21, A11, n/2, n/2, n/2);
	A22 = matrixAdd(A22, matrixMultiply(A21, A12, n/2, n/2, n/2), A22, n/2, n/2);
	A22 = DC_APSP(A22, n/2);

	A21 = matrixMultiply(A22, A21, n/2, n/2, n/2);
	A12 = matrixMultiply(A12, A22, n/2, n/2, n/2);
	A11 = matrixAdd(A11, matrixMultiply(A12, A21, n/2, n/2, n/2), A11, n/2, n/2);


	//code for check
	// prdoublef("A A21\n");
	// for(i=0; i<n/2; i++){
	// for(j=0; j<n/2; j++){
	// 	prdoublef("%d ", A11[i][j]);

	// }
	// prdoublef("\n");
	// }

//copy back
for(i=0; i<n/2; i++)
	for(j=0; j<n/2; j++){
		A[i][j] 		= 	A11[i][j];
		A[i][j+n/2]		=	A12[i][j];
		A[i+n/2][j] 	=	A21[i][j];
		A[i+n/2][j+n/2]	=	A22[i][j];
	}

	return A;

}

int** graph_from_edge_list (char* filenm,int N) {
        int max_edges = 1000000;
        int nedges, nr, t, h;
        int** A = Make2DIntArray(N, N);
        FILE *ptr_file;
        int i,j;
        for (i=0; i<N; i++){
                for (j=0; j<N; j++){
                A[i][j] = Inf;
                }
        }
	for (i=0; i<N; i++){
		A[i][i] = 0;
	}

        ptr_file =fopen(filenm,"r");
                if (!ptr_file)
                        return 0;
        nedges = 0;
        nr = fscanf(ptr_file, "%i %i", &t,&h);
        while (nr == 2) {
        if (nedges >= max_edges) {
        printf("Limit of %d edges exceeded.\n",max_edges);
        exit(1);
        }
        A[t][h] = 1;
        nedges += 1;
        nr = fscanf(ptr_file, "%i %i",&t,&h);
        }
        return A;
}

int main(int argc,char *argv[]){

	//build the graph of lecture11 slide23
	int N; //number of vertex;
	int i, j;
	int** D;
	if (argc != 3) {
            printf("Usage: %s N filenm\n",argv[0]);
            return 1;
        }
        N = atoi(argv[1]);
        char* filenm = argv[2];
        D = graph_from_edge_list(filenm,N);
	
	D = DC_APSP(D, N);

	//check
	for (i=0; i<N; i++){
    	for (j=0; j<N; j++){
        printf("%.0f ", D[i][j]);
    	}
    	printf("\n");
    }

}
