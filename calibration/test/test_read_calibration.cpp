#include <common_output.h>

#include <Eigen/Eigen>
#include <filesystem>
#include <fstream>
#include <generator>
#include <nlohmann/json.hpp>
#include <ranges>

#include "EigenJsonUtils.h"
#include "MagneticFluxDensityDataTransformation.h"

int main() {
	std::ifstream stream(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "current_calibration.json");

	nlohmann::json json = nlohmann::json::parse(stream);
	std::vector transformations = json | std::ranges::views::transform([](auto const &v) { return v["transformation"].template get<Eigen::Matrix<double, 4, 4>>(); }) | std::ranges::to<std::vector>();

	MagneticFluxDensityDataTransformation<16> transformation(std::forward<decltype(transformations)>(transformations));


}
