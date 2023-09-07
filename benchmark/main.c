
//#include <stdio.h>
//#include <stdlib.h>
#include <standard_libraries.h>
#include <uart_gpio.h>
#include <uart_pynq.h>
#include <utils_flute.h>
#include <sensors.h>

#include <mst_kruskal.h>
#include <mst_prim.h>
#include <shortest_path_dijkstra.h>
#include <bfs.h>

void func_0() { uart_pynq_printf("func_0\n"); }
void func_1() { uart_pynq_printf("func_1\n"); }
void func_2() { uart_pynq_printf("func_2\n"); }
void func_3() { uart_pynq_printf("func_3\n"); }
void func_4() { uart_pynq_printf("func_4\n"); }
void func_5() { uart_pynq_printf("func_5\n"); }
void func_6() { uart_pynq_printf("func_6\n"); }
void func_7() { uart_pynq_printf("func_7\n"); }
void func_8() { uart_pynq_printf("func_8\n"); }
void func_9() { uart_pynq_printf("func_9\n"); }


void (*func_ptr[10])() = { func_0, func_1, func_2, func_3, func_4, func_5, func_6, func_7, func_8, func_9 };

int main(int argc, char *argv[]) {
    
    for (int i=1; i < argc; i++) {
        int index = atoi(argv[i]);
        func_ptr[index]();
    }

    kruskalAlgo_simple_call();
    primAlgo_simple_call();
    dijkstra_simple_call();
    bfs_simple_call();
    dfs_simple_call();

 	// int graph[5][3] = { { 0, 1, 10 },
 	// 				     { 0, 2, 6 },
 	// 				     { 0, 3, 5 },
 	// 				     { 1, 3, 15 },
 	// 				     { 2, 3, 4 } };
 	// int minCost = kruskalAlgo(5, graph);

 	// int graph2[5][5] = { { 0, 2, 0, 6, 0 },
 	// 					 { 2, 0, 3, 8, 5 },
 	// 					 { 0, 3, 0, 0, 7 },
 	// 					 { 6, 8, 0, 0, 9 },
 	// 					 { 0, 5, 7, 9, 0 } };
 	// primAlgo(5, graph2);

 	// int graph3[9][9] = { { 0, 4, 0, 0, 0, 0, 0, 8, 0 },
 	// 					{ 4, 0, 8, 0, 0, 0, 0, 11, 0 },
 	// 					{ 0, 8, 0, 7, 0, 4, 0, 0, 2 },
 	// 					{ 0, 0, 7, 0, 9, 14, 0, 0, 0 },
 	// 					{ 0, 0, 0, 9, 0, 10, 0, 0, 0 },
 	// 					{ 0, 0, 4, 14, 10, 0, 2, 0, 0 },
 	// 					{ 0, 0, 0, 0, 0, 2, 0, 1, 6 },
 	// 					{ 8, 11, 0, 0, 0, 0, 1, 0, 7 },
 	// 					{ 0, 0, 2, 0, 0, 0, 6, 7, 0 } };
 
 	// dijkstra(9, graph3, 0);

    return 0;
}
