#pragma once

#include <common_output.h>

#include <ranges>

#include "Array.h"
#include "Averager.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"

class MagnetometerArrayMagneticFluxDensityDataAverager : public Averager<Message<Array<MagneticFluxDensityData, 16> > > {
   public:
	MagnetometerArrayMagneticFluxDensityDataAverager() : Averager(Message<Array<MagneticFluxDensityData, 16> >{}) {}

   private:
	[[nodiscard]] Message<Array<MagneticFluxDensityData, 16> > compute_add(Message<Array<MagneticFluxDensityData, 16> > const &sum, Message<Array<MagneticFluxDensityData, 16> > const &data) const override {
		Message<Array<MagneticFluxDensityData, 16> > out;

		for (auto const &[out, sum, data] : std::ranges::views::zip(out, sum, data)) {
			out.x = sum.x + data.x;
			out.y = sum.y + data.y;
			out.z = sum.z + data.z;
		}

		out.timestamp = data.timestamp;
		out.src = data.src;

		return out;
	}

	[[nodiscard]] Message<Array<MagneticFluxDensityData, 16> > compute_average(Message<Array<MagneticFluxDensityData, 16> > const &sum, std::size_t n) const override {
		Message<Array<MagneticFluxDensityData, 16> > out;

		for (auto const &[out, sum] : std::ranges::views::zip(out, sum)) {
			out.x = sum.x / static_cast<double>(n);
			out.y = sum.y / static_cast<double>(n);
			out.z = sum.z / static_cast<double>(n);
		}

		common::println("Avereage consists of ", n, " samples.");

		out.src += "_mean";

		return out;
	}
};