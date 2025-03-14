#pragma once
#include <common_output.h>

#include <chrono>

#include "Pusher.h"
#include "RawMagnetometerArrayOutput.h"
#include "serial/serial.h"

class MagnetometerArraySerialConnection : public Pusher<RawMagnetometerArrayOutput> {
	serial::Serial _serial;
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

   public:
	MagnetometerArraySerialConnection(std::string const& port = "/dev/ttyUSB0", unsigned int const baud_rate = 115200) : _serial(port, baud_rate, serial::Timeout(1, 80, 0, 0, 0)) {}
	RawMagnetometerArrayOutput push() override {
		RawMagnetometerArrayOutput out;

		out.bytes_transferred = _serial.read(out.buffer.data(), out.buffer.size());
		while (out.string[out.bytes_transferred - 1] != '\n') {
			common::println_warn_loc("no");
			_serial.setTimeout(1, 40, 0, 0, 0);
			out.bytes_transferred = _serial.read(out.buffer.data(), out.buffer.size());
			_serial.setTimeout(1, 80, 0, 0, 0);
			out.bytes_transferred = _serial.read(out.buffer.data(), out.buffer.size());
		}
		out.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();

		return out;
	}
};