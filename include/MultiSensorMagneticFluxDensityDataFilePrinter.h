#pragma once

#include <common.h>
#include <common_output.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>

#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "RunnerSynchronous.h"

class MultiSensorMagneticFluxDensityDataFilePrinter : public RunnerSynchronous<Message<MagneticFluxDensityData>> {
	std::map<std::string, std::ofstream> files;

   public:
	MultiSensorMagneticFluxDensityDataFilePrinter() = default;
	void run(Message<MagneticFluxDensityData> const& data) override {
		if (!files.contains(data.src)) [[unlikely]] {
			auto [ymd, hms] = common::get_year_month_day_hh_mm_ss();
			files[data.src].open(
			    std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / common::stringprint("mag_data_", data.src, "_", common::stringprint(ymd.year(), ymd.month(), ymd.day(), '_', hms.hours(), hms.minutes(), hms.seconds()), ".txt"));
			files[data.src] << "timestamp,x,y,z" << std::endl;
		}

		files[data.src] << data << std::endl;
	}
};