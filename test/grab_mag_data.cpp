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
	std::string sensor_name;

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
			auto [sensor_name1] = read_data<std::string>(heartbeat_regex);
			auto [sensor_name2] = read_data<std::string>(heartbeat_regex);

			if (sensor_name1 == sensor_name2) {
				sensor_name = sensor_name1;

				break;
			} else {
				std::cout << sensor_name1 << " != " << sensor_name2 << std::endl;
			}
		}

		std::cout << "Sensor: " << sensor_name << std::endl;

		return push();
	}
	Message<MagneticFluxDensityData> push() override {
		static boost::regex const mag_data_regex(R"(\[([0-9]+)\] \tX: (-?[0-9]+\.[0-9]+) \tY: (-?[0-9]+\.[0-9]+) \tZ: (-?[0-9]+\.[0-9]+) uTesla)");

		Message<MagneticFluxDensityData> data;
		data.src = sensor_name;
		std::tie(data.timestamp, data.x, data.y, data.z) = read_data<int, double, double, double>(mag_data_regex);

		return data;
	}

   private:
	boost::asio::serial_port serial;
	boost::asio::io_service& io;
	boost::system::error_code ec;
};

class Printer : public RunnerSynchronous<Message<MagneticFluxDensityData>> {
	std::chrono::time_point<std::chrono::system_clock> timestamp;

   public:
	Printer() : timestamp(std::chrono::system_clock::now()) {}
	void run(Message<MagneticFluxDensityData> const& data) override { std::cout << data.timestamp << ": " << data.x << ", " << data.y << ", " << data.z << std::endl; }
};

class FilePrinter : public RunnerSynchronous<Message<MagneticFluxDensityData>> {
	std::ofstream file;

   public:
	FilePrinter() = default;
	void run_once(Message<MagneticFluxDensityData> const& data) override {
		auto const created = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}.get_local_time();
		auto const day = std::chrono::floor<std::chrono::days>(created);
		auto const second = std::chrono::floor<std::chrono::seconds>(created - day);
		std::chrono::hh_mm_ss const hms{second};
		std::chrono::year_month_day const ymd{day};

		auto date = common::stringprint(ymd.year(), ymd.month(), ymd.day(), '_', hms.hours(), hms.minutes(), hms.seconds());

		file.open(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / common::stringprint("mag_data_", data.src, '_', date, ".txt"));

		return run(data);
	}
	void run(Message<MagneticFluxDensityData> const& data) override { file << data << std::endl; }
};

int main() {
	boost::asio::io_service io_service;

	FilePrinter file_printer;
	SerialConnection conn(io_service, "/dev/ttyACM0", 115200);
	Printer printer;

	conn.synchronously_connect(printer);
	conn.synchronously_connect(file_printer);

	auto conn_thread = conn();

	std::this_thread::sleep_for(std::chrono::seconds(100));
}