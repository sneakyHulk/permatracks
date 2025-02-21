#pragma once

#include <ostream>

struct MagneticFluxDensityData {
	float x;
	float y;
	float z;
};

std::ostream& operator<<(std::ostream& os, MagneticFluxDensityData const& d);