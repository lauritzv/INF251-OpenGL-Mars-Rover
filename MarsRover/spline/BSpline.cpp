#include "BSpline.h"
#include <algorithm>

void BSpline::_on_way_point_added()
{
	if(_way_points.size() < 4)
	{
		return;
	}

	const int new_control_point_index=static_cast<int>(_way_points.size()) - 1;

	const auto pt=new_control_point_index - 3;

	for(int i=0; i<=_steps; i++)
	{
		double u=static_cast<double>(i) / static_cast<double>(_steps);

		add_node(interpolate(u,
			_way_points[pt],
			_way_points[pt+1],
			_way_points[pt+2],
			_way_points[pt+3])
		);
	}
}

Vector BSpline::interpolate(double u, const Vector& P0,
	const Vector& P1, const Vector& P2, const Vector& P3)
{
	Vector point = u * u * u * ((-1) * P0 + 3 * P1 - 3 * P2 + P3) / 6;
	point+=u*u*(3*P0 - 6 * P1+ 3 * P2) / 6;
	point+=u*((-3) * P0 + 3 * P2) / 6;
	point+=(P0+4*P1+P2) / 6;

	return point;
}

Vector BSpline::getInterpolatedPosition
	(const double u, int const node0index) {
	if (loop)
	{
		return interpolate(u,
			node(node0index),
			node((node0index + 1) % node_count()),
			node((node0index + 2) % node_count()),
			node((node0index + 3) % node_count())
		);
	}
	else
	{
		return interpolate(u,
			node(node0index),
			node(std::min(node0index + 1, _steps - 1)),
			node(std::min(node0index + 2, _steps - 1)),
			node(std::min(node0index + 3, _steps - 1))
		);
	}
}


