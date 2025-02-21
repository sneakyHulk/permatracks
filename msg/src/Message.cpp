#include "Message.h"

#include "MagneticFluxDensityData.h"

template <typename Type>
std::ostream& operator<<(std::ostream& os, Message<Type> const& msg) {
	os << msg.timestamp << *static_cast<Type*>(&msg);

	return os;
}

template class Message<MagneticFluxDensityData>;