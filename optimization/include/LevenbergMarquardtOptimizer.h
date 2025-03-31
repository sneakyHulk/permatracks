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

template <std::size_t N>
class LevenbergMarquardtOptimizer : public Processor<Message<Array<MagneticFluxDensityData, N>>, Pack<Position, Direction>> {
	double const m1;
	Eigen::Vector<double, 8> init;

	struct DipolModelFunctor : public Eigen::DenseFunctor<double> {
		double const &m1;
		Eigen::Vector<double, 8 * N> _residuum;
		Eigen::Array<bool, 8 * N, 1> _mask;

	   public:
		DipolModelFunctor(double const &m1, Eigen::Vector<double, 8 * N> const &residuum, Eigen::Array<bool, 8 * N, 1> const &mask) : Eigen::DenseFunctor<double>(8, 8 * (N - mask.count())), m1(m1), _residuum(residuum), _mask(mask) {}

		int operator()(InputType const &x, ValueType &fvec) const {
			Eigen::Vector<double, 8 * N> f;
			dipol_model(f.array().data(), nullptr, m1, x(0), x(1), x(2), x(3), x(4), x(5), x(6), x(7));



			for (auto const [i, filtered] : std::ranges::views::zip(_mask, f - _residuum) | std::ranges::views::filter([](auto const &filter) { return !std::get<0>(filter); })) {
				auto [filter, values] = filtered;

				fvec(i) = values;
			}

			fvec -= _residuum;

			return 0;
		}

		int df(InputType const &x, JacobianType &fjac) const {
			Eigen::Matrix<double, 8 * N, 8> j;

			dipol_model_jacobian(nullptr, j.array().data(), m1, x(0), x(1), x(2), x(3), x(4), x(5), x(6), x(7));

			for (auto const [i, filtered] : std::ranges::views::zip(_mask, j) | std::ranges::views::filter([](auto const &filter) { return !std::get<0>(filter); })) {
				auto [filter, values] = filtered;

				fjac(i) = values;
			}

			return 0;
		}
	};

   public:
	LevenbergMarquardtOptimizer(double m1, double x = 21e-2 / 2, double y = 21e-2 / 2, double z = 15e-2 / 2.) : m1(m1), init({x, y, z, 0, 0, 0, 0, 0}) {}

	Pack<Position, Direction> process(Message<Array<MagneticFluxDensityData, 16>> const &data) override {
		Eigen::Vector<double, 8 * N> residuum;
		for (auto const &[in, out] : std::ranges::views::zip(data, residuum | std::ranges::views::adjacent<3> | std::ranges::views::stride(3))) {
			auto const &[inx, iny, inz] = in;

			out = std::tie(inx, iny, inz);
		}
		Eigen::Array<bool, 8 * N, 1> mask = residuum.array().isNaN();

		DipolModelFunctor functor(m1, residuum, mask);

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
