#pragma once

#include <RunnerSynchronous.h>

#include <csv.hpp>
#include <filesystem>
#include <generator>

#include "Direction.h"
#include "Pack.h"
#include "Position.h"

class PositionDirectionError : public RunnerSynchronous<Pack<Position, Direction>> {
	std::filesystem::path _path;
	std::filesystem::path _out;
	std::ofstream _outfile;

	std::generator<Pack<Position, Direction>>* gen;
	decltype(gen->begin()) it;

	static std::generator<Pack<Position, Direction>> get(std::filesystem::path path) {
		csv::CSVReader reader(path.string());

		for (auto const& row : reader) {
			Pack<Position, Direction> ret;

			ret.x = row["mag_x0"].is_null() ? 0. : row["mag_x0"].get<double>();
			ret.y = row["mag_y0"].is_null() ? 0. : row["mag_y0"].get<double>();
			ret.z = row["mag_z0"].is_null() ? 0. : row["mag_z0"].get<double>();
			ret.theta = row["mag_theta0"].is_null() ? 0. : row["mag_theta0"].get<double>();
			ret.phi = row["mag_phi0"].is_null() ? 0. : row["mag_phi0"].get<double>();

			co_yield ret;
		}
	}

   public:
	explicit PositionDirectionError(std::filesystem::path&& path, std::filesystem::path&& out = std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "last_optimization_error.csv")
	    : _path(std::forward<decltype(path)>(path)), _out(std::forward<decltype(out)>(out)), gen(new std::generator(get(_path))), it(gen->begin()) {}

	~PositionDirectionError() { delete gen; }

   private:
	void run_once(Pack<Position, Direction> const& data) override {
		_outfile.open(_out);
		_outfile << "mag_x0,mag_y0,mag_z0,mag_theta0,mag_phi0,computed_mag_x0,computed_mag_y0,computed_mag_z0,computed_mag_theta0,computed_mag_phi0" << std::endl;

		return run(data);
	}
	void run(Pack<Position, Direction> const& data) override {
		if (it != gen->end()) {
			auto const ground_truth = *it;

			_outfile << ground_truth.x << "," << ground_truth.y << "," << ground_truth.z << "," << ground_truth.theta << "," << ground_truth.phi << "," << data.x << "," << data.y << "," << data.z << "," << data.theta << "," << data.phi
			         << std::endl;

			std::cout << "Error: " << std::hypot(ground_truth.x - data.x, ground_truth.y - data.y, ground_truth.z - data.z) << std::flush;
			std::cout << ", x:" << ground_truth.x << " vs. " << data.x << ", y: " << ground_truth.y << " vs. " << data.y << ", z: " << ground_truth.z << " vs. " << data.z << std::endl;

		} else {
			delete gen;

			gen = new std::generator(get(_path));
			it = gen->begin();

			return run(data);
		}

		++it;
	}
};