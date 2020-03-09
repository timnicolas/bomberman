#ifndef USEGLM_HPP_
#define USEGLM_HPP_

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"  // to print vect/mat with glm::to_string
#include <glm/gtx/quaternion.hpp>

// to use glm::vec as map key
// and add lerp function to glm
namespace glm {
	template <typename T>
	bool operator<(tvec3<T> const &lhs, tvec3<T> const &rhs) {
		return (lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y)
			|| (lhs.x < rhs.x && lhs.y < rhs.y)
			|| (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z < rhs.z));
	}

	template <typename T>
	bool operator==(tvec3<T> const &lhs, tvec3<T> const &rhs) {
		return (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z);
	}

	template <typename T>
	bool operator<(tvec2<T> const &lhs, tvec2<T> const &rhs) {
		return (lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y)
			|| (lhs.x < rhs.x && lhs.y < rhs.y));
	}

	template <typename T>
	bool operator==(tvec2<T> const &lhs, tvec2<T> const &rhs) {
		return (lhs.x == rhs.x && lhs.y == rhs.y);
	}

	template <typename T>
	tvec3<T>	lerp(tvec3<T> start, tvec3<T> end, float factor) {
		return (start + factor * (end - start));
	}

	template <typename T>
	tvec2<T>	lerp(tvec2<T> start, tvec2<T> end, float factor) {
		return (start + factor * (end - start));
	}
};  // namespace glm

#endif  // USEGLM_HPP_
