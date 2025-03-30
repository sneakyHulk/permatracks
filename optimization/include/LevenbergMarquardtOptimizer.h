#pragma once

#include <common_output.h>

#include <ranges>
#include <unsupported/Eigen/LevenbergMarquardt>

#include "Array.h"
#include "Direction.h"
#include "MagneticFluxDensityData.h"
#include "Pack.h"
#include "Position.h"
#include "Processor.h"

extern "C" {
#include <sensor_array_dipol_model.h>
}

class LevenbergMarquardtOptimizer : public Processor<Array<MagneticFluxDensityData, 16>, Pack<Position, Direction>> {
	double const m1;
	Eigen::Vector<double, 8> init;

	struct DipolModelFunctor : public Eigen::DenseFunctor<double> {
		double const &m1;
		Eigen::Vector<double, 48> _residuum;

	   public:
		DipolModelFunctor(double const &m1, Array<MagneticFluxDensityData, 16> const &residuum) : Eigen::DenseFunctor<double>(8, 48), m1(m1) {
			for (auto const &[in, out] : std::ranges::views::zip(residuum, _residuum | std::ranges::views::adjacent<3> | std::ranges::views::stride(3))) {
				auto const &[inx, iny, inz] = in;

				out = std::tie(inx, iny, inz);
			}
		}

		int operator()(InputType const &x, ValueType &fvec) const {
			dipol_model(fvec.array().data(), nullptr, m1, x(0), x(1), x(2), x(3), x(4), x(5), x(6), x(7));

			fvec -= _residuum;

			return 0;
		}

		int df(InputType const &x, JacobianType &fjac) const {
			dipol_model_jacobian(nullptr, fjac.array().data(), m1, x(0), x(1), x(2), x(3), x(4), x(5), x(6), x(7));

			return 0;
		}
	};

   public:
	LevenbergMarquardtOptimizer(double m1, double x = 21e-2 / 2, double y = 21e-2 / 2, double z = 15e-2 / 2.) : m1(m1), init({x, y, z, 0, 0, 0, 0, 0}) {}

	Pack<Position, Direction> process(Array<MagneticFluxDensityData, 16> const &data) override {
		DipolModelFunctor functor(m1, data);

		Eigen::LevenbergMarquardt<DipolModelFunctor> lm(functor);
		Eigen::Vector<double, Eigen::Dynamic> result = init;
		auto status = lm.minimize(result);

		switch (status) {
			case Eigen::LevenbergMarquardtSpace::NotStarted: common::println_critical_loc("LevenbergMarquardtSpace::NotStarted"); break;
			case Eigen::LevenbergMarquardtSpace::Running: common::println_critical_loc("LevenbergMarquardtSpace::Running"); break;
			case Eigen::LevenbergMarquardtSpace::ImproperInputParameters: common::println_critical_loc("LevenbergMarquardtSpace::ImproperInputParameters"); break;
			case Eigen::LevenbergMarquardtSpace::RelativeReductionTooSmall:
			case Eigen::LevenbergMarquardtSpace::RelativeErrorTooSmall:
			case Eigen::LevenbergMarquardtSpace::RelativeErrorAndReductionTooSmall:
			case Eigen::LevenbergMarquardtSpace::CosinusTooSmall:
			case Eigen::LevenbergMarquardtSpace::TooManyFunctionEvaluation:
			case Eigen::LevenbergMarquardtSpace::FtolTooSmall:
			case Eigen::LevenbergMarquardtSpace::XtolTooSmall:
			case Eigen::LevenbergMarquardtSpace::GtolTooSmall:
			case Eigen::LevenbergMarquardtSpace::UserAsked:
			default: return {result(0), result(1), result(2), result(3), result(4)};
		}
	}
};
