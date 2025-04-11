#pragma once

#include <common_output.h>
#include <dipol_model.h>

#include <ranges>
#include <unsupported/Eigen/LevenbergMarquardt>

#include "AfterReturnTimeMeasure.h"
#include "Array.h"
#include "Direction.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "Pack.h"
#include "Position.h"
#include "Processor.h"
#include "linspace.h"

template <std::size_t N>
class MultiStartLevenbergMarquardtOptimizer : public Processor<Message<Array<MagneticFluxDensityData, N>>, Pack<Position, Direction>> {
	double const m1;

	struct DipolModelFunctor : public Eigen::DenseFunctor<double> {
		double const &m1;
		Eigen::Vector<double, 3 * N> _residuum;
		Eigen::Array<bool, 3 * N, 1> _mask;

	   public:
		DipolModelFunctor(double const &m1, Eigen::Vector<double, 3 * N> const &residuum, Eigen::Array<bool, 3 * N, 1> const &mask) : Eigen::DenseFunctor<double>(8, mask.count()), m1(m1), _residuum(residuum), _mask(mask) {}

		int operator()(InputType const &x, ValueType &Fvec) const {
			Eigen::Vector<double, 3 * N> F;
			dipol_model(F.array().data(), m1, x(0), x(1), x(2), x(3), x(4), x(5), x(6), x(7));

			F -= _residuum;

			F *= 5e4;  // scale such that F is around 1e0.

			for (auto const [i, j] : std::views::enumerate(std::ranges::views::zip(_mask, std::ranges::views::iota(0)) | std::ranges::views::filter([](auto const &pair) { return std::get<0>(pair); }) |
			                                               std::ranges::views::transform([](auto const &pair) { return std::get<1>(pair); }))) {
				Fvec(i) = F(j);
			}

			return 0;
		}

		int df(InputType const &x, JacobianType &Fjac) const {
			Eigen::Matrix<double, 3 * N, 8, Eigen::RowMajor> J;  // Eigen defaults to Column Major
			dipol_model_jacobian(J.array().data(), m1, x(0), x(1), x(2), x(3), x(4));

			J *= 5e4;  // scale such that J is around 1e0.

			for (auto const [i, j] : std::views::enumerate(std::ranges::views::zip(_mask, std::ranges::views::iota(0)) | std::ranges::views::filter([](auto const &pair) { return std::get<0>(pair); }) |
			                                               std::ranges::views::transform([](auto const &pair) { return std::get<1>(pair); }))) {
				Fjac.row(i) = J.row(j);
			}

			return 0;
		}
	};

   public:
	MultiStartLevenbergMarquardtOptimizer(double m1) : m1(m1) {}

	Pack<Position, Direction> process(Message<Array<MagneticFluxDensityData, 16>> const &data) override {
		AfterReturnTimeMeasure measure(data.timestamp);

		Eigen::Vector<double, 3 * N> residuum;
		for (auto const &[in, out] : std::ranges::views::zip(data, residuum | std::ranges::views::adjacent<3> | std::ranges::views::stride(3))) {
			auto const &[inx, iny, inz] = in;

			out = std::tie(inx, iny, inz);
		}

		Eigen::Array<bool, 3 * N, 1> mask = !residuum.array().isNaN();

		DipolModelFunctor functor(m1, residuum, mask);

		Eigen::Vector<double, Eigen::Dynamic> result(8);

		Eigen::Vector<double, Eigen::Dynamic> smallest_result(8);
		smallest_result.fill(std::numeric_limits<double>::max());
		double smallest_squared_error = std::numeric_limits<double>::max();

		for (auto const x_y_z_theta_phi : std::ranges::views::cartesian_product(linspace<4>(0, 21e-2), linspace<4>(0, 21e-2), linspace<4>(0, 15e-2), linspace_exclusive<4>(-180. * std::numbers::pi / 180., 180. * std::numbers::pi / 180.),
		         linspace_exclusive<4>(-180. * std::numbers::pi / 180., 180. * std::numbers::pi / 180.))) {
			Eigen::LevenbergMarquardt<DipolModelFunctor> lm(functor);
			lm.setMaxfev(1000000);

			result.fill(0.0);
			std::tie(result(0), result(1), result(2), result(3), result(4)) = x_y_z_theta_phi;

			switch (auto status = lm.minimize(result)) {
				// case Eigen::LevenbergMarquardtSpace::NotStarted: common::println_critical_loc("LevenbergMarquardtSpace::NotStarted"); break;
				// case Eigen::LevenbergMarquardtSpace::Running: common::println_critical_loc("LevenbergMarquardtSpace::Running"); break;
				// case Eigen::LevenbergMarquardtSpace::ImproperInputParameters: common::println_critical_loc("LevenbergMarquardtSpace::ImproperInputParameters"); break;
				// case Eigen::LevenbergMarquardtSpace::RelativeReductionTooSmall: common::println_warn_loc("LevenbergMarquardtSpace::RelativeReductionTooSmall"); break;
				// case Eigen::LevenbergMarquardtSpace::RelativeErrorAndReductionTooSmall: common::println_warn_loc("LevenbergMarquardtSpace::RelativeErrorAndReductionTooSmall"); break;
				// case Eigen::LevenbergMarquardtSpace::CosinusTooSmall: common::println_warn_loc("LevenbergMarquardtSpace::CosinusTooSmall"); break;
				// case Eigen::LevenbergMarquardtSpace::TooManyFunctionEvaluation: common::println_warn_loc("LevenbergMarquardtSpace::TooManyFunctionEvaluation"); break;
				// case Eigen::LevenbergMarquardtSpace::FtolTooSmall: common::println_warn_loc("LevenbergMarquardtSpace::FtolTooSmall"); break;
				// case Eigen::LevenbergMarquardtSpace::XtolTooSmall: common::println_warn_loc("LevenbergMarquardtSpace::XtolTooSmall"); break;
				// case Eigen::LevenbergMarquardtSpace::GtolTooSmall: common::println_warn_loc("LevenbergMarquardtSpace::GtolTooSmall"); break;
				// case Eigen::LevenbergMarquardtSpace::UserAsked: common::println_warn_loc("LevenbergMarquardtSpace::UserAsked"); break;
				// case Eigen::LevenbergMarquardtSpace::RelativeErrorTooSmall:;
				default: break;
			}

			typename DipolModelFunctor::ValueType residuals(48);
			functor(result, residuals);
			if (smallest_squared_error > residuals.squaredNorm()) {
				smallest_squared_error = residuals.squaredNorm();
				smallest_result = result;
			}
		}

		return {smallest_result(0), smallest_result(1), smallest_result(2), smallest_result(3), smallest_result(4)};
	}
};
