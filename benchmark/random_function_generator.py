''' This program generates standard input for the main.c file.
The main.c program executes functions specified by stdin.
Example stdin for main.c:
    "a2time,0,bitmnp,0,rspeed,0,idctrn,0,puwmod,0,tblook,0,ttsprk,0"

Input above would make the main.c program execute all functions once
input index being 0. Input index can't exceed the maximum inputs count -1.
It also should not repeat.
'''

import itertools
import random
import sys
import os
from abc import ABC, abstractmethod

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


def generate_possible_inputs_dict(algos):
    possible_inputs = {}
    for name, obj in algos.items():
        possible_inputs[name] = [i for i in range(obj.max_inputs_count)]
    return possible_inputs

    
ALL_BENCHMARK_ALGORITHMS = {s.name : s for s in [
    a2time(),
    #aittft(),
    bitmnp(),
    rspeed(),
    idctrn(),
    puwmod(),
    tblook(),
    ttsprk()
]}

INPUTS_LEFT = generate_possible_inputs_dict(ALL_BENCHMARK_ALGORITHMS)

for name, obj in ALL_BENCHMARK_ALGORITHMS.items():
    # print(name, obj.full_name, obj.max_inputs_count, obj.inputs_count)
    
