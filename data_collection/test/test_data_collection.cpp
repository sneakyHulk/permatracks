#include <MagnetometerArrayMagneticFluxDensityDataPrinter.h>
#include <Pusher.h>
#include <common_output.h>

#include <chrono>
#include <csv.hpp>
#include <filesystem>
#include <fstream>
#include <generator>
#include <nlohmann/json.hpp>

#include "Array.h"
#include "CSVMagnetometerArrayMagneticFluxDensityDataPositionDirectionReader.h"
#include "CSVMagnetometerArrayMagneticFluxDensityDataReader.h"
#include "Message.h"

int main() {
	for (auto const e : CSVMagnetometerArrayMagneticFluxDensityDataPositionDirectionReader<16>::get(
	         std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "LIS3MDL_ARRAY_Besprechungsraum" / "mag_data_LIS3MDL_ARRAY_2025Mar28_17h22min51s_ground_truth_data.txt")) {
		std::cout << e << std::endl;
	}

	CSVMagnetometerArrayMagneticFluxDensityDataReader<16> reader(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "LIS3MDL_ARRAY_Besprechungsraum" / "mag_data_LIS3MDL_ARRAY_2025Mar28_17h22min51s_ground_truth_data.txt");

	MagnetometerArrayMagneticFluxDensityDataPrinter printer;
	reader.synchronously_connect(printer);
	auto reader_thread = reader();

	std::this_thread::sleep_for(std::chrono::seconds(20));
}