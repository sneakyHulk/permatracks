project(optimization)

add_library(${PROJECT_NAME} src/sensor_array_dipol_model.c)
target_include_directories(${PROJECT_NAME} PUBLIC include)

project(test_optimization)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp src/test_model.c)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC optimization)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

