#include "MagnetometerArrayMagneticFluxDensityDataFilePrinter.h"
#include "MagnetometerArrayMagneticFluxDensityDataPrinter.h"
#include "MagnetometerArraySerialConnection.h"
#include "RawMagnetometerArrayOutput2MagneticFluxDensityData.h"

class MagnetometerArrayMagneticFluxDensityDataAverager : public Processor<Message<std::array<MagneticFluxDensityData, 16>>, Message<std::array<MagneticFluxDensityData, 16>>> {
	Message<std::array<MagneticFluxDensityData, 16>> out{};
	unsigned int n = 0;

   public:
	MagnetometerArrayMagneticFluxDensityDataAverager() = default;

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

int main() {
	MagnetometerArrayMagneticFluxDensityDataFilePrinter file_printer;

	{
		common::print("Without magnet. Press ENTER to continue...");
		std::cin.ignore();
		common::println("Wait 5s for the next placement!");

		MagnetometerArraySerialConnection connection("/dev/ttyUSB0", 115200);
		RawMagnetometerArrayOutput2MagneticFluxDensityData converter;
		MagnetometerArrayMagneticFluxDensityDataAverager avg;

		connection.asynchronously_connect(converter);
		converter.asynchronously_connect(avg);
		avg.synchronously_connect(file_printer);

		auto conn_thread = connection();
		auto converter_thread = converter();
		auto avg_thread = avg();

		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

	auto list = std::initializer_list<std::tuple<int, int, bool>>{{1, 1, true}, {1, 1, false}, {10, 1, true}, {10, 1, false}, {10, 11, true}, {10, 11, false}, {1, 11, true}, {1, 11, false}, {5, 6, true}, {5, 6, false}};
	auto list2 = std::initializer_list<std::tuple<int, int, bool>>{{1, 1, true}, {1, 1, false}, {11, 1, true}, {11, 1, false}, {11, 10, true}, {11, 10, false}, {1, 10, true}, {1, 10, false}, {5, 6, true}, {5, 6, false}};
	auto list3 = std::initializer_list<std::tuple<int, int, bool>>{{1, 1, true}, {1, 1, true}, {11, 1, true}, {11, 1, true}, {11, 11, true}, {11, 11, true}, {1, 11, true}, {1, 11, true}, {6, 6, true}, {6, 6, true}};


	for (auto const [x_pos, y_pos, direction] : list) {
		common::print("Place the magnet with its vessel with the nibble facing the ", direction ? "positive" : "negative", " x-direction on (", x_pos, ", ", y_pos, "). Press ENTER to continue...");
		std::cin.ignore();
		common::println("Wait 5s for the next placement!");

		MagnetometerArraySerialConnection connection("/dev/ttyUSB0", 115200);
		RawMagnetometerArrayOutput2MagneticFluxDensityData converter;
		MagnetometerArrayMagneticFluxDensityDataAverager avg;

		connection.asynchronously_connect(converter);
		converter.asynchronously_connect(avg);
		avg.synchronously_connect(file_printer);

		auto conn_thread = connection();
		auto converter_thread = converter();
		auto avg_thread = avg();

		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}