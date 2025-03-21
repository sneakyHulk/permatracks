#pragma once
#include <array>
#include <boost/regex.hpp>

#include "Array.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "Processor.h"
#include "RawMagnetometerArrayOutput.h"

class RawMagnetometerArrayOutput2MagneticFluxDensityData : public Processor<RawMagnetometerArrayOutput, Message<Array<MagneticFluxDensityData, 16>>> {
	double const sensitivity;

   public:
	explicit RawMagnetometerArrayOutput2MagneticFluxDensityData(double const sensitivity_lsb_uT = 68.42) : sensitivity(sensitivity_lsb_uT) {}
	Message<Array<MagneticFluxDensityData, 16>> process(RawMagnetometerArrayOutput const& data) override {
		static boost::regex const array_regex(
		    R"((-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n(-?[0-9]+) (-?[0-9]+) (-?[0-9]+)\r\n)");

		boost::match_results<const char*> submatches;
		if (!boost::regex_match(data.string + 0, data.string + data.bytes_transferred, submatches, array_regex)) {
			common::println_warn_loc("no");
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