#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#define INF 100 // weight is less than 100 in our generated RMAT graph
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

struct timeval tStart, tStop, tElapsed;

void StartTimer()
{
  gettimeofday(&tStart, NULL);
}

double GetTimer()
{
  gettimeofday(&tStop, NULL);
  timersub(&tStop, &tStart, &tElapsed);
  return tElapsed.tv_sec * 1000.0 + tElapsed.tv_usec / 1000.0;
}

int** Make2DIntArray(int arraySizeX, int arraySizeY) {
  int **theArray;

  theArray = (int **)malloc(arraySizeX * sizeof(int *));

  for (int i = 0; i < arraySizeX; i++) theArray[i] = (int *)malloc(
      arraySizeY * sizeof(int));
  return theArray;
}

// A: n*p, B: p*m
int** matrixMultiply(int **A, int **B, int n, int m, int p) {
  int   i, j, k;
  int **C = Make2DIntArray(n, m);

  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
      C[i][j] = INF;
    }
  }

  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
      for (k = 0; k < p; k++) {
        C[i][j] = MIN(A[i][k] + B[k][j], C[i][j]);
      }
    }
  }

  return C;
}

int** matrixAdd(int **A, int **B, int **C, int n, int m) {
  int i, j, k;

  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
      C[i][j] = MIN(A[i][j], B[i][j]);
    }
  }
  return C;
}

int** DC_APSP(int **A, int n) {
  int i, j;
  // printf("%d, %d\n",n, A[0][2]);
  if (n == 1) return A;

  // partition
  int **A11 = Make2DIntArray(n / 2, n / 2);
  int **A12 = Make2DIntArray(n / 2, n / 2);
  int **A21 = Make2DIntArray(n / 2, n / 2);
  int **A22 = Make2DIntArray(n / 2, n / 2);

  for (i = 0; i < n / 2; i++)
    for (j = 0; j < n / 2; j++) {
      A11[i][j] = A[i][j];
      A12[i][j] = A[i][j + n / 2];
      A21[i][j] = A[i + n / 2][j];
      A22[i][j] = A[i + n / 2][j + n / 2];
    }


  A11 = DC_APSP(A11, n / 2);
  A12 = matrixMultiply(A11, A12, n / 2, n / 2, n / 2);
  A21 = matrixMultiply(A21, A11, n / 2, n / 2, n / 2);
  A22 = matrixAdd(A22, matrixMultiply(A21,
                                      A12,
                                      n / 2,
                                      n / 2,
                                      n / 2), A22, n / 2, n / 2);
  A22 = DC_APSP(A22, n / 2);

  A21 = matrixMultiply(A22, A21, n / 2, n / 2, n / 2);
  A12 = matrixMultiply(A12, A22, n / 2, n / 2, n / 2);
  A11 = matrixAdd(A11, matrixMultiply(A12,
                                      A21,
                                      n / 2,
                                      n / 2,
                                      n / 2), A11, n / 2, n / 2);

  // copy back
  for (i = 0; i < n / 2; i++) {
    for (j = 0; j < n / 2; j++) {
      A[i][j]                 =       A11[i][j];
      A[i][j + n / 2]         =       A12[i][j];
      A[i + n / 2][j]         =       A21[i][j];
      A[i + n / 2][j + n / 2] =       A22[i][j];
    }
  }
  free(A11);
  free(A12);
  free(A21);
  free(A22);

  return A;
}

int** graph_from_edge_list(char *filenm, int N) {
  int   max_edges = 1000000;
  int   nedges, nr, t, h, d;
  int **A = Make2DIntArray(N, N);
  FILE *ptr_file;
  int   i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      A[i][j] = INF;
    }
  }

  ptr_file = fopen(filenm, "r");

  if (!ptr_file) return 0;

  nedges = 0;
  nr     = fscanf(ptr_file, "%i %i %i", &t, &h, &d);

  while (nr == 3) {
    if (nedges >= max_edges) {
      printf("Limit of %d edges exceeded.\n", max_edges);
      exit(1);
    }
    A[t][h] = d;
    nedges += 1;
    nr      = fscanf(ptr_file, "%i %i", &t, &h, &d);
  }

  for (i = 0; i < N; i++) {
    A[i][i] = 0;
  }
  return A;
}

int main(int argc, char *argv[]) {
  // build the graph of lecture11 slide23
  int   N; // number of vertex;
  int   i, j;
  int **D;
  if (argc == 1) {
    // Test on simple graph
    N = 4;
    int tmp[4][4] =
    { { 0, INF,   3, INF }, { 2, 0, INF, INF }, { INF, 7, 0, 1 },
      { 6, INF, INF,   0 } };
    D = (int **)malloc(4 * sizeof(int *));

    for (int i = 0; i < 4; i++) {
      D[i] = (int *)malloc(4 * sizeof(int));
    }

    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        D[i][j] = tmp[i][j];
        // printf("%d\n", D[i][j]);
      }
    }
    D = DC_APSP(D, N);
  } else if (argc == 3) {
    N = atoi(argv[1]);
    char *filenm = argv[2];
    D = graph_from_edge_list(filenm, N);
    StartTimer();
    D = DC_APSP(D, N);
    double runtime = GetTimer();
    printf("Total time: %f ms\n", runtime);
  } else {
    printf("Usage: %s N filenm\n", argv[0]);
  }

  //check
  // for (i = 0; i < N; i++) {
  //   for (j = 0; j < N; j++) {
  //     printf("%d ", D[i][j]);
  //   }
  //   printf("\n");
  // }
  return 1;
}
