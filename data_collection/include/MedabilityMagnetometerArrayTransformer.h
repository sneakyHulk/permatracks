#pragma once

#include <Processor.h>

#include "Array.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"

template <std::size_t N>
class MedabilityMagnetometerArrayTransformer : public Processor<Message<Array<MagneticFluxDensityData, N>>, Message<Array<MagneticFluxDensityData, N>>> {
   public:
	MedabilityMagnetometerArrayTransformer() = default;

   private:
	Message<Array<MagneticFluxDensityData, N>> process(Message<Array<MagneticFluxDensityData, N>> const& data) override {
		Message<Array<MagneticFluxDensityData, N>> out = data;

		for (auto& e : out) {
			e.x *= 1e-6;
			e.y *= 1e-6;
			e.z *= 1e-6;

			std::swap(e.x, e.y);
		}

		return out;
	}
};