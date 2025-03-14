#include <Pusher.h>
#include <RunnerSynchronous.h>

#include <boost/asio.hpp>
#include <string>

#include "MagneticFluxDensityData.h"
#include "MessagePrinter.h"
#include "MultiSensorMagneticFluxDensityDataFilePrinter.h"
#include "SerialConnection.h"

int main() {
	boost::asio::io_service io_service;

	SerialConnection conn(io_service, "/dev/ttyACM0", 115200);
	MessagePrinter<MagneticFluxDensityData> printer;
	MultiSensorMagneticFluxDensityDataFilePrinter file_printer;

	conn.synchronously_connect(printer);
	conn.synchronously_connect(file_printer);

	auto conn_thread = conn();

	std::this_thread::sleep_for(std::chrono::seconds(62));
}