
// #include <stdio.h>
// #include <conio.h>

// int a[20][20], reach[20], n;
void dfs(int v, int n, int a[][n+1], int *reach) {
    reach[v] = 1;
    for (int i = 1; i <= n; i++) {
        if (a[v][i] && !reach[i]) {
            // printf("\n %d->%d", v, i);
            dfs(i, n, a, reach);
        }
    }
}



void dfs_simple_call() {
    int reach[9] = {0};
    int adj[][9] =  {{0, 1, 1, 0, 0, 0, 0, 0, 0},
                     {1, 0, 0, 1, 1, 0, 0, 0, 0},
                     {1, 0, 0, 0, 0, 1, 1, 0, 0},
                     {0, 1, 0, 0, 0, 0, 0, 1, 0},
                     {0, 1, 0, 0, 0, 0, 0, 1, 0},
                     {0, 0, 1, 0, 0, 0, 0, 0, 1},
                     {0, 0, 1, 0, 0, 0, 0, 0, 1},
                     {0, 0, 0, 1, 1, 0, 0, 0, 1},
                     {0, 0, 0, 0, 0, 1, 1, 1, 0}};
    dfs(1, 8, adj, reach);

    int count = 0;
    for (int i = 1; i <= 8; i++) {
        if (reach[i])
            count++;
    }
    // if (count == 8)
    //     printf("\n Graph is connected");
    // else
    //     printf("\n Graph is not connected");
}

// int main(int argc, char *argv[]) {
//     // int i, j, count = 0;
//     // printf("\n Enter number of vertices:");
//     // scanf("%d", &n);
//     // for (i = 1; i <= n; i++) {
//     //     reach[i] = 0;
//     //     for (j = 1; j <= n; j++)
//     //         a[i][j] = 0;
//     // }
//     // printf("\n Enter the adjacency matrix:\n");
//     // for (i = 1; i <= n; i++)
//     //     for (j = 1; j <= n; j++)
//     //         scanf("%d", &a[i][j]);
//     // dfs(1);
//     // printf("\n");
// 
//     dfs_simple_call();
//     return 0;
// }