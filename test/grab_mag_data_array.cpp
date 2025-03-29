#include "MagnetometerArrayMagneticFluxDensityDataFilePrinter.h"
#include "MagnetometerArrayMagneticFluxDensityDataPrinter.h"
#include "MagnetometerArraySerialConnection.h"
#include "RawMagnetometerArrayOutput2MagneticFluxDensityData.h"


auto pattern = std::initializer_list<std::tuple<char, int, int, bool> >{{'x', 2, 2, true}, {'x', 9, 2, false}, {'y', 10, 2, true}, {'y', 10, 9, false}, {'x', 9, 10, false}, {'x', 2, 10, true}, {'y', 2, 9, false},
		{'y', 2, 2, true}, {'y', 6, 5, false}, {'x', 6, 6, true}, {'y', 6, 6, true}, {'x', 5, 6, false}, {'z', 2, 2, true}, {'z', 10, 2, true}, {'z', 10, 10, true}, {'z', 2, 10, true}, {'z', 6, 6, true}};


int main() {
	MagnetometerArraySerialConnection conn("/dev/ttyUSB0", 115200);
	RawMagnetometerArrayOutput2MagneticFluxDensityData converter;
	MagnetometerArrayMagneticFluxDensityDataFilePrinter filePrinter;

	conn.asynchronously_connect(converter);
	converter.synchronously_connect(filePrinter);

	auto conn_thread = conn();
	auto converter_thread = converter();

	std::this_thread::sleep_for(std::chrono::seconds(3));
}