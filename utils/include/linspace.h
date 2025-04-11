#pragma once
#include <array>
#include <ranges>
#include <cstdint>

template <std::size_t N>
std::array<double, N> linspace(double from, double to) {
	std::array<double, N> result;

	for (double const step = (to - from) / (N - 1); auto const& [i, e] : std::ranges::views::enumerate(result)) {
		e = from + i * step;
	}

	return result;
}

template <std::size_t N>
std::array<double, N> linspace_exclusive(double from, double to) {
	std::array<double, N> result;

	for (double const step = (to - from) / N; auto const& [i, e] : std::ranges::views::enumerate(result)) {
		e = from + i * step;
	}

	return result;
}