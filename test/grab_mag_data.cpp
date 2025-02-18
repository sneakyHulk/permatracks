#include <Pusher.h>
#include <RunnerSynchronous.h>

#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/regex.hpp>
#include <chrono>

struct MagneticFluxDensityData {
	float x;
	float y;
	float z;
};

template <typename Type>
class SerialConnection : public Pusher<Type> {
   public:
	explicit SerialConnection(boost::asio::io_service& io, std::string const& port = "/dev/ttyACM0", unsigned int baud_rate = 115200) : io(io), serial(io) {
		buffer.resize(1024);

		serial.open(port);
		serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
		serial.set_option(boost::asio::serial_port_base::character_size(8));
		serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
		serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
		serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
	}

	Type push() override {
		static boost::regex const mag_data_regex(R"(X: (-?[0-9]+\.[0-9]+) \tY: (-?[0-9]+\.[0-9]+) \tZ: (-?[0-9]+\.[0-9]+) uTesla)");
		static std::array<char, 256> buf;
		// std::sscanf(buffer.substr(0, bytes_transferred).c_str(), " X: %f  Y: %f  Z:  %f  ", &data.x, &data.y, &data.z) == 3

		boost::match_results<boost::circular_buffer<char>::const_iterator> mag_data_match;
		boost::match_flag_type flags = boost::match_default;
		do {
			std::size_t bytes_transferred = serial.read_some(boost::asio::buffer(buf), ec);
			buffer.insert(buffer.end(), buf.begin(), buf.begin() + bytes_transferred);

			if (ec) {
				std::cerr << "Error: " << ec.message() << std::endl;
			}
		} while (!boost::regex_search(buffer.cbegin(), buffer.cend(), mag_data_match, mag_data_regex));

		MagneticFluxDensityData data{std::stof(mag_data_match.str(1)), std::stof(mag_data_match.str(2)), std::stof(mag_data_match.str(3))};
		buffer.erase_begin(mag_data_match.position() + mag_data_match.length());
		return data;
	}

   private:
	boost::asio::serial_port serial;
	boost::asio::io_service& io;
	boost::circular_buffer<char> buffer = boost::circular_buffer<char>(1024);
	boost::system::error_code ec;
};

class Printer : public RunnerSynchronous<MagneticFluxDensityData> {
	std::chrono::time_point<std::chrono::system_clock> timestamp;

   public:
	Printer() : timestamp(std::chrono::system_clock::now()) {}
	void run(MagneticFluxDensityData const& data) override {
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = now - timestamp;
		timestamp = now;

		std::cout << 1 / diff.count() << "hz " << data.x << ", " << data.y << ", " << data.z << std::endl;
	}
};

int main() {
	boost::asio::io_service io_service;

	// std::regex mag_data_regex(R"(X: (-?[0-9]+\.[0-9]+) \tY: (-?[0-9]+\.[0-9]+) \tZ: (-?[0-9]+\.[0-9]+) uTesla)");

	SerialConnection<MagneticFluxDensityData> conn(io_service, "/dev/ttyACM0", 115200);
	Printer printer;

	conn.synchronously_connect(printer);

	auto conn_thread = conn();

	// io_service.run_for(std::chrono::seconds(100));

	std::this_thread::sleep_for(std::chrono::seconds(100));
}