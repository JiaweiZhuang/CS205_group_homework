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
	int i, j, k;
	int** D;
    	if (argc != 3) {
            printf("Usage: %s N filenm\n",argv[0]);
            return 1;
        }
        N = atoi(argv[1]);
        char* filenm = argv[2];
        D = graph_from_edge_list(filenm,N);

    	D = Floyd_APSP(D, N);


    //check
	for (i=0; i<N; i++){
    	for (j=0; j<N; j++){
        printf("%.0f ", D[i][j]);
    	}
    	printf("\n");
    }


}
