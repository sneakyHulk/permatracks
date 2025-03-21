#pragma once

#include <cstdint>
#include <ostream>

template <typename Type>
struct Message : public Type {
	std::uint32_t timestamp;
	std::string src;
};

// template <typename Type>
// std::ostream& operator<<(std::ostream& os, Message<Type> const& msg);

template <typename Type>
std::ostream& operator<<(std::ostream& os, Message<Type> const& msg) {
	os << msg.timestamp << "," << *static_cast<Type const* const>(&msg);

	return os;
}