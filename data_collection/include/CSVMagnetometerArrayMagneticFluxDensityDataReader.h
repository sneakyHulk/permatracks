#pragma once

#include <Pusher.h>
#include <common_output.h>

#include <chrono>
#include <csv.hpp>
#include <filesystem>
#include <generator>
#include <ranges>

#include "Array.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "Pack.h"

template <std::size_t N, bool wait = true>
class CSVMagnetometerArrayMagneticFluxDensityDataReader : public Pusher<Message<Array<MagneticFluxDensityData, N>>> {
	std::filesystem::path _path;
	std::string _name;

	std::generator<Message<Array<MagneticFluxDensityData, N>>>* gen;
	decltype(gen->begin()) it;

	std::chrono::time_point<std::chrono::system_clock> _current_time;
	std::chrono::time_point<std::chrono::system_clock> _sensor_time;
	std::chrono::time_point<std::chrono::system_clock> next;

   public:
	explicit CSVMagnetometerArrayMagneticFluxDensityDataReader(std::filesystem::path&& path, std::string&& name = "LIS3MDL_ARRAY")
	    : _path(std::forward<decltype(path)>(path)), _name(std::forward<decltype(name)>(name)), gen(new std::generator<Message<Array<MagneticFluxDensityData, N>>>(get(_path, _name))), it(gen->begin()) {}

	static std::generator<Message<Array<MagneticFluxDensityData, N>>> get(std::filesystem::path path, std::string name) {
		csv::CSVReader reader(path.string());

		for (auto const& row : reader) {
			Message<Array<MagneticFluxDensityData, N>> ret;

			for (auto const& [i, out] : std::ranges::views::enumerate(ret)) {
				out = {row[common::stringprint('x', i)].template get<double>(), row[common::stringprint('y', i)].template get<double>(), row[common::stringprint('z', i)].template get<double>()};
			}

			ret.timestamp = row["timestamp"].get<std::uint32_t>();
			ret.src = name;
			co_yield ret;
		}
	}

   private:
	Message<Array<MagneticFluxDensityData, N>> push_once() override {
		auto out = *it;

		_current_time = std::chrono::system_clock::now();
		_sensor_time = next = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(out.timestamp));

		return push();
	}
	Message<Array<MagneticFluxDensityData, N>> push() override {
		if (++it != gen->end()) {
			auto ret = *it;

			if (auto t_new = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(ret.timestamp)); t_new < next) {
				_current_time = std::chrono::system_clock::now() + std::chrono::seconds(1);
				_sensor_time = next = t_new;

			} else {
				next = t_new;
			}

			if constexpr (wait) std::this_thread::sleep_until(_current_time + (next - _sensor_time));
			else std::this_thread::sleep_for(std::chrono::milliseconds(10));

			ret.timestamp = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

			return ret;
		} else {
			delete gen;

			std::this_thread::sleep_for(std::chrono::seconds(5));

			gen = new std::generator<Message<Array<MagneticFluxDensityData, N>>>(get(_path, _name));
			it = gen->begin();

			return push_once();
		}
	}

   public:
	~CSVMagnetometerArrayMagneticFluxDensityDataReader() { delete gen; }
};