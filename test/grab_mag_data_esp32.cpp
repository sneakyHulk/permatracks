#include "MagnetometerArrayMagneticFluxDensityDataFilePrinter.h"
#include "MagnetometerArrayMagneticFluxDensityDataPrinter.h"
#include "MagnetometerArraySerialConnection.h"
#include "RawMagnetometerArrayOutput2MagneticFluxDensityData.h"

int main() {
	MagnetometerArraySerialConnection conn("/dev/ttyUSB0", 115200);
	RawMagnetometerArrayOutput2MagneticFluxDensityData converter;
	MagnetometerArrayMagneticFluxDensityDataPrinter printer;
	MagnetometerArrayMagneticFluxDensityDataFilePrinter filePrinter;

	conn.asynchronously_connect(converter);
	converter.synchronously_connect(printer);
	converter.synchronously_connect(filePrinter);

	auto conn_thread = conn();
	auto converter_thread = converter();

	std::this_thread::sleep_for(std::chrono::seconds(60));
}