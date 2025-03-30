#include <MagnetometerArrayMagneticFluxDensityDataPrinter.h>
#include <Pusher.h>
#include <common_output.h>

#include <array>
#include <chrono>
#include <csv.hpp>
#include <filesystem>
#include <generator>
#include <ranges>
#include <set>

#include "Array.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"

template <std::size_t N>
class CSVSensorArrayReader : public Pusher<Message<Array<MagneticFluxDensityData, N>>> {
	std::filesystem::path path;
	csv::CSVReader reader;
	csv::CSVReader::iterator it;

	std::chrono::time_point<std::chrono::system_clock> _current_time;
	std::chrono::time_point<std::chrono::system_clock> _sensor_time;
	std::chrono::time_point<std::chrono::system_clock> next;

   public:
	explicit CSVSensorArrayReader(std::filesystem::path&& path) : path(path), reader(path.string()), it(reader.begin()) {}

   private:
	Message<Array<MagneticFluxDensityData, N>> push_once() override {
		auto first_timestamp = (*it)["timestamp"].get<std::uint32_t>();
		_current_time = std::chrono::system_clock::now();
		_sensor_time = next = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(first_timestamp));

		return push();
	}
	Message<Array<MagneticFluxDensityData, N>> push() override {
		if (++it != reader.end()) {
			Message<Array<MagneticFluxDensityData, N>> ret;

			auto timestamp = (*it)["timestamp"].get<std::uint32_t>();

			for (auto const& [i, out] : std::ranges::views::enumerate(ret)) {
				out = {(*it)[common::stringprint('x', i)].template get<double>(), (*it)[common::stringprint('y', i)].template get<double>(), (*it)[common::stringprint('z', i)].template get<double>()};
			}

			if (auto t_new = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(timestamp)); t_new < next) {
				_current_time = std::chrono::system_clock::now() + std::chrono::seconds(1);
				_sensor_time = next = t_new;

			} else {
				next = t_new;
			}

			std::this_thread::sleep_until(_current_time + (next - _sensor_time));

			ret.timestamp = timestamp;

			return ret;
		} else {
			std::this_thread::sleep_for(std::chrono::seconds(3));
			reader = csv::CSVReader(path.string());
			it = reader.begin();

			return push_once();
		}
	}
};

int main() {
	CSVSensorArrayReader<16> reader(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "LIS3MDL_ARRAY_Besprechungsraum" / "mag_data_LIS3MDL_ARRAY_2025Mar28_17h22min51s_ground_truth_data.txt");

	MagnetometerArrayMagneticFluxDensityDataPrinter printer;
	reader.synchronously_connect(printer);
	auto reader_thread = reader();

	std::this_thread::sleep_for(std::chrono::seconds(20));
}