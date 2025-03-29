#pragma once

#include <ranges>

#include "Array.h"
#include "Direction.h"
#include "MagneticFluxDensityData.h"
#include "Pack.h"
#include "Position.h"
#include "Processor.h"
#include <common_output.h>

extern "C" {
#include <sensor_array_dipol_model.h>
}

class LevenbergMarquardtOptimizer : public Processor<Array<MagneticFluxDensityData, 16>, Pack<Position, Direction>> {
	struct DipolModelFunctor : public Eigen::DenseFunctor<double> {
		double m1;
		Eigen::Vector<double, 48> residuum;

		DipolModelFunctor(double m1, Array<MagneticFluxDensityData, 16> const &residuum) : Eigen::DenseFunctor<double>(8, 48), m1(m1) {
			for (auto const& e : std::ranges::views::zip(residuum, std::ranges::views::chunk(residuum, 3))) {

			}
		}

		int operator()(InputType const &x, ValueType &fvec) const {
			dipol_model(fvec.array().data(), nullptr, m1, x(0), x(1), x(2), x(3), x(4), x(5), x(6), x(7));

			fvec -= residuum;

			return 0;
		}

		int df(InputType const &x, JacobianType &fjac) const {
			dipol_model_jacobian(nullptr, fjac.array().data(), m1, x(0), x(1), x(2), x(3), x(4), x(5), x(6), x(7));

			return 0;
		}
	};

   public:
	LevenbergMarquardtOptimizer() = default;

	Pack<Position, Direction> process(Array<MagneticFluxDensityData, 16> const &data) {
		Pack<Position, Direction> out;

		return out;
	}
};
