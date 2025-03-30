#pragma once

#include <array>
#include <iostream>

#include "Array.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "RunnerSynchronous.h"

class MagnetometerArrayMagneticFluxDensityDataPrinter : public RunnerSynchronous<Message<Array<MagneticFluxDensityData, 16>>> {
   public:
	MagnetometerArrayMagneticFluxDensityDataPrinter() = default;

	void run(Message<Array<MagneticFluxDensityData, 16>> const& data) override {
		std::cout << data.timestamp;
		for (auto const& e : data) {
			std::cout << "," << e.x << "," << e.y << "," << e.z;
		}
		std::cout << std::endl;
	}
};