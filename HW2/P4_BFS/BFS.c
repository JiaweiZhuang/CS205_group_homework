# include <stdlib.h>
# include <stdio.h>
# include <math.h>

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

int** graph_from_edge_list (char* filenm,int N) {
        int max_edges = 1000000;
        int nedges, nr, t, h;
        int** A = Make2DIntArray(N, N);
        FILE *ptr_file;
        int i,j;
        for (i=0; i<N; i++){
                for (j=0; j<N; j++){
                A[i][j]=0;
                }
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

	//build the graph of lecture11 slide20
	int N; //number of vertex;
	int i, j, idx, flag;
	int** A;
	int nParentNode;  //number of active parent nodes
	int distance;
	if (argc != 3) {
            printf("Usage: %s N filenm\n",argv[0]);
            return 1;
        }
	N = atoi(argv[1]);
	char* filenm = argv[2];
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
	
	nParentNode = 1;
	distance = 0;	
	int* idxParentNode = Make1DIntArray(N);	//index of active parent nodes
        for (i=0; i<N; i++){
        idxParentNode[i]=0;
        }

	int* record = Make1DIntArray(N);
        record[0]=1; //1 is visited nodes
        for (i=1; i<N; i++){
        record[i]=0;
        }
//MxV function   for reference
// for (i=0; i<N; i++){
//     	for (j=0; j<N; j++){
//         y[j] += A[i][j] & x[j];
//     }
// }

while(1){
	distance = distance + 1;  //furtherest levels

	//Parallel!!!
    #pragma acc data copyin(N,A[:N][:N],x[:N],idxParentNode[:N],nParentNode) copy(y[:N])
    {
    #pragma acc parallel private(j,i,idx) 
    {
        #pragma acc loop 
    	for(j=0; j<N; j++){
            
    	    for (i=0; i<nParentNode; i++){
    		    idx = idxParentNode[i];
    			y[j] = y[j] || (A[idx][j] && x[idx]);
    		}
    	}
    }
    }
	
	//reset parent nodes and check if converge
	flag = 1; //converge flag
	nParentNode = 0;

        for(j=0; j<N; j++)
        {
                if(y[j]&&!record[j]){
			idxParentNode[nParentNode] = j;   //recording index of new parent nodes
			nParentNode = nParentNode + 1; 
                        record[j] = 1;
                        flag = 0;
                }
        }

	//print y vector to check
	//for(j=0; j<N; j++)
	//{
	//	printf("%d ", y[j]);
	//}
	printf("distance %d \n", distance);
	printf("nParentNode %d \n", nParentNode);

	if(flag==1){break;}

	//copy y to x
	for(j=0; j<N; j++)
	{
		x[j] = y[j];
	}

}
}
