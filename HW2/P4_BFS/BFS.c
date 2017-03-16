# include <stdlib.h>
# include <stdio.h>
# include <math.h>

int graph_from_edge_list (char filenm,int N) {
        int max_edges = 1000000;
        int nedges, nr, t, h;
        int** A = Make2DIntArray(N, N);
        FILE *ptr_file;
        for (i=0; i<N; i++){
                for (j=0; j<N; j++){
                A[i][j]=0;
                }
        }

        ptr_file =fopen(filenm,"r");
                if (!ptr_file)
                        return 1;
        nedges = 0;
        nr = fscanf(ptr_file, "%i %i", &t,&h);
        while (nr == 2) {
        if (nedges >= max_edges) {
        printf("Limit of %d edges exceeded.\n",max_edges);
        exit(1);
        }
        A[t-1][h-1] = 1;
        nedges += 1;
        nr = fscanf(ptr_file, "%i %i",&t,&h);
        }
        return A;
}

int** Make2DIntArray(int arraySizeX, int arraySizeY) {
    int** theArray;
    theArray = (int**) malloc(arraySizeX*sizeof(int*));
    for (int i = 0; i < arraySizeX; i++)
        theArray[i] = (int*) malloc(arraySizeY*sizeof(int));
    return theArray;
} 

int* Make1DIntArray(int arraySize){
	int* theArray;
	theArray = (int*) malloc(arraySize*sizeof(int));
    return theArray;
}

int main(int argc,char *argv[]){

	//build the graph of lecture11 slide20
	int N; //number of vertex;
	int i, j, temp;
	int** A;
	if (argc != ) {
            printf("Usage: %s N filenm\n",argv[0]);
            return 1;
        }
	N = atoi(argv[1]);
	filenm = argv[2];
        A = graph_from_edge_list(filenm,N);   //Get graph matrix
	
	int* x = Make1DIntArray(N);
	x[0]=1; //1 is the root node, start our BFS from here.
	for (i=1; i<N; i++){
    	x[i]=0;
	}


	int* y = Make1DIntArray(N);
	for (i=0; i<N; i++){
    	y[i]=0;
	}

//MxV function   for reference
// for (i=0; i<N; i++){
//     	for (j=0; j<N; j++){
//         y[j] += A[i][j] & x[j];
//     }
// }

while(1){

	//parallel here?
	for (i=0; i<N; i++){
		if(!x[i]){break;}
		for(j=0; j<N; j++){
			y[j] |= A[i][j] & x[i];
		}
	}

	//print y vector to check
	for(j=0; j<N; j++)
	{
		printf("%d ", y[j]);
	}
	printf("\n");

	//check if there's more node to visit
	temp=1;
	for(j=0; j<N; j++)
	{
		temp &= y[j];
	}

	if(temp==1){break;}

	//copy y to x
	for(j=0; j<N; j++)
	{
		x[j] = y[j];
	}

}


}
