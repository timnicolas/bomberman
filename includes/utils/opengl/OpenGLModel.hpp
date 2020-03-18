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

class OpenGLModel {
	public:
        OpenGLModel(Gui const &_gui, std::string const &path, glm::vec3 offset = {0, 0, 0},
			bool centerEnabled = false, bool scaleEnabled = false);
		virtual ~OpenGLModel();
		OpenGLModel(OpenGLModel const &src);
		OpenGLModel &operator=(OpenGLModel const &rhs);

		void		draw(float animationTimeTick = 0.0f);
		bool		setAnimation(uint32_t id);
		void		setModel(glm::mat4 const model);

		bool		getAnimationId(std::string const name, uint32_t &outId) const;
		aiAnimation	*getAiAnimation(uint32_t id);
		bool		isAnimated() const;
		std::vector<std::string>	getAnimationNames() const;

		// Exceptions
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

		// -- members ----------------------------------------------------------
		static std::unique_ptr<Shader>	_sh;

		Gui const			&_gui;
		std::string const	_path;  // model file path
		std::vector<Mesh *>	_meshes;  // all model meshes

		// assimp utility
		aiScene const		*_scene;
		Assimp::Importer	_importer;

		// textures
		std::vector<Texture>	_texturesLoaded;  // keep track of loaded textures
		std::string				_pathDir;

		// model position
		glm::mat4	_model;  // position in real world
		bool		_centerEnabled;  // center the model on loading
		bool		_scaleEnabled;  // scale the model on loading
		glm::vec3	_minPos, _maxPos;  // to scale the model
		glm::mat4	_modelScale;
		glm::vec3	_offset;  // offset to shift the model manualy

		// global transform matrix
		glm::mat4	_globalTransform;

		// -- animation related ------------------------------------------------
		bool	_isAnimated;

		// bones
		std::array<glm::mat4, MAX_BONES>	_boneOffset;
		std::array<glm::mat4, MAX_BONES>	_bones;  // final bone transformation
		std::map<std::string, uint32_t>		_boneMap;  // link bone name to index
		uint32_t	_nextBoneId;  // to keep track of the bones id

		// animation settings
		aiAnimation					*_curAnimation;
		std::vector<std::string>	_animationNames;
};

#endif  // OPENGLMODEL_HPP_
