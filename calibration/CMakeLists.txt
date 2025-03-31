project(calibration)

add_library(${PROJECT_NAME} src/MagneticFluxDensityDataTransformation.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)


project(test_read_calibration)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE csv)
target_link_libraries(${PROJECT_NAME} PRIVATE gcov)
target_link_libraries(${PROJECT_NAME} PRIVATE calibration)
target_link_libraries(${PROJECT_NAME} PRIVATE common)
target_link_libraries(${PROJECT_NAME} PRIVATE eigen_utils)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")