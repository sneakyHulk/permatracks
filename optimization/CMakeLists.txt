project(optimization)

add_library(${PROJECT_NAME} src/sensor_array_dipol_model.c)
target_include_directories(${PROJECT_NAME} PUBLIC include)

project(test_optimization)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp src/test_model.c)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

project(test_dipol_model_optimization)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC optimization)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} PUBLIC eigen_utils)
target_link_libraries(${PROJECT_NAME} PUBLIC data_collection_offline)
target_link_libraries(${PROJECT_NAME} PUBLIC calibration)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

