#pragma once

#include <cmath>

namespace Airheads {

template<typename ComponentType>
struct Vec2 {
	ComponentType x;
	ComponentType y;
};

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;

template<typename ComponentType>
struct Bounds2 {
	Vec2<ComponentType> min;
	Vec2<ComponentType> max;
};

using Bounds2i = Bounds2<int>;
using Bounds2f = Bounds2<float>;

template<typename ComponentType>
inline void ExpandToIncludePoint(Bounds2<ComponentType>& bounds, const Vec2<ComponentType>& pt) {
	if (bounds.min.x > pt.x)
		bounds.min.x = pt.x;

	if (bounds.min.y > pt.y)
		bounds.min.y = pt.y;

	if (bounds.max.x < pt.x)
		bounds.max.x = pt.x;

	if (bounds.max.y < pt.y)
		bounds.max.y = pt.y;
}

template<typename ComponentType>
inline Vec2<ComponentType> operator-(const Vec2<ComponentType>& lhs, const Vec2<ComponentType>& rhs) {
	return {lhs.x - rhs.x, lhs.y - rhs.y};
}

template<typename ComponentType>
inline ComponentType LengthSquared(const Vec2<ComponentType>& v) {
	return v.x * v.x + v.y * v.y;
}

template<typename ComponentType>
inline ComponentType Length(const Vec2<ComponentType>& v) {
	return (ComponentType) std::sqrt(LengthSquared(v));
}

template<>
inline float Length<float>(const Vec2<float>& v) {
	return std::sqrtf(LengthSquared(v));
}

template<>
inline int Length<int>(const Vec2<int>& v) {
	return (int)std::lround(std::sqrt(LengthSquared(v)));
}

template<typename ComponentType>
inline ComponentType Distance(const Vec2<ComponentType>& v1, const Vec2<ComponentType>& v2) {
	return Length(v1 - v2);
}

} // Airheads