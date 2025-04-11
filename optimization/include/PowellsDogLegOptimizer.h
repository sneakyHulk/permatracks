#pragma once

#include <common_output.h>

#include <ranges>
#include <unsupported/Eigen/NonLinearOptimization>

#include "AfterReturnTimeMeasure.h"
#include "Array.h"
#include "Direction.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "Pack.h"
#include "Position.h"
#include "Processor.h"

#include <dipol_model.h>


template <std::size_t N>
class PowellsDogLegOptimizer : public Processor<Message<Array<MagneticFluxDensityData, N>>, Pack<Position, Direction>> {
	double const m1;

   public:
	PowellsDogLegOptimizer(double m1, double x = 21e-2 / 2, double y = 21e-2 / 2, double z = 15e-2 / 2.) : m1(m1) {}

	Pack<Position, Direction> process(Message<Array<MagneticFluxDensityData, 16>> const &data) override { throw; }
};
