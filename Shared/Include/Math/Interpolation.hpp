#pragma once

#include "Include.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::MATH {
	namespace INTERPOLATION {
		enum struct Type {
			LINEAR,
			EASE_OVERSHOOT,
			EASE_OUT,
			EASE_IN,
			EASE,
			POWER,
			CIRCULAR,
			EXPONENTIAL
		};

		template <typename Value, typename T>
		Value f_general(const Value& a, const Value& b, const T& t, const T& exponent = 3.0, const Type& type = Type::LINEAR) {
			T transform;
			switch (type) {
				case Type::LINEAR: {
					transform = t;
					break;
				}
				case Type::EASE_OVERSHOOT: {
					if (t < 0.5) {
						const T t_val = t * 2.0;
						transform = 0.5 * ((exponent + 1.0) * t_val * t_val * t_val - exponent * t_val * t_val);
					}
					else {
						const T t_val = t * 2.0 - 2.0;
						transform = 0.5 * ((exponent + 1.0) * t_val * t_val * t_val + exponent * t_val * t_val + 2.0);
					}
					break;
				}
				case Type::EASE_OUT: {
					transform = 1.0 - pow(1.0 - t, 3.0);
					break;
				}
				case Type::EASE_IN: {
					transform = t * t * t;
					break;
				}
				case Type::EASE: {
					if (t < 0.5) {
						transform = 4.0 * t * t * t;
					}
					else {
						transform = 1.0 - pow(-2.0 * t + 2.0, 3.0) / 2.0;
					}
					break;
				}
				case Type::POWER: {
					if (t < 0.5) {
						transform = pow(2.0 * t, exponent) / 2.0;
					}
					else {
						transform = 1.0 - pow(-2.0 * t + 2.0, exponent) / 2.0;
					}
					break;
				}
				case Type::CIRCULAR: {
					transform = (1.0 - sqrt(1.0 - t * t));
					break;
				}
				case Type::EXPONENTIAL: {
					transform = pow(2.0, 10.0 * (t - 1.0));
					break;
				}
			}
			return a + (b - a) * transform;
		}

		template <typename Value, typename T>
		Value f_cubicBezier(const Value& P0, const Value& P1, const Value& P2, const Value& P3, const T& t) {
			const T u = 1.0 - t;
			const T tt = t * t;
			const T uu = u * u;
			const T uuu = uu * u;
			const T ttt = tt * t;

			Value point = uuu * P0;        // (1-t)^3 * P0
			point += 3.0 * uu * t * P1;    // 3(1-t)^2 * t * P1
			point += 3.0 * u * tt * P2;    // 3(1-t) * t^2 * P2
			point += ttt * P3;             // t^3 * P3

			return point;
		}

		template <typename Value, typename T>
		Value f_catmullRom(const Value& P0, const Value& P1, const Value& P2, const Value& P3, const T& t) {
			const T t2 = t * t;
			const T t3 = t2 * t;

			return
				0.5 * ((2.0 * P1) +
				(-P0 + P2) * t +
				(2.0 * P0 - 5.0 * P1 + 4.0 * P2 - P3) * t2 +
				(-P0 + 3.0 * P1 - 3.0 * P2 + P3) * t3);
		}

		template <typename Value, typename T>
		Value f_bezier(const vector<Value>& control_points, const T& t) {
			if (control_points.size() == 1) {
				return control_points[0];
			}

			vector<Value> newPoints;
			for (uint64 i = 0; i < control_points.size() - 1; ++i) {
				Value interpolated = (1.0 - t) * control_points[i] + t * control_points[i + 1];
				newPoints.push_back(interpolated);
			}

			return f_bezier(newPoints, t);
		}
	}
}