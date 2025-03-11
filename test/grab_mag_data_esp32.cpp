#include <MagneticFluxDensityData.h>
#include <Message.h>
#include <common.h>
#include <common_output.h>

#include <boost/regex.hpp>
#include <fstream>
#include <regex>

#include "Pusher.h"
#include "serial/serial.h"

struct RawMagnetometerArrayOutput {
	std::size_t bytes_transferred;
	std::uint32_t timestamp;
	union {
		char string[512];
		std::array<std::uint8_t, 512> buffer;
	};
};

class MagnetometerArraySerialConnection : public Pusher<RawMagnetometerArrayOutput> {
	serial::Serial _serial;
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

   public:
	MagnetometerArraySerialConnection(std::string const& port = "/dev/ttyUSB0", unsigned int const baud_rate = 115200) : _serial(port, baud_rate, serial::Timeout(1, 80, 0, 0, 0)) {}
	RawMagnetometerArrayOutput push() override {
		RawMagnetometerArrayOutput out;

		out.bytes_transferred = _serial.read(out.buffer.data(), out.buffer.size());
		while (out.string[out.bytes_transferred - 1] != '\n') {
			std::cout << "no" << std::endl;
			_serial.setTimeout(1, 40, 0, 0, 0);
			out.bytes_transferred = _serial.read(out.buffer.data(), out.buffer.size());
			_serial.setTimeout(1, 80, 0, 0, 0);
			out.bytes_transferred = _serial.read(out.buffer.data(), out.buffer.size());
		}
		out.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();

		return out;
	}
};

class MagnetometerArrayOutput2MagneticFluxDensityIntegerData : public Processor<RawMagnetometerArrayOutput, Message<std::array<MagneticFluxDensityData, 16>>> {
	double const sensitivity;

   public:
	MagnetometerArrayOutput2MagneticFluxDensityIntegerData(double const sensitivity_lsb_uT = 68.42) : sensitivity(sensitivity_lsb_uT) {}
	Message<std::array<MagneticFluxDensityData, 16>> process(RawMagnetometerArrayOutput const& data) override {
		static boost::regex const array_regex(
		    R"((-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n)");

		boost::match_results<const char*> submatches;
		if (!boost::regex_match(data.string + 0, data.string + data.bytes_transferred, submatches, array_regex)) {
			std::cout << "no" << std::endl;
			throw std::invalid_argument("Invalid magnetometer data");
		}

		return {std::array<MagneticFluxDensityData, 16>{
		            MagneticFluxDensityData{std::stoi(submatches.str(1)) / sensitivity, std::stoi(submatches.str(2)) / sensitivity, std::stoi(submatches.str(3)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(4)) / sensitivity, std::stoi(submatches.str(5)) / sensitivity, std::stoi(submatches.str(6)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(7)) / sensitivity, std::stoi(submatches.str(8)) / sensitivity, std::stoi(submatches.str(9)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(10)) / sensitivity, std::stoi(submatches.str(11)) / sensitivity, std::stoi(submatches.str(12)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(13)) / sensitivity, std::stoi(submatches.str(14)) / sensitivity, std::stoi(submatches.str(15)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(16)) / sensitivity, std::stoi(submatches.str(17)) / sensitivity, std::stoi(submatches.str(18)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(19)) / sensitivity, std::stoi(submatches.str(20)) / sensitivity, std::stoi(submatches.str(21)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(22)) / sensitivity, std::stoi(submatches.str(23)) / sensitivity, std::stoi(submatches.str(24)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(25)) / sensitivity, std::stoi(submatches.str(26)) / sensitivity, std::stoi(submatches.str(27)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(28)) / sensitivity, std::stoi(submatches.str(29)) / sensitivity, std::stoi(submatches.str(30)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(31)) / sensitivity, std::stoi(submatches.str(32)) / sensitivity, std::stoi(submatches.str(33)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(34)) / sensitivity, std::stoi(submatches.str(35)) / sensitivity, std::stoi(submatches.str(36)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(37)) / sensitivity, std::stoi(submatches.str(38)) / sensitivity, std::stoi(submatches.str(39)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(40)) / sensitivity, std::stoi(submatches.str(41)) / sensitivity, std::stoi(submatches.str(42)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(43)) / sensitivity, std::stoi(submatches.str(44)) / sensitivity, std::stoi(submatches.str(45)) / sensitivity},
		            MagneticFluxDensityData{std::stoi(submatches.str(46)) / sensitivity, std::stoi(submatches.str(47)) / sensitivity, std::stoi(submatches.str(48)) / sensitivity},
		        },
		    data.timestamp, "LIS3MDL_ARRAY"};
	}
};

class Printer : public RunnerSynchronous<Message<std::array<MagneticFluxDensityData, 16>>> {
   public:
	Printer() = default;

	void run(Message<std::array<MagneticFluxDensityData, 16>> const& data) override {
		std::cout << data.timestamp;
		for (auto const& e : data) {
			std::cout << "," << e.x << "," << e.y << "," << e.z;
		}
		std::cout << std::endl;
	}
};

class FilePrinter : public RunnerSynchronous<Message<std::array<MagneticFluxDensityData, 16>>> {
	std::ofstream file;

   public:
	FilePrinter() = default;
	void run_once(Message<std::array<MagneticFluxDensityData, 16>> const& data) override {
		auto [ymd, hms] = common::get_year_month_day_hh_mm_ss();
		file.open(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / common::stringprint("mag_data_", data.src, "_", common::stringprint(ymd.year(), ymd.month(), ymd.day(), '_', hms.hours(), hms.minutes(), hms.seconds()), ".txt"));

		file << "timestamp,x0,y0,z0,x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,x5,y5,z5,x6,y6,z6,x7,y7,z7,x8,y8,z8,x9,y9,z9,x10,y10,z10,x11,y11,z11,x12,y12,z12,x13,y13,z13,x14,y14,z14,x15,y15,z15" << std::endl;
		return run(data);
	}
	void run(Message<std::array<MagneticFluxDensityData, 16>> const& data) override {
		file << data.timestamp;
		for (auto const& e : data) {
			file << "," << e.x << "," << e.y << "," << e.z;
		}
		file << std::endl;
	}
};

int main() {
	MagnetometerArraySerialConnection conn("/dev/ttyUSB0", 115200);
	MagnetometerArrayOutput2MagneticFluxDensityIntegerData converter;
	Printer printer;
	FilePrinter filePrinter;

	conn.asynchronously_connect(converter);
	converter.synchronously_connect(printer);
	converter.synchronously_connect(filePrinter);

	auto conn_thread = conn();
	auto converter_thread = converter();

	std::this_thread::sleep_for(std::chrono::seconds(60));
}