''' This program generates standard input for the main.c file.
The main.c program executes functions specified by stdin.
Example stdin for main.c:
    "a2time,0,bitmnp,0,rspeed,0,idctrn,0,puwmod,0,tblook,0,ttsprk,0"

Input above would make the main.c program execute all functions once
input index being 0. Input index can't exceed the maximum inputs count -1.
It also should not repeat.

In this script words like "benchmark", "algorithm" and "function" are sometimes used interchangeably.
'''

import itertools
import random
import sys
import os
from abc import ABC, abstractmethod

OUTPUT_FILE = 'random_functions.py'


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


tests = [1, 1024, 500000, 1048576, 50000000, 1073741824, 5000000000, 1099511627776, 5000000000000]

for t in tests: print("{0} == {1}".format(t,humanbytes(t)))


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

inputs_left_dict = generate_possible_inputs_dict(ALL_BENCHMARK_ALGORITHMS)

def get_random_input(benchmark_name, inputs_left_dict):
    ''' Returns random input for the benchmark, 
    if all inputs for the benchmark were used, returns None '''
    inputs_left = inputs_left_dict[benchmark_name]
    if len(inputs_left) == 0:
        raise Exception("No inputs left for benchmark: " + benchmark_name)
    random_input = random.choice(inputs_left)
    inputs_left.remove(random_input)
    return random_input

def produce_stdins(benchmarks, inputs_left_dict, stdin_count):
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
        stdin = ','.join([f'{benchmark_name},{input}' for benchmark_name, input in zip(combination, inputs)])
        stdin_list.append(stdin)
    return stdin_list

def estimate_csv_space(stdins):
    total_space = 0
    for stdin in stdins:
        for i, token in enumerate(stdin.split(',')):
            if i % 2 == 1:
                continue
            total_space += ALL_BENCHMARK_ALGORITHMS[token].estimated_csv_space
    return total_space

train_stdins = produce_stdins(TRAINING_BENCHMARKS, inputs_left_dict, 100)
for stdin in train_stdins:
    print(stdin)

print()
print("Inputs left after generating training examples:")
for name, inps in inputs_left_dict.items():
    print(f'{name:<7} {len(inps):<4} ({len(inps)/ALL_BENCHMARK_ALGORITHMS[name].max_inputs_count*100:.0f}%)')

# Testing dataset will have 3 categories:
# 1. Consisting of the same inputs used during training (to test for false positives)
# 2. Consisting of training benchmarks with previously unused inputs (acting as anomalous program runs)
# 3. Consisttng of testing benchmarks (acting as anomalous program runs)

def produce_testing_stdins(training_stdins, training_benchmarks, testing_benchmarks, inputs_left_dict, stdin_category_counts):
    cat1_stdins = random.sample(training_stdins, stdin_category_counts[0])
    cat2_stdins = produce_stdins(training_benchmarks, inputs_left_dict, stdin_category_counts[1])
    cat3_stdins = produce_stdins(testing_benchmarks, inputs_left_dict, stdin_category_counts[2])
    return cat1_stdins, cat2_stdins, cat3_stdins

test_cat1_stdins, test_cat2_stdins, test_cat3_stdins = produce_testing_stdins(
    train_stdins,
    TRAINING_BENCHMARKS,
    TESTING_BENCHMARKS,
    inputs_left_dict,
    stdin_category_counts=[33, 33, 34]
    )

# print estimated disk space required for csv files
total_space = estimate_csv_space(train_stdins + test_cat1_stdins + test_cat2_stdins + test_cat3_stdins)
print()
print(f'Total estimated space required for csv files: {humanbytes(total_space)}')
print(f'- training: {humanbytes(estimate_csv_space(train_stdins))}')
print(f'- testing cat1: {humanbytes(estimate_csv_space(test_cat1_stdins))}')
print(f'- testing cat2: {humanbytes(estimate_csv_space(test_cat2_stdins))}')
print(f'- testing cat3: {humanbytes(estimate_csv_space(test_cat3_stdins))}')


with open(OUTPUT_FILE, 'w') as f:
    f.write(f'train_stdins = [\n')
    f.write(',\n'.join([f'    "{stdin}"' for stdin in train_stdins]))
    f.write('\n]\n\n')

    f.write(f'test_cat1_stdins = [\n')
    f.write(',\n'.join([f'    "{stdin}"' for stdin in test_cat1_stdins]))
    f.write('\n]\n\n')

    f.write(f'test_cat2_stdins = [\n')
    f.write(',\n'.join([f'    "{stdin}"' for stdin in test_cat2_stdins]))
    f.write('\n]\n\n')

    f.write(f'test_cat3_stdins = [\n')
    f.write(',\n'.join([f'    "{stdin}"' for stdin in test_cat3_stdins]))
    f.write('\n]\n\n')

