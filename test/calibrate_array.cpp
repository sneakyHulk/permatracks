#include <common_output.h>

#include "Direction.h"
#include "MagnetometerArrayMagneticFluxDensityDataAverager.h"
#include "MagnetometerArrayMagneticFluxDensityDataFilePrinter.h"
#include "MagnetometerArrayMagneticFluxDensityDataPrinter.h"
#include "MagnetometerArraySerialConnection.h"
#include "Position.h"
#include "RawMagnetometerArrayOutput2MagneticFluxDensityData.h"
#include "UpdaterMagnetometerArrayMagneticFluxDensityDataToMagnetometerArrayMagneticFluxDensityDataWithTruePositionDirection.h"

int main(int argc, char *argv[]) {
	MagnetometerArrayMagneticFluxDensityDataAverager avg;
	UpdaterMagnetometerArrayMagneticFluxDensityDataToMagnetometerArrayMagneticFluxDensityDataWithTruePositionDirection updater(Position{0, 0, 0}, Direction{0, 0});
	MagnetometerArrayMagneticFluxDensityDataFilePrinterWithTruePositionDirection file_printer;

	avg.asynchronously_connect(updater);
	updater.synchronously_connect(file_printer);

	auto avg_thread = avg();
	auto updater_thread = updater();
	{
		common::print("Without magnet. Press ENTER to continue...");
		std::cin.ignore();
		common::println("Wait 3s for the next placement!");

		MagnetometerArraySerialConnection connection("/dev/ttyUSB0", 115200);
		RawMagnetometerArrayOutput2MagneticFluxDensityData converter;

		connection.asynchronously_connect(converter);
		converter.synchronously_connect(avg);

		auto conn_thread = connection();
		auto converter_thread = converter();

		std::this_thread::sleep_for(std::chrono::seconds(3));

		avg.average();
	}

	auto calibration_pattern = std::initializer_list<std::tuple<char, int, int, bool> >{{'x', 1, 1, true}, {'x', 10, 1, false}, {'y', 11, 1, true}, {'y', 11, 10, false}, {'x', 10, 11, false}, {'x', 1, 11, true}, {'y', 1, 10, false},
	    {'y', 1, 1, true}, {'y', 6, 5, false}, {'x', 6, 6, true}, {'y', 6, 6, true}, {'x', 5, 6, false}, {'z', 1, 1, true}, {'z', 11, 1, true}, {'z', 11, 11, true}, {'z', 1, 11, true}, {'z', 6, 6, true}};

	for (auto const [holes_facing, x_pos, y_pos, in_positive_direction] : calibration_pattern) {
		updater.update(make_position(holes_facing, x_pos, y_pos, in_positive_direction, 11e-2), make_direction(holes_facing, x_pos, y_pos, in_positive_direction));

		common::print("Place the magnet with its vessel with the nibble facing the ", in_positive_direction ? "positive" : "negative", holes_facing, "-direction on (", x_pos, ", ", y_pos, "). Press ENTER to continue...");
		std::cin.ignore();
		common::println("Wait 3s for the next placement!");

		MagnetometerArraySerialConnection connection("/dev/ttyUSB0", 115200);
		RawMagnetometerArrayOutput2MagneticFluxDensityData converter;

		connection.asynchronously_connect(converter);
		converter.synchronously_connect(avg);

		auto conn_thread = connection();
		auto converter_thread = converter();

		std::this_thread::sleep_for(std::chrono::seconds(3));

		avg.average();
	}
}
