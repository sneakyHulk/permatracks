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
	std::filesystem::path _path;

	std::generator<Message<Array<MagneticFluxDensityData, N>>>* gen;
	decltype(gen->begin()) it;

	std::chrono::time_point<std::chrono::system_clock> _current_time;
	std::chrono::time_point<std::chrono::system_clock> _sensor_time;
	std::chrono::time_point<std::chrono::system_clock> next;

   public:
	explicit CSVSensorArrayReader(std::filesystem::path&& path) : _path(std::forward<decltype(path)>(path)), gen(new std::generator<Message<Array<MagneticFluxDensityData, N>>>(get(_path))), it(gen->begin()) {}

	static std::generator<Message<Array<MagneticFluxDensityData, N>>> get(std::filesystem::path path) {
		csv::CSVReader reader(path.string());

		for (auto const& row : reader) {
			Message<Array<MagneticFluxDensityData, N>> ret;

			for (auto const& [i, out] : std::ranges::views::enumerate(ret)) {
				out = {row[common::stringprint('x', i)].template get<double>(), row[common::stringprint('y', i)].template get<double>(), row[common::stringprint('z', i)].template get<double>()};
			}

			ret.timestamp = row["timestamp"].get<std::uint32_t>();
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
	~CSVSensorArrayReader() { delete gen; }
};

int main() {



	CSVSensorArrayReader<16> reader(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "LIS3MDL_ARRAY_Besprechungsraum" / "mag_data_LIS3MDL_ARRAY_2025Mar28_17h22min51s_ground_truth_data.txt");

	MagnetometerArrayMagneticFluxDensityDataPrinter printer;
	reader.synchronously_connect(printer);
	auto reader_thread = reader();

	std::this_thread::sleep_for(std::chrono::seconds(20));
}