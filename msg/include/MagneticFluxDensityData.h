#pragma once

#include <ostream>

struct MagneticFluxDensityData {
	double x;
	double y;
	double z;
};

std::ostream& operator<<(std::ostream& os, MagneticFluxDensityData const& d);