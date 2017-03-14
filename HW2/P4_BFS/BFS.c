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

int main(){

	//build the graph of lecture11 slide20
	int N; //number of vertex;
	int i, j, temp;

	N=7;


	int* x = Make1DIntArray(N);
	x[0]=1; //1 is the root node, start our BFS from here.
	for (i=1; i<N; i++){
    	x[i]=0;
	}


	int* y = Make1DIntArray(N);
	for (i=0; i<N; i++){
    	y[i]=0;
	}


	int** A = Make2DIntArray(N, N);	
	for (i=0; i<N; i++){
    	for (j=0; j<N; j++){
        A[i][j]=0;
    	}
    }



    //build the edge
    A[0][0]=1;
    A[0][1]=1;
    A[0][3]=1;
    A[1][0]=1;
    A[1][1]=1;
    A[1][4]=1;
    A[1][6]=1;
    A[2][2]=1;
    A[2][5]=1;
    A[3][0]=1;
    A[3][2]=1;
    A[3][3]=1;    
    A[3][6]=1;
    A[4][4]=1;
    A[4][5]=1;
    A[5][2]=1;
    A[5][5]=1;
    A[6][2]=1;
    A[6][3]=1;
    A[6][4]=1;
    A[6][6]=1;


//MxV function   for reference
// for (i=0; i<N; i++){
//     	for (j=0; j<N; j++){
//         y[j] += A[i][j] & x[j];
//     }
// }

while(1){

	//parallel here?
	for (i=0; i<N; i++){
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

	//check if there's more mode to visit
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