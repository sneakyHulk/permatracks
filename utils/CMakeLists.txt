project(concurra_utils)

add_library(${PROJECT_NAME} src/Averager.cpp src/MessagePrinter.cpp src/Remover.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)

project(eigen_utils)

add_library(${PROJECT_NAME} src/EigenJsonUtils.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)

project(test_eigen_utils)

add_executable(${PROJECT_NAME} test/test_eigen_utils.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC eigen_utils)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
