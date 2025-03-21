project(msg)

add_library(${PROJECT_NAME} src/MagneticFluxDensityData.cpp src/Message.cpp src/MagneticFluxDensityIntegerData.cpp src/Position.cpp src/Direction.cpp src/Pack.cpp src/Box.cpp src/Array.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)