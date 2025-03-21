#include <Averager.h>
#include <common_output.h>

#include <ranges>
#include <semaphore>

#include "Array.h"
#include "Box.h"
#include "Direction.h"
#include "MagnetometerArrayMagneticFluxDensityDataFilePrinter.h"
#include "MagnetometerArrayMagneticFluxDensityDataPrinter.h"
#include "MagnetometerArraySerialConnection.h"
#include "Pack.h"
#include "Position.h"
#include "RawMagnetometerArrayOutput2MagneticFluxDensityData.h"

class MagnetometerArrayMagneticFluxDensityDataAverager2 : public Processor<Message<std::array<MagneticFluxDensityData, 16>>, Message<std::array<MagneticFluxDensityData, 16>>> {
	Message<std::array<MagneticFluxDensityData, 16>> out{};
	unsigned int n = 0;

   public:
	MagnetometerArrayMagneticFluxDensityDataAverager2() = default;

	Message<std::array<MagneticFluxDensityData, 16>> process(Message<std::array<MagneticFluxDensityData, 16>> const& data) override {
		for (auto i = 0; i < data.size(); ++i) {
			out[i].x += data[i].x;
			out[i].y += data[i].y;
			out[i].z += data[i].z;
		}

		++n;

		if (stop_token.stop_requested()) {
			for (auto& [x, y, z] : out) {
				x /= n;
				y /= n;
				z /= n;
			}

			out.timestamp = data.timestamp;
			out.src = data.src + "_mean";

			common::println("Avereage consists of ", n, " samples.");

			return out;
		}

		throw std::logic_error("still waiting");
	}
};

class MagnetometerArrayMagneticFluxDensityDataAverager : public Averager<Message<Array<MagneticFluxDensityData, 16>>> {
   public:
	MagnetometerArrayMagneticFluxDensityDataAverager() : Averager(Message<Array<MagneticFluxDensityData, 16>>{}) {}

   private:
	[[nodiscard]] Message<Array<MagneticFluxDensityData, 16>> compute_add(Message<Array<MagneticFluxDensityData, 16>> const& sum, Message<Array<MagneticFluxDensityData, 16>> const& data) const override {
		Message<Array<MagneticFluxDensityData, 16>> out;

		for (auto const& [out, sum, data] : std::ranges::views::zip(out, sum, data)) {
			out.x = sum.x + data.x;
			out.y = sum.y + data.y;
			out.z = sum.z + data.z;
		}

		out.timestamp = data.timestamp;
		out.src = data.src;

		return out;
	}

	[[nodiscard]] Message<Array<MagneticFluxDensityData, 16>> compute_average(Message<Array<MagneticFluxDensityData, 16>> const& sum, std::size_t n) const override {
		Message<Array<MagneticFluxDensityData, 16>> out;

		for (auto const& [out, sum] : std::ranges::views::zip(out, sum)) {
			out.x = sum.x / n;
			out.y = sum.y / n;
			out.z = sum.z / n;
		}

		out.src += "_mean";

		return out;
	}
};

class UpdaterMagnetometerArrayMagneticFluxDensityDataToMagnetometerArrayMagneticFluxDensityDataWithTruePositionDirection
    : public Processor<Message<Array<MagneticFluxDensityData, 16>>, Pack<Message<Array<MagneticFluxDensityData, 16>>, Position, Direction>> {
	Position position;
	Direction direction;
	std::binary_semaphore data_semaphore{1};

   public:
	UpdaterMagnetometerArrayMagneticFluxDensityDataToMagnetometerArrayMagneticFluxDensityDataWithTruePositionDirection(Position&& position, Direction&& direction)
	    : position(std::forward<decltype(position)>(position)), direction(std::forward<decltype(direction)>(direction)) {}

	void update(Position&& position_, Direction&& direction_) {
		data_semaphore.acquire();

		position = position_;
		direction = direction_;

		data_semaphore.release();
	}

   private:
	Pack<Message<Array<MagneticFluxDensityData, 16>>, Position, Direction> process(Message<Array<MagneticFluxDensityData, 16>> const& data) override {
		Pack<Message<Array<MagneticFluxDensityData, 16>>, Position, Direction> out;

		static auto assign = []<typename Type>(Type* const out, Type const* const data) { *out = *data; };

		assign.template operator()<Message<Array<MagneticFluxDensityData, 16>>>(&out, &data);

		data_semaphore.acquire();

		assign.template operator()<Position>(&out, &position);
		assign.template operator()<Direction>(&out, &direction);

		data_semaphore.release();

		return out;
	}
};

int main() {
	MagnetometerArrayMagneticFluxDensityDataAverager avg;
	UpdaterMagnetometerArrayMagneticFluxDensityDataToMagnetometerArrayMagneticFluxDensityDataWithTruePositionDirection updater(Position{1, 2, 3}, Direction{1, 2});
	MagnetometerArrayMagneticFluxDensityDataFilePrinterWithTruePositionDirection file_printer;

	avg.asynchronously_connect(updater);
	updater.synchronously_connect(file_printer);

	auto avg_thread = avg();
	auto updater_thread = updater();

	{
		common::print("Without magnet. Press ENTER to continue...");
		std::cin.ignore();
		common::println("Wait 5s for the next placement!");

		MagnetometerArraySerialConnection connection("/dev/ttyUSB0", 115200);
		RawMagnetometerArrayOutput2MagneticFluxDensityData converter;

		connection.asynchronously_connect(converter);
		converter.synchronously_connect(avg);

		auto conn_thread = connection();
		auto converter_thread = converter();

		std::this_thread::sleep_for(std::chrono::seconds(2));

		avg.average();
	}

	auto list = std::initializer_list<std::tuple<int, int, bool>>{
	    {1, 1, true},
	    {1, 1, false},
	};  //{10, 1, true}, {10, 1, false}, {10, 11, true}, {10, 11, false}, {1, 11, true}, {1, 11, false}, {5, 6, true}, {5, 6, false}};
	auto list2 = std::initializer_list<std::tuple<int, int, bool>>{{1, 1, true}, {1, 1, false}, {11, 1, true}, {11, 1, false}, {11, 10, true}, {11, 10, false}, {1, 10, true}, {1, 10, false}, {5, 6, true}, {5, 6, false}};
	auto list3 = std::initializer_list<std::tuple<int, int, bool>>{{1, 1, true}, {1, 1, true}, {11, 1, true}, {11, 1, true}, {11, 11, true}, {11, 11, true}, {1, 11, true}, {1, 11, true}, {6, 6, true}, {6, 6, true}};

	for (auto const [x_pos, y_pos, direction] : list) {
		updater.update(Position{static_cast<double>(x_pos), static_cast<double>(y_pos), 1}, Direction{static_cast<double>(x_pos), static_cast<double>(y_pos)});

		common::print("Place the magnet with its vessel with the nibble facing the ", direction ? "positive" : "negative", " x-direction on (", x_pos, ", ", y_pos, "). Press ENTER to continue...");
		std::cin.ignore();
		common::println("Wait 5s for the next placement!");

		MagnetometerArraySerialConnection connection("/dev/ttyUSB0", 115200);
		RawMagnetometerArrayOutput2MagneticFluxDensityData converter;

		connection.asynchronously_connect(converter);
		converter.synchronously_connect(avg);

		auto conn_thread = connection();
		auto converter_thread = converter();

		std::this_thread::sleep_for(std::chrono::seconds(2));

		avg.average();
	}
}