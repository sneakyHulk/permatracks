#pragma once

#include <Pusher.h>
#include <common_output.h>

#include <chrono>
#include <csv.hpp>
#include <filesystem>
#include <generator>
#include <ranges>

#include "Array.h"
#include "Direction.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "Pack.h"
#include "Position.h"

template <std::size_t N>
class CSVMagnetometerArrayMagneticFluxDensityDataPositionDirectionReader : public Pusher<Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>>> {
	std::filesystem::path _path;

	std::generator<Message<Array<MagneticFluxDensityData, N>>>* gen;
	decltype(gen->begin()) it;

	std::chrono::time_point<std::chrono::system_clock> _current_time;
	std::chrono::time_point<std::chrono::system_clock> _sensor_time;
	std::chrono::time_point<std::chrono::system_clock> next;

   public:
	explicit CSVMagnetometerArrayMagneticFluxDensityDataPositionDirectionReader(std::filesystem::path&& path)
	    : _path(std::forward<decltype(path)>(path)), gen(new std::generator<Message<Array<MagneticFluxDensityData, N>>>(get(_path))), it(gen->begin()) {}

	static std::generator<Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>>> get(std::filesystem::path path) {
		csv::CSVReader reader(path.string());

		for (auto const& row : reader) {
			Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>> ret;

			for (auto const& [i, out] : std::ranges::views::enumerate(ret)) {
				out = {row[common::stringprint('x', i)].template get<double>(), row[common::stringprint('y', i)].template get<double>(), row[common::stringprint('z', i)].template get<double>()};
			}

			ret.x = row["mag_x0"].template get<double>();
			ret.y = row["mag_y0"].template get<double>();
			ret.z = row["mag_z0"].template get<double>();
			ret.theta = row["mag_theta0"].template get<double>();
			ret.phi = row["mag_phi0"].template get<double>();

			ret.timestamp = row["timestamp"].get<std::uint32_t>();
			co_yield ret;
		}
	}

   private:
	Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>> push_once() override {
		auto out = *it;

		_current_time = std::chrono::system_clock::now();
		_sensor_time = next = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(out.timestamp));

		return push();
	}
	Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>> push() override {
		if (++it != gen->end()) {
			auto ret = *it;

			if (auto t_new = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(ret.timestamp)); t_new < next) {
				_current_time = std::chrono::system_clock::now() + std::chrono::seconds(1);
				_sensor_time = next = t_new;

			} else {
				next = t_new;
			}

			std::this_thread::sleep_until(_current_time + (next - _sensor_time));

			return ret;
		} else {
			delete gen;

			gen = new std::generator<Message<Array<MagneticFluxDensityData, N>>>(get(_path));
			it = gen->begin();

			return push_once();
		}
	}

   public:
	~CSVMagnetometerArrayMagneticFluxDensityDataPositionDirectionReader() { delete gen; }
};