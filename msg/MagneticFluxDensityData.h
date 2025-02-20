#include <ostream>

struct MagneticFluxDensityData {
	float x;
	float y;
	float z;
};

std::ostream& operator<<(std::ostream& os, MagneticFluxDensityData const& d) {
	os << d.x << "," << d.y << "," << d.z;

	return os;
}