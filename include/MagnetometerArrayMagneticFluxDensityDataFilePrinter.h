#pragma once
#include <common.h>
#include <common_output.h>

#include <filesystem>
#include <fstream>

#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "RunnerSynchronous.h"

class MagnetometerArrayMagneticFluxDensityDataFilePrinter : public RunnerSynchronous<Message<std::array<MagneticFluxDensityData, 16>>> {
	std::ofstream file;

   public:
	MagnetometerArrayMagneticFluxDensityDataFilePrinter() = default;
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