#pragma once

namespace Airheads {

template<typename ComponentType>
struct Vec2 {
  ComponentType x;
  ComponentType y;
};

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;

} // Airheads