/*
    * C Program to find the shortest path between two vertices in a graph
    * using the Floyd-Warshall algorithm
    */
    
#include <stdio.h>
#include <stdlib.h>
    
// void floydWarshall(int n, int graph[][n]) {
void floydWarshall(int n, int graph[][n]) {
    int i, j, k;
    for (k = 0; k < n; k++) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                if (graph[i][j] > graph[i][k] + graph[k][j])
                    graph[i][j] = graph[i][k] + graph[k][j];
            }
        }
    }
}

void floydWarshall_simple_call() {
    int n = 4;
    int graph[][4] = {
        {0, 12, 45, 2},
        {1, 0, 45, 32},
        {77, 43, 0, 2},
        {42, 3, 88, 0}
    };
    floydWarshall(n, graph);
}

// int main(void) {
//     int i, j;
//     // printf("Enter the number of vertices: ");
//     // scanf("%d", &n);
//     // int **graph = (int **)malloc((long unsigned) n * sizeof(int *));
//     // for (i = 0; i < n; i++) {
//     //     graph[i] = (int *)malloc((long unsigned) n * sizeof(int));
//     // }
//     // for (i = 0; i < n; i++) {
//     //     for (j = 0; j < n; j++) {
//     //         if (i == j)
//     //             graph[i][j] = 0;
//     //         else
//     //             graph[i][j] = 100;
//     //     }
//     // }
//     int n = 4;
//     int graph[][4] = {
//         {0, 12, 45, 2},
//         {1, 0, 45, 32},
//         {77, 43, 0, 2},
//         {42, 3, 88, 0}
//     };
//     // printf("Enter the edges: \n");
//     // for (i = 0; i < n; i++) {
//     //     for (j = 0; j < n; j++) {
//     //         printf("[%d][%d]: ", i, j);
//     //         scanf("%d", &graph[i][j]);
//     //     }
//     // }
//     printf("The original graph is:\n");
//     for (i = 0; i < n; i++) {
//         for (j = 0; j < n; j++) {
//             printf("%d ", graph[i][j]);
//         }
//         printf("\n");
//     }
//     floydWarshall(n, graph);
//     printf("The shortest path matrix is:\n");
//     for (i = 0; i < n; i++) {
//         for (j = 0; j < n; j++) {
//             printf("%d ", graph[i][j]);
//         }
//         printf("\n");
//     }
//     return 0;
// }