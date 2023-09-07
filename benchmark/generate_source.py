import os

FILE_NAME = 'main.c'
EXECUTABLE_NAME = FILE_NAME.split('.')[0]
FUNC_PREFIX = 'func_'

normal_sequences = [
    "0 1 2"
    ]

abnormal_sequences = [
    "2 1 1",
    "1 1 0"
    ]


def generate_functions(count, func_prefix):
    funcs_str = ''
    funcs_names = []
    for i in range(count):
        func_name = f'{func_prefix}{i}';
        funcs_names.append(func_name)
        funcs_str += (
                f'void {func_name}() {{ uart_pynq_printf("{func_name}\\n"); }}\n'
                )

    return funcs_str, funcs_names


functions, funcs_names = generate_functions(10, func_prefix=FUNC_PREFIX)

content = f'''
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

{functions}

void (*func_ptr[{len(funcs_names)}])() = {{ {', '.join(funcs_names)} }};

int main(int argc, char *argv[]) {{
    
    for (int i=1; i < argc; i++) {{
        int index = atoi(argv[i]);
        func_ptr[index]();
    }}

    kruskalAlgo_simple_call();
    primAlgo_simple_call();
    dijkstra_simple_call();
    bfs_simple_call();
    dfs_simple_call();

 	// int graph[5][3] = {{ {{ 0, 1, 10 }},
 	// 				     {{ 0, 2, 6 }},
 	// 				     {{ 0, 3, 5 }},
 	// 				     {{ 1, 3, 15 }},
 	// 				     {{ 2, 3, 4 }} }};
 	// int minCost = kruskalAlgo(5, graph);

 	// int graph2[5][5] = {{ {{ 0, 2, 0, 6, 0 }},
 	// 					 {{ 2, 0, 3, 8, 5 }},
 	// 					 {{ 0, 3, 0, 0, 7 }},
 	// 					 {{ 6, 8, 0, 0, 9 }},
 	// 					 {{ 0, 5, 7, 9, 0 }} }};
 	// primAlgo(5, graph2);

 	// int graph3[9][9] = {{ {{ 0, 4, 0, 0, 0, 0, 0, 8, 0 }},
 	// 					{{ 4, 0, 8, 0, 0, 0, 0, 11, 0 }},
 	// 					{{ 0, 8, 0, 7, 0, 4, 0, 0, 2 }},
 	// 					{{ 0, 0, 7, 0, 9, 14, 0, 0, 0 }},
 	// 					{{ 0, 0, 0, 9, 0, 10, 0, 0, 0 }},
 	// 					{{ 0, 0, 4, 14, 10, 0, 2, 0, 0 }},
 	// 					{{ 0, 0, 0, 0, 0, 2, 0, 1, 6 }},
 	// 					{{ 8, 11, 0, 0, 0, 0, 1, 0, 7 }},
 	// 					{{ 0, 0, 2, 0, 0, 0, 6, 7, 0 }} }};
 
 	// dijkstra(9, graph3, 0);

    return 0;
}}
'''

with open(FILE_NAME, 'w') as f:
    f.write(content)

# print('Compling...')
# print( os.popen(f'gcc {FILE_NAME} -o {EXECUTABLE_NAME}').read() )
# 
# print('Running normal sequences:')
# for seq in normal_sequences:
#     seq = ' '.join(seq)
#     print( os.popen(f'./{EXECUTABLE_NAME} {seq}').read())
# 
# print()
# print('Running abnormal sequences:')
# for seq in abnormal_sequences:
#     print( os.popen(f'./{EXECUTABLE_NAME} {seq}').read())




