#pragma once

#include <Processor.h>
#include <common_output.h>

#include <numbers>
#include <semaphore>

#include "Array.h"
#include "Direction.h"
#include "MagneticFluxDensityData.h"
#include "Message.h"
#include "Pack.h"
#include "Position.h"

class UpdaterMagnetometerArrayMagneticFluxDensityDataToMagnetometerArrayMagneticFluxDensityDataWithTruePositionDirection
    : public Processor<Message<Array<MagneticFluxDensityData, 16> >, Pack<Message<Array<MagneticFluxDensityData, 16> >, Position, Direction> > {
	Position position;
	Direction direction;
	std::binary_semaphore data_semaphore{1};

   public:
	UpdaterMagnetometerArrayMagneticFluxDensityDataToMagnetometerArrayMagneticFluxDensityDataWithTruePositionDirection(Position &&position, Direction &&direction)
	    : position(std::forward<decltype(position)>(position)), direction(std::forward<decltype(direction)>(direction)) {}

	void update(Position &&position_, Direction &&direction_) {
		data_semaphore.acquire();

		position = position_;
		direction = direction_;

		data_semaphore.release();
	}

   private:
	Pack<Message<Array<MagneticFluxDensityData, 16> >, Position, Direction> process(Message<Array<MagneticFluxDensityData, 16> > const &data) override {
		Pack<Message<Array<MagneticFluxDensityData, 16> >, Position, Direction> out;

		static auto assign = []<typename Type>(Type *const out, Type const *const data) { *out = *data; };

		assign.template operator()<Message<Array<MagneticFluxDensityData, 16> > >(&out, &data);

		data_semaphore.acquire();

		assign.template operator()<Position>(&out, &position);
		assign.template operator()<Direction>(&out, &direction);

		data_semaphore.release();

		return out;
	}
};

Position make_position(char holes_facing, int x_pos, int y_pos, bool in_positive_direction, double height) {
	if (holes_facing == 'x') {
		return Position(37.5e-3 + (x_pos - 1) * 15e-3, 30e-3 + (y_pos - 1) * 15e-3, height);
	}
	if (holes_facing == 'y') {
		return Position(30e-3 + (x_pos - 1) * 15e-3, 37.5e-3 + (y_pos - 1) * 15e-3, height);
	}
	if (holes_facing == 'z') {
		return Position(30e-3 + (x_pos - 1) * 15e-3, 30e-3 + (y_pos - 1) * 15e-3, height);
	}

	common::println_critical_loc("Wrong input");
	throw;
}

Direction make_direction(char holes_facing, int x_pos, int y_pos, bool in_positive_direction) {
	if (holes_facing == 'x') {
		return Direction(in_positive_direction ? 90. * std::numbers::pi / 180. : -90. * std::numbers::pi / 180., 0.);
	}
	if (holes_facing == 'y') {
		return Direction(90. * std::numbers::pi / 180., in_positive_direction ? 90. * std::numbers::pi / 180. : -90. * std::numbers::pi / 180.);
	}
	if (holes_facing == 'z') {
		return Direction(in_positive_direction ? 0. : 180. * std::numbers::pi / 180., 0.);
	}

	common::println_critical_loc("Wrong input");
	throw;
}