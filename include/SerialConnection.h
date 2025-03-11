#include <Pusher.h>
#include <RunnerSynchronous.h>
#include <common_output.h>

#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/regex.hpp>
#include <chrono>
#include <fstream>
#include <string>

#include "MagneticFluxDensityData.h"
#include "Message.h"

class SerialConnection : public Pusher<Message<MagneticFluxDensityData>> {
	std::string board_name;

   public:
	explicit SerialConnection(boost::asio::io_service& io, std::string const& port = "/dev/ttyACM0", unsigned int const baud_rate = 115200) : io(io), serial(io) {
		serial.open(port);
		serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
		serial.set_option(boost::asio::serial_port_base::character_size(8));
		serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
		serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
		serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
	}

	template <typename T>
	[[nodiscard]] int create(boost::match_results<boost::circular_buffer<char>::const_iterator>::string_type const& str) const requires std::is_same_v<T, int> {
		return std::stoi(str);
	}

	template <typename T>
	[[nodiscard]] double create(boost::match_results<boost::circular_buffer<char>::const_iterator>::string_type const& str) const requires std::is_same_v<T, double> {
		return std::stod(str);
	}

	template <typename T>
	[[nodiscard]] std::string create(boost::match_results<boost::circular_buffer<char>::const_iterator>::string_type const& str) const requires std::is_same_v<T, std::string> {
		return {str.begin(), str.end()};
	}

	template <typename... Types>
	std::tuple<Types...> read_data(boost::regex const& regex) {
		static std::array<char, 256> buf{};
		static boost::circular_buffer<char> buffer = boost::circular_buffer<char>(1024);

		boost::match_results<boost::circular_buffer<char>::const_iterator> match;
		boost::match_flag_type flags = boost::match_default;
		do {
			std::size_t const bytes_transferred = serial.read_some(boost::asio::buffer(buf), ec);
			buffer.insert(buffer.end(), buf.begin(), buf.begin() + bytes_transferred);

			if (ec) {
				common::println_critical_loc(ec.message());
			}
		} while (!boost::regex_search(buffer.cbegin(), buffer.cend(), match, regex));

		auto sequence = std::index_sequence_for<Types...>{};

		auto create_tuple = [this, &match]<std::size_t... I>(std::index_sequence<I...>) { return std::make_tuple(create<Types>(match.str(I + 1))...); };

		auto tmp = create_tuple(std::index_sequence_for<Types...>{});
		buffer.erase_begin(match.position() + match.length());
		return tmp;
	}

	Message<MagneticFluxDensityData> push_once() override {
		static boost::regex const heartbeat_regex(R"(Heartbeat: '([0-9a-zA-Z]+)')");

		while (true) {
			auto [board_name1] = read_data<std::string>(heartbeat_regex);
			auto [board_name2] = read_data<std::string>(heartbeat_regex);

			if (board_name1 == board_name2) {
				board_name = board_name1;

				break;
			}
			std::cout << board_name1 << " != " << board_name2 << std::endl;
		}

		std::cout << "Board: " << board_name << std::endl;

		return push();
	}
	Message<MagneticFluxDensityData> push() override {
		static boost::regex const mag_data_regex(R"(\[([0-9]+)\] ([0-9a-zA-Z]+): X=(-?[0-9]+\.[0-9]+), Y=(-?[0-9]+\.[0-9]+), Z=(-?[0-9]+\.[0-9]+).)");

		Message<MagneticFluxDensityData> data;

		std::tie(data.timestamp, data.src, data.x, data.y, data.z) = read_data<int, std::string, double, double, double>(mag_data_regex);

		return data;
	}

   private:
	boost::asio::serial_port serial;
	boost::asio::io_service& io;
	boost::system::error_code ec;
};