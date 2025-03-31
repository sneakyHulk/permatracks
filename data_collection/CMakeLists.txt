project(data_collection_offline)

add_library(${PROJECT_NAME} src/CSVMagnetometerArrayReader.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)
target_link_libraries(${PROJECT_NAME} PUBLIC csv)
target_link_libraries(${PROJECT_NAME} PUBLIC gcov)
target_compile_options(${PROJECT_NAME} PUBLIC -fcoroutines)

project(data_collection_online)

add_library(${PROJECT_NAME} src/MagnetometerArraySerialConnection.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC concurra)
target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PUBLIC msg)

project(test_data_collection)

add_executable(${PROJECT_NAME} test/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC visualization)
target_link_libraries(${PROJECT_NAME} PUBLIC data_collection_offline)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")