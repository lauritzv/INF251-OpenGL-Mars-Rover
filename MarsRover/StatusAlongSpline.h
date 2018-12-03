#pragma once
#include "SceneNode.h"
#include "spline/Curve.h"
#include <algorithm>

class StatusAlongSpline
{
public:
	Curve* curve;
	double movespeed;
	double phaseOffset;
	double position_along_path = 0.;
	Vector3f currentPos;
	Vector3f nextPos;
	bool loop;

	StatusAlongSpline() = default;

	StatusAlongSpline(Curve* curve, double movespeed, double phase_offset, bool loop)
		: curve(curve),
		  movespeed(movespeed),
		  phaseOffset(phase_offset),
		  loop(loop)
	{
		currentPos = PositionAlongPath(phase_offset);
		nextPos = currentPos;
		position_along_path = phaseOffset;
	}

	Vector3f PositionAlongPath(const double &delta_time) {
		const int nodeCount = curve->node_count();
		const double deltaMovement = delta_time * movespeed;
		position_along_path += deltaMovement;

		if (curve->loop) {
			if (position_along_path > curve->total_length())
				position_along_path -= curve->total_length();
		}
		else position_along_path = std::min(position_along_path, curve->total_length() - 1);

		const auto position_rescaled = position_along_path / curve->total_length() * curve->node_count();
		const auto node_number = static_cast<int>(position_rescaled);

		// interpolate the position between the pre-interpolated positions:
		const auto node_number_a_influence = position_rescaled - node_number;
		const auto interpolatedPos = curve->getInterpolatedPosition(node_number_a_influence, node_number);

		// from double Vector to Vector3f:
		return Vector3f(interpolatedPos.x, interpolatedPos.y, interpolatedPos.z);
	}
};
