cmake_minimum_required(VERSION 3.30)
project(matlab_codegen_vs_sympy)

find_package(benchmark REQUIRED)

add_executable(matlab_codegen_vs_sympy sensor_array_dipol_model.c)