project(calibration)

#add_library(${PROJECT_NAME})
#target_include_directories(${PROJECT_NAME} PUBLIC include)

project(test_read_calibration)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE csv)
target_link_libraries(${PROJECT_NAME} PRIVATE gcov)
target_link_libraries(${PROJECT_NAME} PRIVATE common)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")