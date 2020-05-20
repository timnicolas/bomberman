#ifndef OPENGLMODEL_HPP_
#define OPENGLMODEL_HPP_

// maximum bones on one model
#define MAX_BONES 100

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <array>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <utility>  // for pair

#include <assimp/Importer.hpp>
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Gui.hpp"

namespace AnimKeyType {
	/**
	 * @brief Animation key type
	 */
	enum Enum {
		SCALE,
		ROTATION,
		POSITION
	};

	std::array<std::string, 3> const	enumName = {{
		"scale",
		"rotation",
		"position"
	}};
}  // namespace AnimKeyType

/**
 * @brief Vertices limits
 */
struct	VerticesLimits {
	float	xMin;  /**< The x min */
	float	xMax;  /**< The x max */
	float	yMin;  /**< The y min */
	float	yMax;  /**< The y max */
	float	zMin;  /**< The z min */
	float	zMax;  /**< The z max */

	VerticesLimits();
};

/**
 * @brief class to load and draw a 3d model from file
 *
 */
class OpenGLModel {
	public:
        OpenGLModel(Camera const &cam, std::string const &path, glm::vec3 offset = {0, 0, 0},
			bool centerEnabled = false, bool scaleEnabled = false);
		virtual ~OpenGLModel();
		OpenGLModel(OpenGLModel const &src);
		OpenGLModel &operator=(OpenGLModel const &rhs);

		void		draw(float animationTimeTick = 0.0f);
		bool		setAnimation(uint32_t id);
		void		setModel(glm::mat4 const model);
		void		setMeshTexture(TextureType::Enum type, std::string const meshName,
			std::string const path, bool inSpaceSRGB = true);
		void		printMeshesNames() const;

		// getters
		bool		getAnimationId(std::string const name, uint32_t &outId) const;
		aiAnimation	*getAiAnimation(uint32_t id) const;
		bool		isAnimated() const;
		std::vector<std::string>	getAnimationNames() const;
		uint32_t	getNbAnimations() const;
		Camera const	&getCam() const;
		std::string	getPath() const;

		// Exceptions
		/**
		 * @brief Model exception
		 */
		class ModelException : public std::runtime_error {
			public:
				ModelException();
				explicit ModelException(char const *what_arg);
		};

	private:
		OpenGLModel();  // private default constructor, should not be called

		void	_setConstUniforms();

		// -- model loading ----------------------------------------------------
		void	_loadModel();
		void	_processNode(aiNode *node, aiScene const *scene);
		void	_processMesh(aiMesh *aiMesh, aiScene const *scene);
		void	_loadMaterialTextures(std::vector<Texture> &textures,
			aiScene const *scene, aiMaterial *aiMat, aiTextureType type,
			TextureType::Enum textType);
		static Material	_loadMaterial(aiMaterial *aiMat);
		void	_processBones(aiMesh *aiMesh, Mesh &mesh);
		void	_updateMinMaxPos(glm::vec3 pos);
		void	_calcCenterScale();

		// -- animation bones calculation --------------------------------------
		void	_setBonesTransform(float animationTimeTick, aiNode *node,
			aiScene const *scene, glm::mat4 parentTransform);
		aiNodeAnim const	*_findNodeAnim(std::string const nodeName);

		// interpolate scaling
		glm::vec3	_calcInterpolatedScaling(float animationTimeTick,
			const aiNodeAnim* nodeAnim);
		// interpolate rotation
		glm::quat	_calcInterpolatedRotation(float animationTimeTick,
			aiNodeAnim const *nodeAnim);
		// interpolate position
		glm::vec3	_calcInterpolatedPosition(float animationTimeTick,
			aiNodeAnim const *nodeAnim);

		std::pair<uint32_t, uint32_t>	_findAnimIndex(AnimKeyType::Enum animType,
			float animationTimeTick, aiNodeAnim const *nodeAnim);

		// -- utils ------------------------------------------------------------
		void	_loadTexture(TextureType::Enum type, std::string const path,
			bool inSpaceSRGB);

		// -- members ----------------------------------------------------------
		static std::unique_ptr<Shader>	_sh;  /**< Shader */

		Camera const		&_cam;  /**< A reference to the camera */
		std::string const	_path;  /**< model file path */
		std::unordered_map<std::string, Mesh *>	_meshes;  /**< all model meshes */

		// assimp utility
		aiScene const		*_scene;  /**< The assimp scene */
		Assimp::Importer	_importer;  /**< The assimp importer */

		// textures
		std::unordered_map<std::string, Texture>	_texturesLoaded;  /**< to keep track of loaded textures */
		std::string									_pathDir;  /**< The path with textures */

		// model position
		glm::mat4	_model;  /**< position in real world */
		bool		_centerEnabled;  /**< center the model on loading */
		bool		_scaleEnabled;  /**< scale the model on loading */
		glm::vec3	_minPos, _maxPos;  /**< to scale the model */
		glm::mat4	_modelScale;  /**< The model scale */
		glm::vec3	_offset;  /**< offset to shift the model manually */

		// global transform matrix
		glm::mat4	_globalTransform;  /**< Global transform matrix */

		// -- animation related ------------------------------------------------
		bool	_isAnimated;  /**< Is the model animated */

		// bones
		std::array<glm::mat4, MAX_BONES>	_boneOffset;  /**< The offset with last bone */
		std::array<glm::mat4, MAX_BONES>	_bones;  /**< final bone transformation */
		std::map<std::string, uint32_t>		_boneMap;  /**< link bone name to index */
		uint32_t	_nextBoneId;  /**< to keep track of the bones id */

		// animation settings
		aiAnimation					*_curAnimation;  /**< The current animation used */
		std::vector<std::string>	_animationNames;  /**< All animations names */
};

#endif  // OPENGLMODEL_HPP_
