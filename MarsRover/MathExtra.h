#pragma once

class MathExtra
{
public:

	template <class U>
	static U clamp(const U& in, const U& min, const U& max)
	{
		float out;
		if (in < min) out = min;
		else if (in > max) out = max;
		else out = in;
		return out;
	}
};