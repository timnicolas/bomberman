#ifndef MODEL_HPP_
#define MODEL_HPP_

// maximum bones on one model
#define MAX_BONES 100

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <array>
#include <stdexcept>
#include <memory>
#include <utility>  // for pair

#include <assimp/Importer.hpp>
#include "Texture.hpp"
#include "Mesh.hpp"

namespace AnimKeyType {
	enum Enum {
		SCALE,
		ROTATION,
		POSITION
	};

	std::array<std::string, 3> const	enumName = {
		"scale",
		"rotation",
		"position"
	};
}  // namespace AnimKeyType

class Model {
	public:
        Model(std::string const &path, float const &dtTime, float const &animationSpeed);
		virtual ~Model();
		Model(Model const &src);
		Model &operator=(Model const &rhs);

		void	draw();
		void	loadNextAnimation();

		// Exceptions
		class ModelException : public std::runtime_error {
			public:
				ModelException();
				explicit ModelException(char const *what_arg);
		};

	private:
		Model();  // private default constructor, should not be called

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
		void	_calcModelMatrix();

		// -- animation bones calculation --------------------------------------
		void	_setBonesTransform(float animationTime, aiNode *node,
			aiScene const *scene, glm::mat4 parentTransform);
		aiNodeAnim const	*_findNodeAnim(aiAnimation const *animation,
			std::string const nodeName);

		// interpolate scaling
		glm::vec3	_calcInterpolatedScaling(float animationTime,
			const aiNodeAnim* nodeAnim);
		// interpolate rotation
		glm::quat	_calcInterpolatedRotation(float animationTime,
			aiNodeAnim const *nodeAnim);
		// interpolate position
		glm::vec3	_calcInterpolatedPosition(float animationTime,
			aiNodeAnim const *nodeAnim);

		std::pair<uint32_t, uint32_t>	_findAnimIndex(AnimKeyType::Enum animType,
			float animationTime, aiNodeAnim const *nodeAnim);

		// -- members ----------------------------------------------------------
		static std::unique_ptr<Shader>	_sh;

		std::string const	_path;  // model file path
		std::vector<Mesh>	_meshes;  // all model meshes

		// assimp utility
		aiScene const		*_scene;
		Assimp::Importer	_importer;

		// textures
		std::vector<Texture>	_texturesLoaded;  // keep track of loaded textures
		std::string				_pathDir;

		// model position
		glm::mat4				_model;  // position in real world
		glm::vec3				_minPos, _maxPos;  // to scale the model
		glm::mat4				_modelScale;

		// global transform matrix
		glm::mat4			_globalTransform;

		// -- animation related ------------------------------------------------
		bool	_isAnimated;

		// bones
		std::array<glm::mat4, MAX_BONES>	_boneOffset;
		std::array<glm::mat4, MAX_BONES>	_bones;  // final bone transformation
		std::map<std::string, uint32_t>		_boneMap;  // link bone name to index
		uint32_t			_nextBoneId;  // to keep track of the bones id

		// animation settings
		float const			&_dtTime;
		float const			&_animationSpeed;
		float				_animationTime;
		uint32_t			_curAnimationId;
		aiAnimation			*_curAnimation;
};

#endif  // MODEL_HPP_
