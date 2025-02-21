project(msg)

add_library(${PROJECT_NAME} src/MagneticFluxDensityData.cpp src/Message.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)