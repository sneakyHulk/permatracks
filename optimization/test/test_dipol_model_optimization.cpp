#include "LevenbergMarquardtOptimizer.h"

int main() {
	Array<MagneticFluxDensityData, 16> residuum;
	Eigen::Vector<double, 48> _residuum;

	residuum[0].z = 5.0;

	residuum[15].x = 3.0;

	for (auto const &[in, out] : std::ranges::views::zip(residuum, _residuum | std::ranges::views::adjacent<3> | std::ranges::views::stride(3))) {
		auto const &[inx, iny, inz] = in;

		out = std::tie(inx, iny, inz);
	}

	std::cout << _residuum;

}