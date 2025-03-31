#pragma once

#include <Processor.h>

#include <Eigen/Dense>
#include <cstdint>
#include <ranges>
#include <vector>

#include "Array.h"
#include "Direction.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "Pack.h"
#include "Position.h"

template <std::size_t N>
class MagneticFluxDensityDataPositionDirectionTransformation : public Processor<Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>>, Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>>> {
	std::vector<Eigen::Matrix<double, 4, 4>> transformations;

   public:
	explicit MagneticFluxDensityDataPositionDirectionTransformation(std::vector<Eigen::Matrix<double, 4, 4>>&& transformations) : transformations(std::forward<decltype(transformations)>(transformations)) {}

	Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>> process(Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>> const& data) override {
		Message<Pack<Array<MagneticFluxDensityData, N>, Position, Direction>> out = data;
		for (auto const& [e, transformation] : std::ranges::views::zip(out, transformations)) {
			Eigen::Vector<double, 4> point{e.x, e.y, e.z, 1.0};
			Eigen::Vector<double, 4> transformed = transformation * point;

			e.x = transformed(0);
			e.y = transformed(1);
			e.z = transformed(2);
		}

		return out;
	}
};

template <std::size_t N>
class MagneticFluxDensityDataTransformation : public Processor<Message<Array<MagneticFluxDensityData, N>>, Message<Array<MagneticFluxDensityData, N>>> {
	std::vector<Eigen::Matrix<double, 4, 4>> transformations;

   public:
	explicit MagneticFluxDensityDataTransformation(std::vector<Eigen::Matrix<double, 4, 4>>&& transformations) : transformations(std::forward<decltype(transformations)>(transformations)) {}

	Message<Array<MagneticFluxDensityData, N>> process(Message<Array<MagneticFluxDensityData, N>> const& data) override {
		Message<Array<MagneticFluxDensityData, N>> out = data;
		for (auto const& [e, transformation] : std::ranges::views::zip(out, transformations)) {
			Eigen::Vector<double, 4> point{e.x, e.y, e.z, 1.0};
			Eigen::Vector<double, 4> transformed = transformation * point;

			e.x = transformed(0);
			e.y = transformed(1);
			e.z = transformed(2);
		}

		return out;
	}
};