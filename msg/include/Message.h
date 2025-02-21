#pragma once

#include <iostream>
#include <cstdint>

template <typename Type>
struct Message : public Type {
	std::uint32_t timestamp;
};

template <typename Type>
std::ostream& operator<<(std::ostream& os, Message<Type> const& msg);