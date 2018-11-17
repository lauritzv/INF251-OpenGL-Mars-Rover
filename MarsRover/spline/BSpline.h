#ifndef _H__BSPLINE_H
#define _H__BSPLINE_H

#include "Curve.h"

class BSpline : public Curve
{
public:
	BSpline() = default;
	virtual ~BSpline() = default;
	Vector getInterpolatedPosition(const double u, int node0index) override;

protected:
	Vector interpolate(double u, const Vector& P0, const Vector& P1, const Vector& P2, const Vector& P3);
	virtual void _on_way_point_added();
};

#endif