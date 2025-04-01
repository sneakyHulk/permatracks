#include <common_output.h>

#include <ranges>
#include <semaphore>

#include "Direction.h"
#include "MagnetometerArrayMagneticFluxDensityDataFilePrinter.h"
#include "MagnetometerArrayMagneticFluxDensityDataPrinter.h"
#include "MagnetometerArraySerialConnection.h"
#include "Position.h"
#include "RawMagnetometerArrayOutput2MagneticFluxDensityData.h"
#include "UpdaterMagnetometerArrayMagneticFluxDensityDataToMagnetometerArrayMagneticFluxDensityDataWithTruePositionDirection.h"

int main(int argc, char *argv[]) {
	UpdaterMagnetometerArrayMagneticFluxDensityDataToMagnetometerArrayMagneticFluxDensityDataWithTruePositionDirection updater(Position{0, 0, 0}, Direction{0, 0});
	MagnetometerArrayMagneticFluxDensityDataFilePrinterWithTruePositionDirection file_printer;

	updater.synchronously_connect(file_printer);

	auto updater_thread = updater();
	{
		common::print("Without magnet. Press ENTER to continue...");
		std::cin.ignore();
		common::println("Wait 3s for the next placement!");

		MagnetometerArraySerialConnection connection("/dev/ttyUSB0", 115200);
		RawMagnetometerArrayOutput2MagneticFluxDensityData converter;

		connection.asynchronously_connect(converter);
		converter.asynchronously_connect(updater);

		auto conn_thread = connection();
		auto converter_thread = converter();

		std::this_thread::sleep_for(std::chrono::seconds(3));
	}

	auto pattern = std::initializer_list<std::tuple<char, int, int, bool> >{{'x', 2, 2, true}, {'x', 9, 2, false}, {'y', 10, 2, true}, {'y', 10, 9, false}, {'x', 9, 10, false}, {'x', 2, 10, true}, {'y', 2, 9, false}, {'y', 2, 2, true},
	    {'y', 6, 5, false}, {'x', 6, 6, true}, {'y', 6, 6, true}, {'x', 5, 6, false}, {'z', 2, 2, true}, {'z', 10, 2, true}, {'z', 10, 10, true}, {'z', 2, 10, true}, {'z', 6, 6, true}};

	for (auto const [holes_facing, x_pos, y_pos, in_positive_direction] : pattern) {
		updater.update(make_position(holes_facing, x_pos, y_pos, in_positive_direction, 11e-2), make_direction(holes_facing, x_pos, y_pos, in_positive_direction));

		common::print("Place the magnet with its vessel with the nibble facing the ", in_positive_direction ? "positive" : "negative", holes_facing, "-direction on (", x_pos, ", ", y_pos, "). Press ENTER to continue...");
		std::cin.ignore();
		common::println("Wait 3s for the next placement!");

		MagnetometerArraySerialConnection connection("/dev/ttyUSB0", 115200);
		RawMagnetometerArrayOutput2MagneticFluxDensityData converter;

		connection.asynchronously_connect(converter);
		converter.asynchronously_connect(updater);

		auto conn_thread = connection();
		auto converter_thread = converter();

		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
}
