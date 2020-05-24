#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <string>
#include <fstream>
#include <sstream>

#include "includesOpengl.hpp"

/**
 * @brief Shader class used to manage shader compilation
 *
 * It also adds some tools to set uniform and activate shader easier
 * Warning! before instantiating a Shader object you need to create the opengl contex with glfwCreateWindow
 */
class Shader {
	public:
		Shader(std::string const vsPath, std::string const fsPath, std::string const gsPath = "");
		Shader(Shader const &src);
		virtual ~Shader();

		Shader &operator=(Shader const &rhs);

		void	use();
		void	unuse();
		void	setBool(const std::string &name, bool value) const;
		void	setInt(const std::string &name, int value) const;
		void	setFloat(const std::string &name, float value) const;
		void	setDouble(const std::string &name, double value) const;

		void	setVec2(const std::string &name, float x, float y) const;
		void	setVec2(const std::string &name, const glm::vec2 &vec) const;
		void	setVec2Double(const std::string &name, double x, double y) const;
		void	setVec2Double(const std::string &name, const glm::tvec2<double> &vec) const;

		void	setVec3(const std::string &name, float x, float y, float z) const;
		void	setVec3(const std::string &name, const glm::vec3 &vec) const;
		void	setVec3Double(const std::string &name, double x, double y, double z) const;
		void	setVec3Double(const std::string &name, const glm::tvec3<double> &vec) const;

		void	setVec4(const std::string &name, float x, float y, float z, float w) const;
		void	setVec4(const std::string &name, const glm::vec4 &vec) const;
		void	setVec4Double(const std::string &name, double x, double y, double z, double w) const;
		void	setVec4Double(const std::string &name, const glm::tvec4<double> &vec) const;

		void	setMat2(const std::string &name, const glm::mat2 &mat) const;
		void	setMat2Double(const std::string &name, const glm::dmat2 &mat) const;
		void	setMat3(const std::string &name, const glm::mat3 &mat) const;
		void	setMat3Double(const std::string &name, const glm::dmat3 &mat) const;
		void	setMat4(const std::string &name, const glm::mat4 &mat) const;
		void	setMat4Double(const std::string &name, const glm::dmat4 &mat) const;

		/**
		 * @brief Shader exception
		 */
		class ShaderError : public std::exception {
			public:
				/**
				 * @brief Function auto called on errors
				 *
				 * @return const char* Error message
				 */
				virtual const char* what() const throw() = 0;
		};
		/**
		 * @brief Shader compilation exception
		 */
		class ShaderCompileException : public ShaderError {
			public:
				/**
				 * @brief Function auto called on errors
				 *
				 * @return const char* Error message
				 */
				virtual const char* what() const throw() {
					return ("Shader failed to compile!");
				}
		};
		/**
		 * @brief Shader linking exception
		 */
		class ShaderLinkingException : public ShaderError {
			public:
				/**
				 * @brief Function auto called on errors
				 *
				 * @return const char* Error message
				 */
				virtual const char* what() const throw() {
					return ("Shader program failed to link!");
				}
		};

		uint32_t	id;  /**< shader ID */

	private:
		void	checkCompileErrors(uint32_t shader, std::string type);

		std::string	_vsPath;
		std::string	_gsPath;
		std::string	_fsPath;
};

#endif  // SHADER_HPP_
