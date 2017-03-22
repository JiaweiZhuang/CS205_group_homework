#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#define INF 100
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

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

int** Floyd_APSP(int **D, int N) {
  int i, j, k;

  for (k = 0; k < N; k++) {
    printf("k: %d \n", k);

    for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
        D[i][j] = MIN(D[i][j], D[i][k] + D[k][j]);
      }
    }
  }
  return D;
}

int** Floyd_APSP_Parallel(int **D, int **tmp, int N) {
  int i, j, k;

  #pragma acc data copyin(D[:N][:N]) copy(tmp[:N][:N])
  {
    #pragma acc  private(j,i)
    {
      for (k = 0; k < N; k++) {
        // printf("k: %d \n", k);

        #pragma acc parallel loop collapse(2)
        for (i = 0; i < N; i++) {
          for (j = 0; j < N; j++) {
            tmp[i][j] = MIN(D[i][j], D[i][k] + D[k][j]);
          }
        }
        D = tmp;
      }
    }
  }
  return D;
}

int** graph_from_edge_list(char *filenm, int N) {
  int   max_edges = 1000000;
  int   nedges, nr, t, h, d;
  int **A = Make2DIntArray(N, N);
  FILE *ptr_file;
  int   i, j;

  printf("A...");

  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      A[i][j] = INF;
    }
  }
  printf("B...");
  ptr_file = fopen(filenm, "r");

  if (!ptr_file) return 0;

  nedges = 0;
  nr     = fscanf(ptr_file, "%i %i %i", &t, &h, &d);
  printf("Begin reading file... \n");

  while (nr == 3) {
    if (nedges >= max_edges) {
      printf("Limit of %d edges exceeded.\n", max_edges);
      exit(1);
    }
    A[t][h] = d;
    nedges += 1;
    nr      = fscanf(ptr_file, "%i %i %i", &t, &h, &d);
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
    int **tmp2 = Make2DIntArray(N, N);
    D = Floyd_APSP_Parallel(D, tmp2, N);

  } else if (argc == 3) {
    N = atoi(argv[1]);
    char *filenm = argv[2];

    D = graph_from_edge_list(filenm, N);
    int **tmp = Make2DIntArray(N, N);

    StartTimer();
    D = Floyd_APSP_Parallel(D, tmp, N);
    double runtime = GetTimer();
    printf("Total: %f ms\n", runtime);

  } else {
    printf("Usage: %s N filenm\n", argv[0]);
  }

  // check
  // for (i=0; i<N; i++) {
  //         for (j=0; j<N; j++) {
  //                 printf("%d ", D[i][j]);
  //         }
  //         printf("\n");
  // }

  return 1;
}
