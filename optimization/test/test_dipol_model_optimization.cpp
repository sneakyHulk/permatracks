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

int main() {
	std::ifstream stream(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "current_calibration.json");

	nlohmann::json json = nlohmann::json::parse(stream);
	std::vector transformations = json | std::ranges::views::transform([](auto const &v) { return v["transformation"].template get<Eigen::Matrix<double, 4, 4>>(); }) | std::ranges::to<std::vector>();


	double H = 4e-3;
	double R = 3e-3;
	double m = 1.35 / 4 * H * R * R;

	CSVMagnetometerArrayMagneticFluxDensityDataReader<16> reader(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "mag_data_LIS3MDL_ARRAY_2025Mar28_17h22min51s_ground_truth_data.txt");
	MagneticFluxDensityDataTransformation<16> transformation(std::forward<decltype(transformations)>(transformations));
	LevenbergMarquardtOptimizer<16> opti(m);

	reader.asynchronously_connect(transformation);
	transformation.asynchronously_connect(opti);



}