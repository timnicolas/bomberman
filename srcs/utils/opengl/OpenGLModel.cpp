#include <limits>
#include <algorithm>
#include <iterator>

#include "OpenGLModel.hpp"
#include "Logging.hpp"
#include "assimpUtils.hpp"

// -- static initialisation ----------------------------------------------------
std::unique_ptr<Shader> OpenGLModel::_sh = nullptr;

// -- Constructors -------------------------------------------------------------
/**
 * @brief Construct a new OpenGLModel object
 *
 * @param cam camera ref to access to view and projection
 * @param path the 3d model file path
 * @param offset an optional offset to shift the model manually
 * @param centerEnabled option to center the model based on the min/max vertices, not recommended
 * @param scaleEnabled option to scale the model based on the min/max vertices, not recommended
 */
OpenGLModel::OpenGLModel(Camera const &cam, std::string const &path, glm::vec3 offset,
	bool centerEnabled, bool scaleEnabled)
: _cam(cam),
  _path(path),
  _meshes({}),
  _texturesLoaded({}),
  _centerEnabled(centerEnabled),
  _scaleEnabled(scaleEnabled),
  _offset(offset)
{
	// init static shader if null
	if (!_sh) {
		_sh = std::unique_ptr<Shader>(
			new Shader("shaders/model_vs.glsl", "shaders/model_fs.glsl"));
	}

	// used to scale the model
	_minPos = {
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max()};
	_maxPos = {
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest()};

	// retrieve path directory
	_pathDir = _path.substr(0, _path.find_last_of('/'));

	// init model and animation settings
	_model = glm::mat4(1.0f);
	_boneOffset = {{glm::mat4()}};
	_bones = {{glm::mat4(1.0f)}};
	_nextBoneId = 0;

	// load the model from file
	_loadModel();

	// send constants uniforms (lighting, projection mat, ...)
	_setConstUniforms();
}

/**
 * @brief Destroy the Open G L Model:: Open G L Model object
 */
OpenGLModel::~OpenGLModel() {
	// free meshes
	for (auto &it : _meshes) {
		delete(it.second);
	}

	// free textures
	for (auto const &textr : _texturesLoaded) {
		glDeleteTextures(1, &textr.second.id);
	}
}

/**
 * @brief Construct a new Open G L Model:: Open G L Model object
 *
 * @param src The object to do the copy
 */
OpenGLModel::OpenGLModel(OpenGLModel const &src)
: _cam(src._cam),
  _path(src._path),
  _meshes({}),
  _texturesLoaded({}) {
	*this = src;
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return OpenGLModel& A reference to the copied object
 */
OpenGLModel &OpenGLModel::operator=(OpenGLModel const &rhs) {
	#if DEBUG
		logWarn("OpenGLModel::operator= called, remove if it's desired")
	#endif

	if (this != &rhs) {
		_scene = nullptr;
		_texturesLoaded = rhs._texturesLoaded;
		_pathDir = rhs._pathDir;
		_model = rhs._model;
		_minPos = {
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max()};
		_maxPos = {
			std::numeric_limits<float>::lowest(),
			std::numeric_limits<float>::lowest(),
			std::numeric_limits<float>::lowest()};
		_boneMap = std::map<std::string, uint32_t>();
		_boneOffset = {{glm::mat4()}};
		_bones = {{glm::mat4(1.0f)}};
		_nextBoneId = 0;
		_scaleEnabled = rhs._scaleEnabled;

		// reload the model to avoid conflicts in meshes destruction
		_loadModel();
	}
	return *this;
}

// -- _setConstUniforms --------------------------------------------------------
void	OpenGLModel::_setConstUniforms() {
	_sh->use();

	// camera projection
	_sh->setMat4("projection", _cam.getProjection());

	// direction light
	_sh->setVec3("dirLight.direction", -0.2f, -0.8f, 0.6f);
	_sh->setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	_sh->setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f);
	_sh->setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

	_sh->unuse();
}

// -- _loadModel ---------------------------------------------------------------
void	OpenGLModel::_loadModel() {
	// load the model with assimp
	_scene = _importer.ReadFile(_path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_LimitBoneWeights |
		aiProcess_CalcTangentSpace |
		aiProcess_GlobalScale);

	// take ownership of the scene
	_scene = _importer.GetScene();

	// if assimp failed to load the model
	if (!_scene || (_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !_scene->mRootNode) {
		throw OpenGLModel::ModelException(std::string(
			"assimp: " + std::string(_importer.GetErrorString())).c_str());
	}

	// get global transform
	_globalTransform = AiUtils::aiToGlmMat(_scene->mRootNode->mTransformation);

	// load all meshes
	_processNode(_scene->mRootNode, _scene);

	// -- animations -----------------------------------------------------------
	_curAnimation = nullptr;

	// animation finded
	if (_scene->mNumAnimations > 0) {
		_isAnimated = true;

		// save animation names
		for (uint32_t i = 0; i < _scene->mNumAnimations; ++i) {
			_animationNames.push_back(_scene->mAnimations[i]->mName.C_Str());
		}

		// set the current animation
		_curAnimation = _scene->mAnimations[0];
	}
	// no animation finded
	else {
		_isAnimated = false;
		_sh->use();
		// send defaut bones uniforms
		for (uint16_t i = 0; i < MAX_BONES; ++i) {
			_sh->setMat4("bones[" + std::to_string(i) + "]", _bones[i]);
		}
		_sh->unuse();
	}

	// center/scale the model if activated
	_calcCenterScale();

	// send animation and model uniform
	_sh->use();
	_sh->setBool("isAnimated", _isAnimated);
	_sh->setMat4("model", _model);
	_sh->unuse();
}

// -- _processNode -------------------------------------------------------------
void	OpenGLModel::_processNode(aiNode *node, aiScene const *scene) {
	aiMesh	*mesh;

	// process all the node's meshes (if any)
	for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
		mesh = scene->mMeshes[node->mMeshes[i]];
		_processMesh(mesh, scene);
	}

	// recursion with each of its children
	for (uint32_t i = 0; i < node->mNumChildren; ++i) {
		_processNode(node->mChildren[i], scene);
	}
}

// -- _processMesh -------------------------------------------------------------
void	OpenGLModel::_processMesh(aiMesh *aiMesh, aiScene const *scene) {
	std::vector<Vertex>		vertices;
	std::vector<uint32_t>	vertIndices;

	std::vector<Texture>	textures;
	aiMaterial				*material;
	std::vector<Texture>	diffuseMaps;
	std::vector<Texture>	specularMaps;
	std::vector<Texture>	normalMaps;

	VerticesLimits			vLimits;

	// process vertices
	for (uint32_t i = 0; i < aiMesh->mNumVertices; ++i) {
		Vertex	vertex;

		// process vertex positions
		vertex.pos = AiUtils::aiToVec3(aiMesh->mVertices[i]);
		// update the min/max pos to scale the model later
		_updateMinMaxPos(vertex.pos);
		// process vertex normals
		vertex.norm = AiUtils::aiToVec3(aiMesh->mNormals[i]);
		// process vertex texture coordinates
		vertex.texCoords = glm::vec2(0.0f, 0.0f);
		if (aiMesh->mTextureCoords[0]) {
			vertex.texCoords = AiUtils::aiToVec2(aiMesh->mTextureCoords[0][i]);
		}
		// process vertex tangents (for normal map)
		if (aiMesh->mTangents) {
			vertex.tangents = AiUtils::aiToVec3(aiMesh->mTangents[i]);
		}
		else {
			vertex.tangents = glm::vec3();
		}

		// update vertices limits
		if (vertex.pos.x < vLimits.xMin)
			vLimits.xMin = vertex.pos.x;
		if (vertex.pos.x > vLimits.xMax)
			vLimits.xMax = vertex.pos.x;
		if (vertex.pos.y < vLimits.yMin)
			vLimits.yMin = vertex.pos.y;
		if (vertex.pos.y > vLimits.yMax)
			vLimits.yMax = vertex.pos.y;
		if (vertex.pos.z < vLimits.zMin)
			vLimits.zMin = vertex.pos.z;
		if (vertex.pos.z > vLimits.zMax)
			vLimits.zMax = vertex.pos.z;

		vertices.push_back(vertex);
	}

	// calculate bounding box
	BoundingBox	boundingBox;
	boundingBox.center = glm::vec3(
		vLimits.xMin + (vLimits.xMax - vLimits.xMin) / 2,
		vLimits.yMin + (vLimits.yMax - vLimits.yMin) / 2,
		vLimits.zMin + (vLimits.zMax - vLimits.zMin) / 2
	);
	boundingBox.radius = std::max({
		vLimits.xMax - vLimits.xMin,
		vLimits.yMax - vLimits.yMin,
		vLimits.zMax - vLimits.zMin}) / 2;

	// process vertIndices
	for (uint32_t i = 0; i < aiMesh->mNumFaces; ++i) {
		// all face is conposed of 3 vertIndices due to aiProcess_Triangulate
		for (uint32_t j = 0; j < aiMesh->mFaces[i].mNumIndices; ++j) {
			vertIndices.push_back(aiMesh->mFaces[i].mIndices[j]);
		}
	}

	// process material texture
	material = scene->mMaterials[aiMesh->mMaterialIndex];
	try {
		// load diffuse textures
		_loadMaterialTextures(textures, scene, material, aiTextureType_DIFFUSE,
			TextureType::DIFFUSE);
		// load specular textures
		_loadMaterialTextures(textures, scene, material, aiTextureType_SPECULAR,
			TextureType::SPECULAR);
		// load specular textures
		_loadMaterialTextures(textures, scene, material, aiTextureType_NORMALS,
			TextureType::NORMAL);
	}
	catch (TextureException const &e) {
		#if DEBUG
			logWarn("failed to load texture: " << e.what())
		#endif
	}

	// load material
	Material	mat = _loadMaterial(material);

	/* create uniques Meshes name */
	std::string	baseName = std::string(aiMesh->mName.C_Str()) + "::" + mat.name;
	std::string meshName = baseName;
	// if the name already exist prepend a number
	auto	it = _texturesLoaded.find(meshName);
	for (uint16_t i = 0; it != _texturesLoaded.end(); ++i) {
		meshName = baseName + std::to_string(i);
		it = _texturesLoaded.find(meshName);
	}

	// create the mesh
	Mesh	*mesh = new Mesh(*this, *_sh, meshName, vertices, vertIndices,
		mat, boundingBox);

	// set mesh textures
	for (Texture const &tex : textures) {
		mesh->setTexture(tex.type, tex);
	}

	// process mesh bones
	_processBones(aiMesh, *mesh);

	// send mesh to gpu
	mesh->sendMesh();

	// save the mesh
	_meshes.insert({meshName, mesh});
}

// -- _loadMaterialTextures ----------------------------------------------------
void	OpenGLModel::_loadMaterialTextures(std::vector<Texture> &textures,
	aiScene const *scene, aiMaterial *aiMat, aiTextureType type,
	TextureType::Enum textType)
{
	aiString	texLocation;
	std::string	location;
	Texture		texture;
	int			locationId;

	// retrieve texture location
	aiMat->Get(AI_MATKEY_TEXTURE(type, 0), texLocation);
	location = texLocation.C_Str();

	// if the texture was previously loaded, return
	auto	it = _texturesLoaded.find(location);
	if (it != _texturesLoaded.end()) {
		texture = it->second;
		texture.type = textType;
		textures.push_back(texture);
		return;
	}

	// load texture
	if (location.length() != 0) {
		bool inSpaceSRGB = textType == TextureType::DIFFUSE ||
			textType == TextureType::SPECULAR;
		// embedded texture type
		if (location[0] == '*') {
			locationId = std::stoi(location.substr(1));
			texture.id = textureFromFbx(scene, locationId, inSpaceSRGB);
		}
		// regular file texture type
		else {
			texture.id = textureFromFile(_pathDir + "/" + location, inSpaceSRGB);
		}
		texture.type = textType;
		texture.path = location;
		textures.push_back(texture);

		// save to _texturesLoaded to skip duplicate textures loading later
		_texturesLoaded.insert({texture.path, texture});
	}
}

// -- _loadMaterial ------------------------------------------------------------
Material	OpenGLModel::_loadMaterial(aiMaterial *aiMat) {
	Material	mat;
	aiColor3D	color(0.f, 0.f, 0.f);
	float		shininess;
	aiString	matName;

	// name
	if (aiMat->Get(AI_MATKEY_NAME, matName) == AI_SUCCESS) {
		mat.name = matName.C_Str();
	}

	// diffuse
	if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
		mat.diffuse = glm::vec3(color.r, color.g, color.b);
	}

	// ambient
	if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
		mat.ambient = glm::vec3(color.r, color.g, color.b);
	}

	// specular
	if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
		mat.specular = glm::vec3(color.r, color.g, color.b);
	}

	// shininess
	if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
		mat.shininess = shininess;
	}

	return mat;
}

// -- _processBones ------------------------------------------------------------
void	OpenGLModel::_processBones(aiMesh *aiMesh, Mesh &mesh) {
	uint32_t				boneId;
	std::string				boneName;
	int						vertexID;
	float					weight;

	// loop through mesh bones
	for (uint32_t i = 0; i < aiMesh->mNumBones; ++i) {
		boneId = 0;
		boneName = aiMesh->mBones[i]->mName.data;

		// it's a new bone, create new boneId
		if (_boneMap.find(boneName) == _boneMap.end()) {
			boneId = _nextBoneId;
			++_nextBoneId;
			_boneMap[boneName] = boneId;  // save the new boneId
		}
		// existing bone, retrieve boneId from _boneMap
		else {
			boneId = _boneMap[boneName];
		}

		// set the bone offset
		_boneOffset[boneId] = AiUtils::aiToGlmMat(aiMesh->mBones[i]->mOffsetMatrix);

		// send bone infos to the mesh
		for (uint32_t j = 0; j < aiMesh->mBones[i]->mNumWeights; ++j) {
			vertexID = aiMesh->mBones[i]->mWeights[j].mVertexId;
			weight = aiMesh->mBones[i]->mWeights[j].mWeight;

			// skip low weight bones
			if (weight <= 0.01) {
				continue;
			}

			mesh.addBoneData(boneId, weight, vertexID);
		}
	}
}

// -- _updateMinMaxPos ---------------------------------------------------------
// update min/max pos to use later to scale and center the model
void	OpenGLModel::_updateMinMaxPos(glm::vec3 pos) {
	if (pos.x > _maxPos.x)
		_maxPos.x = pos.x;
	if (pos.y > _maxPos.y)
		_maxPos.y = pos.y;
	if (pos.z > _maxPos.z)
		_maxPos.z = pos.z;

	if (pos.x < _minPos.x)
		_minPos.x = pos.x;
	if (pos.y < _minPos.y)
		_minPos.y = pos.y;
	if (pos.z < _minPos.z)
		_minPos.z = pos.z;
}

// -- _calcCenterScale ---------------------------------------------------------
// calculate the model matrix to scale and center the OpenGLModel
void	OpenGLModel::_calcCenterScale() {
	glm::vec3	transl(0, 0, 0);
	float		maxDiff;
	float		scale = 1.0f;

	_modelScale = glm::mat4(1.0f);

	// scale
	if (_scaleEnabled) {
		maxDiff = _maxPos.x - _minPos.x;
		if (maxDiff < _maxPos.y - _minPos.y) {
			maxDiff = _maxPos.y - _minPos.y;
		}
		if (maxDiff < _maxPos.z - _minPos.z) {
			maxDiff = _maxPos.z - _minPos.z;
		}
		maxDiff /= 2;
		scale /= maxDiff;

		// apply the scale
		_modelScale = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	}

	// center
	if (_centerEnabled) {
		transl = -((_minPos + _maxPos) * 0.5f);

		// verification due to float precision
		transl.x = scale * ((transl.x < 0.00001f && transl.x > -0.00001f) ? 0.0f : transl.x);
		transl.y = scale * ((transl.y < 0.00001f && transl.y > -0.00001f) ? 0.0f : transl.y);
		transl.z = scale * ((transl.z < 0.00001f && transl.z > -0.00001f) ? 0.0f : transl.z);
	}

	// add the optional offset to shift the model manually
	transl += _offset;

	// apply the translation
	_modelScale = glm::translate(_modelScale, transl);
}

// -- draw ---------------------------------------------------------------------
/**
 * @brief draw the 3d model
 *
 * @param animationTimeTick the curent animation time in tick
 */
void	OpenGLModel::draw(float animationTimeTick) {
	_sh->use();

	// update bones if the model is animated
	if (_isAnimated) {
		// recalculate bones pos according to the animation
		_setBonesTransform(animationTimeTick, _scene->mRootNode, _scene, _globalTransform);

		// update bones uniforms
		for (uint16_t i = 0; i < MAX_BONES; ++i) {
			_sh->setMat4("bones[" + std::to_string(i) + "]", _bones[i]);
		}
	}

	// update uniforms
	_sh->setMat4("model", _model);
	_sh->setMat4("view", _cam.getViewMatrix());
	_sh->setMat4("modelScale", _modelScale);
	_sh->setBool("isAnimated", _isAnimated);
	_sh->setVec3("viewPos", _cam.pos);

	// draw all meshes
	for (auto &mesh : _meshes) {
		mesh.second->draw(_model);
	}

	glBindVertexArray(0);
	_sh->unuse();
}

// -- loadNextAnimation --------------------------------------------------------
/**
 * @brief change the selected animation by id
 *
 * @param id the animation id
 * @return true on success
 * @return false on failure
 */
bool	OpenGLModel::setAnimation(uint32_t id) {
	if (_isAnimated) {
		if (id < _scene->mNumAnimations) {
			// retrieve animation
			_curAnimation = _scene->mAnimations[id];
			return true;
		}

		logErr("invalid animation id");
	}
	else {
		logWarn("the model have no animation");
	}

	return false;
}

// -- setModel -----------------------------------------------------------------
/**
 * @brief change the model matrix
 *
 * @param model
 */
void	OpenGLModel::setModel(glm::mat4 const model) { _model = model; }

// -- getAnimationId -----------------------------------------------------------
/**
 * @brief retrieve animation id from name
 *
 * @param name the animation name
 * @param outId modified with result on success
 * @return true on success
 * @return false on failure
 */
bool	OpenGLModel::getAnimationId(std::string const name, uint32_t &outId) const {
	auto it = std::find(_animationNames.begin(), _animationNames.end(), name);

	// if the name was finded
	if (it != _animationNames.end()) {
		// save the id to outId ref
		outId = std::distance(_animationNames.begin(), it);
		return true;
	}

	logWarn("animation name \"" << name << "\" not found");
	return false;
}

/**
 * @brief manually set the texture of a specified mesh
 *
 * @param type the texture type DIFFUSE/SPECULAR/NORMAL/...
 * @param meshName the name of the desired mesh, find it with printMeshesNames
 * @param path the texture path
 * @param inSpaceSRGB is the texture in srgb space ?
 */
void	OpenGLModel::setMeshTexture(TextureType::Enum type, std::string const meshName,
	std::string const path, bool inSpaceSRGB)
{
	_loadTexture(type, path, inSpaceSRGB);

	// load only if the texture was not previously loaded
	auto	it = _texturesLoaded.find(path);
	if (it != _texturesLoaded.end()) {
		// it->second

		auto	meshIt = _meshes.find(meshName);
		if (meshIt != _meshes.end()) {
			meshIt->second->setTexture(type, it->second);
			return;
		}

		logWarn("[setMeshTexture] meshName: \"" << meshName << "\" not found");
	}
}

/**
 * @brief print all meshes names
 *
 */
void	OpenGLModel::printMeshesNames() const {
	for (auto &it : _meshes) {
		logInfo('"' << it.first << '"');
	}
}

// -- getters ------------------------------------------------------------------
/**
 * @brief get assimp animation object from id
 *
 * @param id the animation id
 * @return aiAnimation* on success
 * @return nullptr on failure
 */
aiAnimation	*OpenGLModel::getAiAnimation(uint32_t id) const {
	if (_isAnimated) {
		// on valid id
		if (id < _scene->mNumAnimations) {
			return _scene->mAnimations[id];
		}

		logErr("invalid animation id");
	} else {
		logWarn("the model have no animation");
	}

	return nullptr;
}

/**
 * @brief to check if a model is animated
 *
 * @return true if animated
 * @return false if not animated
 */
bool	OpenGLModel::isAnimated() const { return _isAnimated; }

/**
 * @brief return number of animations
 *
 * @return uint32_t the number of animations
 */
uint32_t	OpenGLModel::getNbAnimations() const {
	if (_scene) {
		return _scene->mNumAnimations;
	}
	return 0;
}

/**
 * @brief return the availables animations names
 *
 * @return std::vector<std::string>
 */
std::vector<std::string>	OpenGLModel::getAnimationNames() const {
	return _animationNames;
}

/**
 * @brief Get the camera
 *
 * @return Camera const& The camera
 */
Camera const	&OpenGLModel::getCam() const { return _cam; }

/**
 * @brief Get the path
 *
 * @return std::string Path
 */
std::string	OpenGLModel::getPath() const { return _path; }

// -- _setBonesTransform -------------------------------------------------------
void	OpenGLModel::_setBonesTransform(float animationTimeTick, aiNode *node,
	aiScene const *scene, glm::mat4 parentTransform)
{
	// retrieve animation node
	std::string	nodeName(node->mName.data);
	glm::mat4	nodeTransform = AiUtils::aiToGlmMat(node->mTransformation);

	aiNodeAnim const	*nodeAnim = _findNodeAnim(nodeName);

	try {
		// if we have an animation node, interpolate the bone transformation
		if (nodeAnim) {
			// interpolate scaling
			glm::vec3	scaling = _calcInterpolatedScaling(animationTimeTick, nodeAnim);
			glm::mat4	scalingM = glm::scale(glm::mat4(1.0), scaling);

			// interpolate rotation
			glm::quat	rotationQ = _calcInterpolatedRotation(animationTimeTick, nodeAnim);
			glm::mat4	rotationM = glm::toMat4(rotationQ);

			// interpolate translation
			glm::vec3	translation = _calcInterpolatedPosition(animationTimeTick, nodeAnim);
			glm::mat4	translationM = glm::translate(glm::mat4(1.0), translation);

			// Combine the above transformations
			nodeTransform = translationM * rotationM * scalingM;
		}

		// calculate the new bone global transformation with parent node
		glm::mat4	boneGlobalTransform = parentTransform * nodeTransform;

		// update the bone global transformation
		if (_boneMap.find(nodeName) != _boneMap.end()) {
			uint32_t	boneIndex = _boneMap[nodeName];
			_bones[boneIndex] =
				_globalTransform * boneGlobalTransform * _boneOffset[boneIndex];
		}

		// recursion with each of the bone childs
		for (uint32_t i = 0; i < node->mNumChildren; ++i) {
			_setBonesTransform(animationTimeTick, node->mChildren[i], scene,
				boneGlobalTransform);
		}
	}
	catch (ModelException const &e) {
		logDebug("Error in bones calculation: " << e.what());
	}
}

// -- _findNodeAnim ------------------------------------------------------------
aiNodeAnim const	*OpenGLModel::_findNodeAnim(std::string const nodeName)
{
	// loop through animations nodes until we find the desired node
	for (uint32_t i = 0; i < _curAnimation->mNumChannels; ++i) {
		aiNodeAnim const	*nodeAnim = _curAnimation->mChannels[i];

		// animation node finded, return it
		if (std::string(nodeAnim->mNodeName.data) == nodeName) {
			return nodeAnim;
		}
	}

	return nullptr;
}

// -- _calcInterpolatedScaling -------------------------------------------------
glm::vec3	OpenGLModel::_calcInterpolatedScaling(float animationTimeTick,
	aiNodeAnim const *nodeAnim)
{
	// if the scaling is not animated, return the scale value
	if (nodeAnim->mNumScalingKeys == 1) {
		return AiUtils::aiToVec3(nodeAnim->mScalingKeys[0].mValue);
	}

	// retrieve curent and next scaling index
	std::pair<uint32_t, uint32_t>	animIndex = _findAnimIndex(AnimKeyType::SCALE,
		animationTimeTick, nodeAnim);
	// calculate dtTime between first, second animation index
	float	indexDtTime = static_cast<float>(nodeAnim->mScalingKeys[animIndex.second].mTime
		- nodeAnim->mScalingKeys[animIndex.first].mTime);
	// calculate the mix factor
	float	factor = (animationTimeTick - static_cast<float>(
		nodeAnim->mScalingKeys[animIndex.first].mTime)) / indexDtTime;
	factor = std::clamp(factor, 0.0f, 1.0f);

	// retrieve start/end values
	glm::vec3 const	startScale = AiUtils::aiToVec3(
		nodeAnim->mScalingKeys[animIndex.first].mValue);
	glm::vec3 const	endScale = AiUtils::aiToVec3(
		nodeAnim->mScalingKeys[animIndex.second].mValue);

	return glm::lerp(startScale, endScale, factor);
}

// -- _calcInterpolatedRotation ------------------------------------------------
glm::quat	OpenGLModel::_calcInterpolatedRotation(float animationTimeTick,
	aiNodeAnim const *nodeAnim)
{
	// if the scaling is not animated, return the scale value
	if (nodeAnim->mNumRotationKeys == 1) {
		return AiUtils::aiToQuat(nodeAnim->mRotationKeys[0].mValue);
	}

	// retrieve curent and next scaling index
	std::pair<uint32_t, uint32_t>	animIndex = _findAnimIndex(AnimKeyType::SCALE,
		animationTimeTick, nodeAnim);
	// calculate dtTime between first, second animation index
	float	indexDtTime = static_cast<float>(nodeAnim->mRotationKeys[animIndex.second].mTime
		- nodeAnim->mRotationKeys[animIndex.first].mTime);
	// calculate the mix factor
	float	factor = (animationTimeTick - static_cast<float>(
		nodeAnim->mRotationKeys[animIndex.first].mTime)) / indexDtTime;
	factor = std::clamp(factor, 0.0f, 1.0f);

	// retrieve start/end values
	glm::quat const	startRotatQ = AiUtils::aiToQuat(
		nodeAnim->mRotationKeys[animIndex.first].mValue);
	glm::quat const	endRotatQ = AiUtils::aiToQuat(
		nodeAnim->mRotationKeys[animIndex.second].mValue);

	return glm::normalize(glm::slerp(startRotatQ, endRotatQ, factor));
}

// -- _calcInterpolatedPosition ------------------------------------------------
glm::vec3	OpenGLModel::_calcInterpolatedPosition(float animationTimeTick,
	aiNodeAnim const *nodeAnim)
{
	// if the scaling is not animated, return the scale value
	if (nodeAnim->mNumPositionKeys == 1) {
		return AiUtils::aiToVec3(nodeAnim->mPositionKeys[0].mValue);
	}

	// retrieve curent and next scaling index
	std::pair<uint32_t, uint32_t>	animIndex = _findAnimIndex(AnimKeyType::SCALE,
		animationTimeTick, nodeAnim);
	// calculate dtTime between first, second animation index
	float	indexDtTime = static_cast<float>(nodeAnim->mPositionKeys[animIndex.second].mTime
		- nodeAnim->mPositionKeys[animIndex.first].mTime);
	// calculate the mix factor
	float	factor = (animationTimeTick - static_cast<float>(
		nodeAnim->mPositionKeys[animIndex.first].mTime)) / indexDtTime;
	factor = std::clamp(factor, 0.0f, 1.0f);

	// retrieve start/end values
	glm::vec3 const	startPos = AiUtils::aiToVec3(
		nodeAnim->mPositionKeys[animIndex.first].mValue);
	glm::vec3 const	endPos = AiUtils::aiToVec3(
		nodeAnim->mPositionKeys[animIndex.second].mValue);

	return glm::lerp(startPos, endPos, factor);
}

// -- _findAnimIndex -----------------------------------------------------------
std::pair<uint32_t, uint32_t>	OpenGLModel::_findAnimIndex(AnimKeyType::Enum animType,
	float animationTimeTick, aiNodeAnim const *nodeAnim)
{
	// retrieve number of animation keys
	uint32_t	nbAnimKeys = nodeAnim->mNumScalingKeys;
	if (animType == AnimKeyType::ROTATION) {
		nbAnimKeys = nodeAnim->mNumRotationKeys;
	}
	else if (animType == AnimKeyType::POSITION) {
		nbAnimKeys = nodeAnim->mNumPositionKeys;
	}

	// exception if no key exist
	if (nbAnimKeys < 1) {
		throw OpenGLModel::ModelException(std::string("animation: no " +
			AnimKeyType::enumName[animType] + " animation key finded").c_str());
	}

	// find the corect key pair
	for (uint32_t i = 0; i < nbAnimKeys - 1; ++i) {
		if (animType == AnimKeyType::SCALE && animationTimeTick <=
			static_cast<float>(nodeAnim->mScalingKeys[i + 1].mTime)) {
			return std::make_pair(i, i + 1);
		}
		else if (animType == AnimKeyType::ROTATION && animationTimeTick <=
			static_cast<float>(nodeAnim->mRotationKeys[i + 1].mTime))  {
			return std::make_pair(i, i + 1);
		}
		else if (animType == AnimKeyType::POSITION && animationTimeTick <=
			static_cast<float>(nodeAnim->mPositionKeys[i + 1].mTime))  {
			return std::make_pair(i, i + 1);
		}
	}
	// it's the last key, return it twice
	uint32_t	lastI = nbAnimKeys - 1;
	return std::make_pair(lastI, lastI);
}

// -- _loadTexture --------------------------------------------------------------
/**
 * @brief load new texture manually
 *
 * @param type
 * @param path
 * @param inSpaceSRGB
 */
void	OpenGLModel::_loadTexture(TextureType::Enum type, std::string const path,
	bool inSpaceSRGB)
{
	// load only if the texture was not previously loaded
	auto	it = _texturesLoaded.find(path);
	if (it == _texturesLoaded.end()) {
		Texture	texture;

		try {
			texture.id = textureFromFile(path, inSpaceSRGB);
		}
		catch (TextureException const &e) {
			#if DEBUG
				logWarn("failed to load texture: " << e.what())
			#endif
		}
		texture.type = type;
		texture.path = path;

		_texturesLoaded.insert({path, texture});
	}
}


// -- Exceptions errors --------------------------------------------------------
/**
 * @brief Construct a new Open G L Model:: Model Exception:: Model Exception object
 */
OpenGLModel::ModelException::ModelException()
: std::runtime_error("ModelException") {}

/**
 * @brief Construct a new Open G L Model:: Model Exception:: Model Exception object
 *
 * @param what_arg Error message
 */
OpenGLModel::ModelException::ModelException(char const *what_arg)
: std::runtime_error(
	std::string(std::string("ModelException: ") + what_arg).c_str()) {}

// -- VerticesLimits -----------------------------------------------------------
/**
 * @brief Construct a new Vertices Limits:: Vertices Limits object
 */
VerticesLimits::VerticesLimits() {
	xMin = std::numeric_limits<float>::max();
	xMax = std::numeric_limits<float>::lowest();
	yMin = std::numeric_limits<float>::max();
	yMax = std::numeric_limits<float>::lowest();
	zMin = std::numeric_limits<float>::max();
	zMax = std::numeric_limits<float>::lowest();
}
