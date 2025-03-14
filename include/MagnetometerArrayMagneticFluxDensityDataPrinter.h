#pragma once

#include <array>
#include <iostream>

#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "RunnerSynchronous.h"

class MagnetometerArrayMagneticFluxDensityDataPrinter : public RunnerSynchronous<Message<std::array<MagneticFluxDensityData, 16>>> {
   public:
	MagnetometerArrayMagneticFluxDensityDataPrinter() = default;

	void run(Message<std::array<MagneticFluxDensityData, 16>> const& data) override {
		std::cout << data.timestamp;
		for (auto const& e : data) {
			std::cout << "," << e.x << "," << e.y << "," << e.z;
		}
		std::cout << std::endl;
	}
};