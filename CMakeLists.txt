cmake_minimum_required(VERSION 3.30)
project(permatracks)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(thirdparty/common)
add_subdirectory(thirdparty/concurra)
add_subdirectory(thirdparty/matplotlibcpp17)
add_subdirectory(thirdparty/csv-parser)
set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_INSTALL OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(HAVE_STD_REGEX ON)
add_subdirectory(thirdparty/benchmark)

add_library(serial thirdparty/serial/src/serial.cc thirdparty/serial/src/impl/unix.cc thirdparty/serial/src/impl/win.cc)
target_include_directories(serial PUBLIC thirdparty/serial/include)

if (APPLE)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(FOUNDATION_LIBRARY Foundation REQUIRED)

    target_link_libraries(serial PUBLIC ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
endif ()

if (UNIX AND NOT APPLE)
    target_link_libraries(serial PUBLIC rt pthread)
endif ()

find_package(Boost REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(nlohmann_json REQUIRED)

add_subdirectory(msg)
add_subdirectory(calibration)
add_subdirectory(optimization)
add_subdirectory(data_collection)
add_subdirectory(visualization)
add_subdirectory(utils)
add_subdirectory(meetings/matlab_codegen_vs_sympy)


project(grab_mag_data)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC data_collection_online)
target_link_libraries(${PROJECT_NAME} PUBLIC visualization)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra_utils)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

project(grab_mag_data_array)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC data_collection_online)
target_link_libraries(${PROJECT_NAME} PUBLIC visualization)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra_utils)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC serial)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

project(grab_gound_truth_mag_data_array)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC data_collection_online)
target_link_libraries(${PROJECT_NAME} PUBLIC visualization)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra_utils)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

project(calibrate_array)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC data_collection_online)
target_link_libraries(${PROJECT_NAME} PUBLIC visualization)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra_utils)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

project(calc_hard_and_soft_iron_offsets)

add_executable(${PROJECT_NAME} src/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC matplotlibcpp17::matplotlibcpp17)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

project(test_calc_hard_and_soft_iron_offsets)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC matplotlibcpp17::matplotlibcpp17)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

project(test_magneto)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.c thirdparty/mymathlib/src/choleski.c thirdparty/mymathlib/src/copy_vector.c thirdparty/mymathlib/src/get_submatrix.c thirdparty/mymathlib/src/hessenberg_elementary.c thirdparty/mymathlib/src/identity_matrix.c thirdparty/mymathlib/src/interchange_cols.c thirdparty/mymathlib/src/interchange_rows.c thirdparty/mymathlib/src/lower_triangular.c thirdparty/mymathlib/src/matrix_x_its_transpose.c thirdparty/mymathlib/src/multiply_matrices.c thirdparty/mymathlib/src/qr_hessenberg_matrix.c thirdparty/mymathlib/src/transpose_square_matrix.c thirdparty/mymathlib/src/upper_triangular.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_directories(${PROJECT_NAME} PUBLIC)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC m)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC sciplot)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC matplotlibcpp17::matplotlibcpp17)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
