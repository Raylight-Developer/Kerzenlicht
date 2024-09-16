#include "Object/Data/Curve.hpp"

KL::OBJECT::DATA::CURVE::Spline::Spline() {
	type = SPLINE::Type::POLY;
	points = vector<dvec3>();
}

KL::OBJECT::DATA::Curve::Curve() {
	splines = vector<CURVE::Spline*>();
	cpu_update = true;
}