#pragma once

#include <Runner.h>
#include <RunnerSynchronous.h>

#include "Direction.h"
#include "Pack.h"
#include "Position.h"
#include <filesystem>

class PositionDirectionFilePrinter : RunnerSynchronous<Pack<Position, Direction>> {
   public:
	PositionDirectionFilePrinter() = default;

   private:
	void run_once(Pack<Position, Direction> const& data) override {

	}
	void run(Pack<Position, Direction> const& data) override { std::cout << data << std::endl; }
};