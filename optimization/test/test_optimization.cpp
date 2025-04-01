#include <iostream>
#include <ranges>
#include <unsupported/Eigen/LevenbergMarquardt>

extern "C" {
#include <test_model.h>
}

struct Functor : public Eigen::DenseFunctor<double> {
	Eigen::Vector<double, 3> _residuum;
	Eigen::Array<bool, 3, 1> _mask;
	// Constructor
	explicit Functor(Eigen::Vector<double, 3> const &B, Eigen::Array<bool, 3, 1> const &mask) : Eigen::DenseFunctor<double>(3, mask.count()), _residuum(B), _mask(mask) {}

	// Number of residuals
	int operator()(InputType const &x, ValueType &Fvec) const {
		Eigen::Vector<double, 3> F;

		test_model(F.array().data(), nullptr, x(0), x(1), x(2));

		F -= _residuum;

		for (auto const [i, j] : std::views::enumerate(std::ranges::views::zip(_mask, std::ranges::views::iota(0)) | std::ranges::views::filter([](auto const &pair) { return std::get<0>(pair); }) |
		                                               std::ranges::views::transform([](auto const &pair) { return std::get<1>(pair); }))) {
			Fvec(i) = F(j);
		}

		return 0;
	}

	int df(InputType const &x, JacobianType &Fjac) const {
		Eigen::Matrix<double, 3, 3> J;
		test_model_jacobian(nullptr, J.array().data(), x(0), x(1), x(2));

		for (auto const [i, j] : std::views::enumerate(std::ranges::views::zip(_mask, std::ranges::views::iota(0)) | std::ranges::views::filter([](auto const &pair) { return std::get<0>(pair); }) |
		                                               std::ranges::views::transform([](auto const &pair) { return std::get<1>(pair); }))) {
			Fjac.row(i) = J.row(j);
		}

		return 0;
	}
};

int main() {
	Eigen::Vector<double, 3> B;
	B << 8., 8., 8.;
	Functor functor(B, Eigen::Array<bool, 3, 1>{true, true, true});
	Eigen::LevenbergMarquardt<Functor> lm(functor);

	lm.setXtol(1.0e-6);
	lm.setFtol(1.0e-6);

	Eigen::Vector<double, Eigen::Dynamic> x(3);
	x << 1.0, 1.0, 1.0;  // Initial guess

	auto status = lm.minimize(x);

	std::cout << "Solver status: " << status << std::endl;
	std::cout << "Solution: " << x.transpose() << std::endl;
}
