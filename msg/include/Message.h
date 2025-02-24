#pragma once

#include <ostream>
#include <cstdint>

template <typename Type>
struct Message : public Type {
	std::uint32_t timestamp;
	std::string src;
};

template <typename Type>
std::ostream& operator<<(std::ostream& os, Message<Type> const& msg);