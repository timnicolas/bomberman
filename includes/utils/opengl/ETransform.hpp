#ifndef ETRANSFORM_HPP_
#define ETRANSFORM_HPP_

#define WORLD_UP glm::vec3(0, 1, 0)
#define GLM_QUAT_IDENTITY glm::quat(1, 0, 0, 0)

#include "useGlm.hpp"

/**
 * @brief Class to manage model transformation
 *
 */
class ETransform {
	public:
		// -- constructors -----------------------------------------------------
		ETransform(glm::vec3 pos = {0, 0, 0}, glm::vec3 scale = {1.0, 1.0, 1.0},
			glm::quat rotQ = GLM_QUAT_IDENTITY);
		ETransform(glm::vec3 pos, glm::vec3 scale, glm::mat4 rotM);
		ETransform(glm::vec3 pos, glm::vec3 scale, float rotAngle,
			glm::vec3 rotAxis = WORLD_UP);
		virtual ~ETransform();
		ETransform(ETransform const &src);
		ETransform &operator=(ETransform const &rhs);

		// -- setters ----------------------------------------------------------
		void	setPos(glm::vec3 const pos);
		void	setRot(glm::quat const rot);
		void	setRot(glm::mat4 const rotM);
		void	setRot(float const rotAngle, glm::vec3 const rotAxis = WORLD_UP);
		void	setScale(glm::vec3 const scale);

		// -- getters ----------------------------------------------------------
		glm::vec3	getPos() const;
		glm::quat	getRot() const;
		glm::vec3	getScale() const;
		glm::mat4	getModel() const;

	private:
		void	_updateModel();

		glm::vec3	_pos;  /**< Position */
		glm::quat	_rot;  /**< Rotation */
		glm::vec3	_scale;  /**< Scale */
		glm::mat4	_model;  /**< Contain the final transformation matrix */
};

#endif  // ETRANSFORM_HPP_
