cmake_minimum_required(VERSION 3.30)
project(matlab_codegen_vs_sympy)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp dipol_model.h dipol_model.cpp sensor_array_dipol_model.h sensor_array_dipol_model.c codegen/lib/model/model.c codegen/lib/model/model_initialize.c codegen/lib/model/model_terminate.c codegen/lib/model/rt_nonfinite.c codegen/lib/model/rtGetInf.c codegen/lib/model/rtGetNaN.c codegen/lib/model_jacobian/model_jacobian.c)
target_include_directories(${PROJECT_NAME} PUBLIC codegen/lib /usr/local/MATLAB/R2024b/extern/include)
target_link_directories(${PROJECT_NAME} PUBLIC codegen/lib)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC benchmark::benchmark)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")