#include <fstream>
#include <iostream>

#include "EigenJsonUtils.h"

int main() {
	Eigen::VectorXd v(5);
	v << 1, std::numeric_limits<double>::quiet_NaN(), 3, std::numeric_limits<double>::quiet_NaN(), 5;

	Eigen::Vector<bool, -1> filtered = v.array().isNaN();

	std::cout << filtered.count() << std::endl;

	for (auto const [value, filter] : std::ranges::views::zip(v, filtered) | std::ranges::views::filter([](auto const &v) { return !std::get<1>(v); })) {
		std::cout << value << std::endl;
	}

	std::cout << filtered << std::endl;
	std::cout << v << std::endl;
}