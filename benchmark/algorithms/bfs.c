// #include <stdio.h>

#include <standard_libraries.h>
#include <uart_pynq.h>

void bfs(int v, int n, int adj[][n], int *visited, int *queue, int *front, int *rear)
{
    for (int i = 1; i <= n; i++) {
        if (adj[v][i] && !visited[i]) {
            *rear += 1;
            // printf("rear: %d, front:%d, n:%d\n", *rear, *front, n);
            queue[*rear] = i;
        }
    }
    if (*front <= *rear) {
        // printf("front: %d\n", *front);
        visited[queue[*front]] = 1;
        bfs(queue[(*front)++], n, adj, visited, queue, front, rear);
    }
}

void bfs_simple_call() {
    int visited[9] = {0};
    int queue[9] = {0};
    int adj[][9] =  {{0, 1, 1, 0, 0, 0, 0, 0, 0},
                     {1, 0, 0, 1, 1, 0, 0, 0, 0},
                     {1, 0, 0, 0, 0, 1, 1, 0, 0},
                     {0, 1, 0, 0, 0, 0, 0, 1, 0},
                     {0, 1, 0, 0, 0, 0, 0, 1, 0},
                     {0, 0, 1, 0, 0, 0, 0, 0, 1},
                     {0, 0, 1, 0, 0, 0, 0, 0, 1},
                     {0, 0, 0, 1, 1, 0, 0, 0, 1},
                     {0, 0, 0, 0, 0, 1, 1, 1, 0}};
    int front = -1, rear = -1;
    bfs(2, 9, adj, visited, queue, &front, &rear);
}
 
// int main()
// {
//     // int v;
//     // printf("Enter the number of vertices: ");
//     const int n = 9;
//     // scanf("%d", &n);
//     // for (int i = 1; i <= n; i++)
//     // {
//     //     queue[i] = 0;
//     //     visited[i] = 0;
//     // }
//     int visited[9] = {0};
//     int queue[9] = {0};
//     int adj[][9] =  {{0, 1, 1, 0, 0, 0, 0, 0, 0},
//                      {1, 0, 0, 1, 1, 0, 0, 0, 0},
//                      {1, 0, 0, 0, 0, 1, 1, 0, 0},
//                      {0, 1, 0, 0, 0, 0, 0, 1, 0},
//                      {0, 1, 0, 0, 0, 0, 0, 1, 0},
//                      {0, 0, 1, 0, 0, 0, 0, 0, 1},
//                      {0, 0, 1, 0, 0, 0, 0, 0, 1},
//                      {0, 0, 0, 1, 1, 0, 0, 0, 1},
//                      {0, 0, 0, 0, 0, 1, 1, 1, 0}};
//     // printf("Enter graph data in matrix form:    \n");
//     // for (int i = 1; i <= n; i++)
//     //     for (int j = 1; j <= n; j++)
//     //         scanf("%d", &adj[i][j]);
//     // printf("Enter the starting vertex: ");
//     // scanf("%d", &v);
//     int front = -1, rear = -1;
//     bfs(2, n, adj, visited, queue, &front, &rear);
//     uart_pynq_printf("The node which are reachable are:    \n");
//     for (int i = 1; i <= n; i++)
//         if (visited[i])
//             uart_pynq_printf("%d\t", i);
//         else
//             uart_pynq_printf("BFS is not possible. Not all nodes are reachable");
//     return 0;
// }
// 
// 
// // 0, 1, 1, 0, 0, 0, 0, 0, 0,
// // 1, 0, 0, 1, 1, 0, 0, 0, 0,
// // 1, 0, 0, 0, 0, 1, 1, 0, 0,
// // 0, 1, 0, 0, 0, 0, 0, 1, 0,
// // 0, 1, 0, 0, 0, 0, 0, 1, 0,
// // 0, 0, 1, 0, 0, 0, 0, 0, 1,
// // 0, 0, 1, 0, 0, 0, 0, 0, 1,
// // 0, 0, 0, 1, 1, 0, 0, 0, 1,
// // 0, 0, 0, 0, 0, 1, 1, 1, 0,