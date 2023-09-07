// Copied from: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/?ref=lbp

// C program for Dijkstra's single source shortest path
// algorithm. The program is for adjacency matrix
// representation of the graph

#include <limits.h>
#include <stdbool.h>
// #include <stdio.h>

#include <standard_libraries.h>

// A utility function to find the vertex with minimum
// distance value, from the set of vertices not yet included
// in shortest path tree
int minDistance(int n, int dist[], bool sptSet[])
{
	// Initialize min value
	int min = INT_MAX, min_index;

	for (int v = 0; v < n; v++)
		if (sptSet[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}

// A utility function to print the constructed distance
// array
void printSolution(int n, int dist[])
{
	// printf("Vertex \t\t Distance from Source\n");
	for (int i = 0; i < n; i++) {
		// printf("%d \t\t\t\t %d\n", i, dist[i]);
	}
}

// Function that implements Dijkstra's single source
// shortest path algorithm for a graph represented using
// adjacency matrix representation
void dijkstra(int n, int graph[][n], int src)
{
	int dist[n]; // The output array. dist[i] will hold the
				// shortest
	// distance from src to i

	bool sptSet[n]; // sptSet[i] will be true if vertex i is
					// included in shortest
	// path tree or shortest distance from src to i is
	// finalized

	// Initialize all distances as INFINITE and stpSet[] as
	// false
	for (int i = 0; i < n; i++)
		dist[i] = INT_MAX, sptSet[i] = false;

	// Distance of source vertex from itself is always 0
	dist[src] = 0;

	// Find shortest path for all vertices
	for (int count = 0; count < n - 1; count++) {
		// Pick the minimum distance vertex from the set of
		// vertices not yet processed. u is always equal to
		// src in the first iteration.
		int u = minDistance(n, dist, sptSet);

		// Mark the picked vertex as processed
		sptSet[u] = true;

		// Update dist value of the adjacent vertices of the
		// picked vertex.
		for (int v = 0; v < n; v++)

			// Update dist[v] only if is not in sptSet,
			// there is an edge from u to v, and total
			// weight of path from src to v through u is
			// smaller than current value of dist[v]
			if (!sptSet[v] && graph[u][v]
				&& dist[u] != INT_MAX
				&& dist[u] + graph[u][v] < dist[v])
				dist[v] = dist[u] + graph[u][v];
	}

	// print the constructed distance array
	printSolution(n, dist);
}

void dijkstra_simple_call() {
	int graph[9][9] = { { 0, 4, 0, 0, 0, 0, 0, 8, 0 },
						{ 4, 0, 8, 0, 0, 0, 0, 11, 0 },
						{ 0, 8, 0, 7, 0, 4, 0, 0, 2 },
						{ 0, 0, 7, 0, 9, 14, 0, 0, 0 },
						{ 0, 0, 0, 9, 0, 10, 0, 0, 0 },
						{ 0, 0, 4, 14, 10, 0, 2, 0, 0 },
						{ 0, 0, 0, 0, 0, 2, 0, 1, 6 },
						{ 8, 11, 0, 0, 0, 0, 1, 0, 7 },
						{ 0, 0, 2, 0, 0, 0, 6, 7, 0 } };
	// Function call
	dijkstra(9, graph, 0);
}

// // driver's code
// int main()
// {
// 	/* Let us create the example graph discussed above */
// 	int graph[n][n] = { { 0, 4, 0, 0, 0, 0, 0, 8, 0 },
// 						{ 4, 0, 8, 0, 0, 0, 0, 11, 0 },
// 						{ 0, 8, 0, 7, 0, 4, 0, 0, 2 },
// 						{ 0, 0, 7, 0, 9, 14, 0, 0, 0 },
// 						{ 0, 0, 0, 9, 0, 10, 0, 0, 0 },
// 						{ 0, 0, 4, 14, 10, 0, 2, 0, 0 },
// 						{ 0, 0, 0, 0, 0, 2, 0, 1, 6 },
// 						{ 8, 11, 0, 0, 0, 0, 1, 0, 7 },
// 						{ 0, 0, 2, 0, 0, 0, 6, 7, 0 } };
// 
// 	// Function call
// 	dijkstra(n, graph, 0);
// 
// 	return 0;
// }
