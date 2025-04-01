#include <common_output.h>

#include <Eigen/Eigen>
#include <filesystem>
#include <fstream>
#include <generator>
#include <nlohmann/json.hpp>
#include <ranges>

#include "CSVMagnetometerArrayMagneticFluxDensityDataReader.h"
#include "EigenJsonUtils.h"
#include "LevenbergMarquardtOptimizer.h"
#include "MagneticFluxDensityDataTransformation.h"
#include "MedabilityMagnetometerArrayTransformer.h"
#include "PositionDirectionError.h"
#include "PositionDirectionPrinter.h"

int main() {
	std::ifstream stream(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "current_calibration.json");

	nlohmann::json json = nlohmann::json::parse(stream);
	std::vector transformations = json | std::ranges::views::transform([](auto const &v) { return v["transformation"].template get<Eigen::Matrix<double, 4, 4>>(); }) | std::ranges::to<std::vector>();

	double H = 4e-3;
	double R = 3e-3;
	double m = 1.35 / 4 * H * R * R;

	// CSVMagnetometerArrayMagneticFluxDensityDataReader<16, false> reader(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "LIS3MDL_ARRAY_Besprechungsraum" / "1" / "mag_data_LIS3MDL_ARRAY_2025Mar28_17h22min51s_ground_truth_data.txt");
	CSVMagnetometerArrayMagneticFluxDensityDataReader<16, false> reader(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "LIS3MDL_ARRAY_Besprechungsraum" / "2" / "mag_data_LIS3MDL_ARRAY_2025Apr01_14h42min53s_ground_truth_data.txt");
	MedabilityMagnetometerArrayTransformer<16> transform;
	MagneticFluxDensityDataTransformation<16> coordinate_transformation(std::forward<decltype(transformations)>(transformations));
	LevenbergMarquardtOptimizer<16> opti(m);
	PositionDirectionPrinter printer;
	// PositionDirectionError err(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "LIS3MDL_ARRAY_Besprechungsraum" / "2" / "mag_data_LIS3MDL_ARRAY_2025Mar28_17h22min51s_ground_truth_data.txt");
	PositionDirectionError err(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "LIS3MDL_ARRAY_Besprechungsraum" / "2" / "mag_data_LIS3MDL_ARRAY_2025Apr01_14h42min53s_ground_truth_data.txt");

	reader.asynchronously_connect(transform);
	transform.asynchronously_connect(coordinate_transformation);
	coordinate_transformation.asynchronously_connect(opti);
	opti.synchronously_connect(printer);
	opti.synchronously_connect(err);

	auto reader_thread = reader();
	auto transform_thread = transform();
	auto coordinate_transformation_thread = coordinate_transformation();
	auto opti_thread = opti();

	std::this_thread::sleep_for(std::chrono::seconds(10));
}