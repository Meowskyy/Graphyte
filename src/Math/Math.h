#pragma once
#include <algorithm>

template <typename t>
class Math {
	static t Clamp(t value, t min, t max)
	{
		if (value < min) value = min;
		if (value > max) value = max;
		return value;
	}

	static t Clamp01(t value)
	{
		//if (value < 0) value = min;
		//if (value > 1) value = max;
		return Clamp(value, 0, 1);
	}
};