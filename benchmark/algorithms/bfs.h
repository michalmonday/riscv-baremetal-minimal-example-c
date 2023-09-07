#ifndef BDS_H
#define BDS_H

extern void bfs_simple_call();
extern void bfs(int v, int n, int adj[][n], int *visited, int *queue, int *front, int *rear);

#endif