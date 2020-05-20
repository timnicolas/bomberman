#ifndef MATERIAL_HPP_
#define MATERIAL_HPP_

#include <iostream>
#include "includesOpengl.hpp"

/**
 * @brief basic material for shaders
 */
class Material {
	public:
		Material(std::string const name = "",
			glm::vec3 const diffuse = glm::vec3(.36f, .34f, .32f), \
			glm::vec3 const specular = glm::vec3(.3f, .3f, .3f), \
			glm::vec3 const ambient = glm::vec3(0.2f, 0.2f, 0.2f), \
			float const shininess = 16.0f);
		Material(Material const &src);
		virtual ~Material();

		Material &operator=(Material const &rhs);

		std::string	name;  /**< Material name */
		glm::vec3	diffuse;  /**< Material diffuse */
		glm::vec3	specular;  /**< Material specular */
		glm::vec3	ambient;  /**< Material ambient */
		float		shininess;  /**< Material shininess */

	private:
};

std::ostream & operator << (std::ostream &out, Material const &c);

#endif  // MATERIAL_HPP_
