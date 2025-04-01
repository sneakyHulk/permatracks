#pragma once
#include <Runner.h>
#include <RunnerSynchronous.h>

#include "Direction.h"
#include "Pack.h"
#include "Position.h"

class PositionDirectionPrinter : public RunnerSynchronous<Pack<Position, Direction>> {
   public:
	PositionDirectionPrinter() = default;

   private:
	void run(Pack<Position, Direction> const& data) override {
		static auto n = 0;
		std::cout << n++ << ": ";
		std::cout << data << std::endl;
	}
};