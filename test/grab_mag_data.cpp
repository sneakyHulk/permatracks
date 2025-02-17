#include <Pusher.h>

#include <boost/asio.hpp>
#include <chrono>
#include <regex>

struct MagneticFluxDensityData {
	float x;
	float y;
	float z;
};

template <typename Type>
class SerialConnection : public Pusher<Type> {
   public:
	explicit SerialConnection(boost::asio::io_service& io, std::string const& port = "/dev/ttyACM0", unsigned int baud_rate = 115200) : io(io), serial(io) {
		serial.open(port);
		serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
		serial.set_option(boost::asio::serial_port_base::character_size(8));
		serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
		serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
		serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
	}

	Type push() override {
		while (true)
		boost::asio::read_until(serial, buffer, '\n', [this](boost::system::error_code ec, std::size_t length) {

		});
		return {};
	}

   private:
	boost::asio::serial_port serial;
	boost::asio::io_service& io;
	boost::asio::streambuf buffer;
};

int main() {
	boost::asio::io_service io_service;

	std::regex mag_data_regex(R"(X: (-?[0-9]+\.[0-9]+) \tY: (-?[0-9]+\.[0-9]+) \tZ: (-?[0-9]+\.[0-9]+) uTesla)");

	SerialConnection<MagneticFluxDensityData> conn(io_service, "/dev/ttyACM0", 115200);

	auto conn_thread = conn();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}