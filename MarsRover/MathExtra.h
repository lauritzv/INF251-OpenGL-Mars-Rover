#pragma once

class MathExtra
{
public:
	MathExtra() = default;
	~MathExtra() = default;

	template <class U>
	U clamp(const U& in, const U& min, const U& max)
	{
		float out;
		if (in < min) out = min;
		else if (in > max) out = max;
		else out = in;
		return out;
	}
};