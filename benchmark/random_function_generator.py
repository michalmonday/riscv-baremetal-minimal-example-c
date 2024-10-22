''' This program generates standard input for the main.c file.
The main.c program executes functions specified by stdin.
Format:
    "function_name,input_index,iterations,benchmark_name,input_index,iterations..."
Example stdin for main.c:
    "a2time,0,1,bitmnp,0,1,rspeed,0,1,idctrn,0,1,puwmod,0,1,tblook,0,1,ttsprk,0,1"

Input above would make the main.c program execute all functions once
input index being 0. Input index can't exceed the maximum inputs count -1.
It also should not repeat.

In this script words like "benchmark", "algorithm" and "function" are sometimes used interchangeably.


TODO: ITERATIONS must be dynamic (based on the estimated trace size of specific function/algorithm),
      it must be set to make the program execute for at least 300_000 cycles.
      We are aiming to collect a single data item (put it into FIFO) around every 100_000 cycles.
      As ridiculous as it sounds, this would allow software anomaly detection. Thanks to collecting
      HPC values, it will be possible to profile the program behaviour despite collecting the data
      rarely. For that reason the program execution must be at least 300_000 cycles long, aiming to collect 
      at least 2 or 3 data items during program execution (which will be enough for recognizing that different software is running).
'''

import itertools
import random
import sys
import os
from abc import ABC, abstractmethod
import math
import argparse

parser = argparse.ArgumentParser(description='Generate random functions for benchmarking')
parser.add_argument('--output-file', type=str, help='Output file name')
parser.add_argument('--min-clk-count', type=int, default=400_000, help='Minimum clock count for each function')
parser.add_argument('--count', type=int, default=100, help='Number of functions to generate')
parser.add_argument('--all-algorithms-for-training', action='store_true', help='Generate functions for all algorithms')
args = parser.parse_args()

# OUTPUT_FILE = 'random_functions.py'
# MIN_CLK_COUNT = 400_000

class BenchmarkAlgorithm(ABC):
    ''' This abstract class is the base of every algorithm, 
    it ensures each algorithm implements the same members/properties. '''
    @property
    @abstractmethod
    def name(self):
        raise NotImplementedError("Subclasses must implement this")
        
    @property 
    @abstractmethod
    def full_name(self):
        raise NotImplementedError("Subclasses must implement this")
        
    @property
    @abstractmethod
    def max_inputs_count(self):
        ''' maximum possible inputs '''
        raise NotImplementedError("Subclasses must implement this")
        
    @property
    @abstractmethod
    def inputs_count(self):
        ''' inputs to use while generating dataset, input index is 
        supplied to programs by stdin '''
        raise NotImplementedError("Subclasses must implement this")

    @property
    @abstractmethod
    def estimated_csv_space(self):
        ''' Size in bytes of the csv containing metrics collected
        from a single run of the program. '''
        raise NotImplementedError("Subclasses must implement this")

    @property
    @abstractmethod
    def estimated_iteration_clk_count(self):
        ''' Estimated clock cycles per iteration of the algorithm. 
        It can be used to calculate how many iterations should be run
        to let the program run for at least 300_000 or more cycles. ''
        It was calculated by an experiment like: 
        # Experiment to see clock duration of each iteration of each algorithm
        algorithm_names = [
            'a2time',
            'bitmnp',
            'rspeed',
            'idctrn',
            'puwmod',
            'tblook',
            'ttsprk'
        ]
        stdins = []
        for name in algorithm_names:
            stdins.append(f'{name},0,1') # on purpose
            stdins.append(f'{name},0,1')
            stdins.append(f'{name},0,2')
            stdins.append(f'{name},0,3')
            stdins.append(f'{name},0,4')
            stdins.append(f'{name},0,5')
            stdins.append(f'{name},0,6')
            stdins.append(f'{name},0,7')
            stdins.append(f'{name},0,8')
            stdins.append(f'{name},0,9')
        random_functions = {'time_per_iteration_experiment' : stdins}
        csv_creation_timings = create_csvs_for_all_stdin_lists(random_functions,
                                                               dst_dir=dst_dir, 
                                                               packet_format=packet_format, 
                                                               columns_order=columns_order)
        clocks_per_iteration = {name : [] for name in algorithm_names}
        single_iteration_times = {}
        for stdin in stdins:
            name, input_index, iterations = stdin.split(',')
            f_name = Path(dst_dir) / 'time_per_iteration_experiment' / f'{name}_{input_index}_{iterations}.csv'
            iterations = int(iterations)
            total_time = pd.read_csv(f_name)['total_clk_counter_halt_agnostic'].iloc[-1]
            if iterations == 1:
                single_iteration_times[name] = total_time
                continue
            clocks_per_iteration[name].append((total_time - single_iteration_times[name]) / iterations)
        for name, clk_ticks in clocks_per_iteration.items():
            print(name, clk_ticks)
        '''
        raise NotImplementedError("Subclasses must implement this")
        
class a2time(BenchmarkAlgorithm): 
    @property
    def name(self):
        return 'a2time'
    @property
    def full_name(self):
        return 'Angle to Time Conversion'
    @property
    def max_inputs_count(self):
        return 500
    @property
    def inputs_count(self):
        ''' single collected file is around 680KB so all 500 runs can be collected '''
        return 500
    @property
    def estimated_csv_space(self):
        return 680_000 # 680KB
    @property 
    def estimated_iteration_clk_count(self):
        return 300
        

# aittft never ends execution, it probably wasn't adapted well
    
# class aittft(BenchmarkAlgorithm): 
#     @property
#     def name(self):
#         return 'a2time'
#     @property
#     def max_inputs_count(self):
#         return 200
#     @property
#     def inputs_count(self):
#         ''' running aittft is problematic (program resets in weird way before finishing the program) '''
        return 0

class bitmnp(BenchmarkAlgorithm): 
    @property
    def name(self):
        return 'bitmnp'
    @property
    def full_name(self):
        return 'Bit Manipulation'
    @property
    def max_inputs_count(self):
        return 128
    @property
    def inputs_count(self):
        ''' single collected file is around 25MB so need to be careful with setting this '''
        return 5
    @property
    def estimated_csv_space(self):
        return 25_000_000 # 25MB
    @property 
    def estimated_iteration_clk_count(self):
        return 44500
    
class rspeed(BenchmarkAlgorithm): 
    @property
    def name(self):
        return 'rspeed'    
    @property
    def full_name(self):
        return 'Road speed calculation'
    @property
    def max_inputs_count(self):
        return 500
    @property
    def inputs_count(self):
        ''' single collected file is around 450KB so all 500 runs can be collected '''
        return self.max_inputs_count 
    @property
    def estimated_csv_space(self):
        return 450_000 # 450KB
    @property 
    def estimated_iteration_clk_count(self):
        return 210
    
class idctrn(BenchmarkAlgorithm): 
    @property
    def name(self):
        return 'idctrn'
    @property
    def full_name(self):
        return 'Inverse Discrete Cosine Transform'
    @property
    def max_inputs_count(self):
        return 512
    @property
    def inputs_count(self):
        ''' single collected file is around 15MB so need to be careful with setting this '''
        return 10 
    @property
    def estimated_csv_space(self):
        return 15_000_000 # 15MB
    @property 
    def estimated_iteration_clk_count(self):
        return 53000
    
class puwmod(BenchmarkAlgorithm): 
    @property
    def name(self):
        return 'puwmod'
    @property
    def full_name(self):
        return 'Pulse Width Modulation'
    @property
    def max_inputs_count(self):
        return 2420
    @property
    def inputs_count(self):
        ''' single collected file is around 1MB, running it 2420 would collect over 2GB data,
        so for now it's set to 512 ''' 
        return 500
    @property
    def estimated_csv_space(self):
        return 1_000_000 # 1MB
    @property 
    def estimated_iteration_clk_count(self):
        return 120

class tblook(BenchmarkAlgorithm): 
    @property
    def name(self):
        return 'tblook'
    @property
    def full_name(self):
        return 'Table Lookup and Interpolation'
    @property
    def max_inputs_count(self):
        return 232
    @property
    def inputs_count(self):
        ''' single collected file is around 270KB so all 232 runs are collected ''' 
        return 232
    @property
    def estimated_csv_space(self):
        return 270_000 # 270KB
    @property 
    def estimated_iteration_clk_count(self):
        return 1400
    
class ttsprk(BenchmarkAlgorithm): 
    @property
    def name(self):
        return 'ttsprk'
    @property
    def full_name(self):
        return 'Tooth to Spark'
    @property
    def max_inputs_count(self):
        return 200
    @property
    def inputs_count(self):
        ''' single collected file is around 2MB, all 200 runs will collect around 400MB which is ok ''' 
        return 200
    @property
    def estimated_csv_space(self):
        return 2_000_000 # 2MB
    @property 
    def estimated_iteration_clk_count(self):
        return 450

def humanbytes(B):
    """Return the given bytes as a human friendly KB, MB, GB, or TB string."""
    B = float(B)
    KB = float(1024)
    MB = float(KB ** 2) # 1,048,576
    GB = float(KB ** 3) # 1,073,741,824
    TB = float(KB ** 4) # 1,099,511,627,776

    if B < KB:
        return '{0} {1}'.format(B,'Bytes' if 0 == B > 1 else 'Byte')
    elif KB <= B < MB:
        return '{0:.2f} KB'.format(B / KB)
    elif MB <= B < GB:
        return '{0:.2f} MB'.format(B / MB)
    elif GB <= B < TB:
        return '{0:.2f} GB'.format(B / GB)
    elif TB <= B:
        return '{0:.2f} TB'.format(B / TB)

def generate_possible_inputs_dict(algos):
    possible_inputs = {}
    for name, obj in algos.items():
        possible_inputs[name] = [i for i in range(obj.max_inputs_count)]
    return possible_inputs


ALL_BENCHMARK_ALGORITHMS = {b.name : b for b in [
    a2time(),
    #aittft(),
    bitmnp(),
    rspeed(),
    idctrn(),
    puwmod(),
    tblook(),
    ttsprk()
]}

TRAINING_BENCHMARKS = {b.name : b for b in [
    a2time(), # Angle to Time Conversion
    bitmnp(), # Bit Manipulation
    rspeed(), # Road speed calculation
    idctrn()  # Inverse Discrete Cosine Transform
]}

TESTING_BENCHMARKS = {b.name : b for b in [
    puwmod(), # Pulse Width Modulation
    tblook(), # Table Lookup and Interpolation
    ttsprk()  # Tooth to Spark
]}



def get_random_input(benchmark_name, inputs_left_dict):
    ''' Returns random input for the benchmark, 
    if all inputs for the benchmark were used, returns None '''
    inputs_left = inputs_left_dict[benchmark_name]
    if len(inputs_left) == 0:
        raise Exception("No inputs left for benchmark: " + benchmark_name)
    random_input = random.choice(inputs_left)
    inputs_left.remove(random_input)
    return random_input

def get_iterations_to_reach_clk_count(benchmark_name, clk_count):
    ''' Returns the number of iterations required to reach the specified clk_count '''
    return math.ceil(clk_count / ALL_BENCHMARK_ALGORITHMS[benchmark_name].estimated_iteration_clk_count)

def produce_stdins(benchmarks, inputs_left_dict, stdin_count, min_clk_count):
    stdin_list = []
    # generate all combinations of training benchmark names without repetitions 
    combinations = []
    for i in range(1, len(benchmarks)+1):
        # by default itertools.combinations returns tuples, convert them to lists so they can be shuffled
        combinations += list(list(tup) for tup in itertools.combinations(benchmarks.keys(), i))
    for i in range(stdin_count):
        # get random combination of training benchmarks
        combination = random.choice(combinations)
        # shuffle the combination so the order of function calls is random
        random.shuffle(combination)
        # get random input for each benchmark
        inputs = [get_random_input(benchmark_name, inputs_left_dict) for benchmark_name in combination]
        min_clk_count_per_algorithm = min_clk_count / len(combination)

        stdin = ','.join([f'{benchmark_name},{input},{get_iterations_to_reach_clk_count(benchmark_name, min_clk_count_per_algorithm)}' for benchmark_name, input in zip(combination, inputs)])
        stdin_list.append(stdin)
    return stdin_list

def estimate_csv_space(stdins):
    total_space = 0
    for stdin in stdins:
        for i, token in enumerate(stdin.split(',')):
            if i % 3 != 0:
                continue
            total_space += ALL_BENCHMARK_ALGORITHMS[token].estimated_csv_space
    return total_space

def estimate_clk_count(stdins):
    total_clk_count = 0
    for stdin in stdins:
        tokens = stdin.split(',')
        for i, token in enumerate(tokens):
            if i % 3 != 0:
                continue
            iterations = int(tokens[i+2])
            total_clk_count += ALL_BENCHMARK_ALGORITHMS[token].estimated_iteration_clk_count * iterations
    return total_clk_count

def produce_testing_stdins(training_stdins, training_benchmarks, testing_benchmarks, inputs_left_dict, stdin_category_counts):
    cat1_stdins = random.sample(training_stdins, stdin_category_counts[0])
    cat2_stdins = produce_stdins(training_benchmarks, inputs_left_dict, stdin_category_counts[1], args.min_clk_count)
    cat3_stdins = produce_stdins(testing_benchmarks, inputs_left_dict, stdin_category_counts[2], args.min_clk_count)
    return cat1_stdins, cat2_stdins, cat3_stdins

def produce_standalone_stdins(all_benchmarks, min_clk_count):
    ''' Produces stdins for each benchmark alone with input index 0 and iterations to reach min_clk_count '''
    return [f'{name},0,{get_iterations_to_reach_clk_count(name, min_clk_count)}' for name in all_benchmarks.keys()]

def produce_standalone_single_iteration_stdins(all_benchmarks):
    ''' Produces stdins for each benchmark alone with input index 0 and iterations to reach MIN_CLK_COUNT '''
    return [f'{name},0,1' for name in all_benchmarks.keys()]


if args.all_algorithms_for_training:
    TRAINING_BENCHMARKS = ALL_BENCHMARK_ALGORITHMS

inputs_left_dict = generate_possible_inputs_dict(ALL_BENCHMARK_ALGORITHMS)

train_stdins = produce_stdins(TRAINING_BENCHMARKS, inputs_left_dict, args.count, args.min_clk_count)
for stdin in train_stdins:
    print(stdin)
print()
print("Inputs left after generating training examples:")
for name, inps in inputs_left_dict.items():
    print(f'{name:<7} {len(inps):<4} ({len(inps)/ALL_BENCHMARK_ALGORITHMS[name].max_inputs_count*100:.0f}%)')

# Testing dataset will have 3 categories:
# 1. Consisting of the same inputs used during training (to test for false positives)
# 2. Consisting of training benchmarks with previously unused inputs (to test for false positives)
# 3. Consisting of testing benchmarks (acting as anomalous program runs)

standalone_stdins = produce_standalone_stdins(ALL_BENCHMARK_ALGORITHMS, args.min_clk_count)
standalone_single_iteration_stdins = produce_standalone_single_iteration_stdins(ALL_BENCHMARK_ALGORITHMS)

test_cat1_stdins, test_cat2_stdins, test_cat3_stdins = produce_testing_stdins(
    train_stdins,
    TRAINING_BENCHMARKS,
    TESTING_BENCHMARKS,
    inputs_left_dict,
    stdin_category_counts=[args.count, args.count, args.count]
    )

# print estimated disk space required for csv files
total_space = estimate_csv_space(train_stdins + test_cat1_stdins + test_cat2_stdins + test_cat3_stdins)
print()
print("WARNING: Following estimates do not take into account iterations (these all assume iterations=1).")
print(f'Total estimated space required for csv files: {humanbytes(total_space)}')
print(f'- training: {humanbytes(estimate_csv_space(train_stdins))}')
print(f'- testing cat1: {humanbytes(estimate_csv_space(test_cat1_stdins))}')
print(f'- testing cat2: {humanbytes(estimate_csv_space(test_cat2_stdins))}')
print(f'- testing cat3: {humanbytes(estimate_csv_space(test_cat3_stdins))}')


# save dict to file like:
# random_functions: {
#     "train_stdins" : [...],
#     "test_cat1_stdins" : [...],
#     "test_cat2_stdins" : [...],
#     "test_cat3_stdins" : [...]
# }
with open(args.output_file, 'w') as f:
    f.write('random_functions = {\n')
    f.write('    "standalone" : [\n')
    f.write(',\n'.join([f'        "{stdin}"' for stdin in standalone_stdins]))
    f.write('\n    ],\n\n')

    f.write('    "standalone_single_iteration" : [\n')
    f.write(',\n'.join([f'        "{stdin}"' for stdin in standalone_single_iteration_stdins]))
    f.write('\n    ],\n\n')
    
    f.write(f'    "train" : [\n')
    f.write(',\n'.join([f'        "{stdin}"' for stdin in train_stdins]))
    f.write('\n    ],\n\n')

    f.write(f'    "test_cat1" : [\n')
    f.write(',\n'.join([f'        "{stdin}"' for stdin in test_cat1_stdins]))
    f.write('\n    ],\n\n')

    f.write(f'    "test_cat2" : [\n')
    f.write(',\n'.join([f'        "{stdin}"' for stdin in test_cat2_stdins]))
    f.write('\n    ],\n\n')

    f.write(f'    "test_cat3" : [\n')
    f.write(',\n'.join([f'        "{stdin}"' for stdin in test_cat3_stdins]))
    f.write('\n    ]\n\n')

    f.write('}\n')

