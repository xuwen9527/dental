#ifndef __RAY_H__
#define __RAY_H__
#include <glm/glm.hpp>

namespace ray {
  bool intersect_box(glm::vec3 orig, glm::vec3 dir, glm::vec3 minp, glm::vec3 maxp, float& distHit);
  bool intersect_triangle(glm::vec3 orig, glm::vec3 dir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float& distHit);
}

#endif