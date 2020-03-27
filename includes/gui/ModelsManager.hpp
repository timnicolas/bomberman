#ifndef MODELSMANAGER_HPP_
#define MODELSMANAGER_HPP_

#include <unordered_map>

#include "OpenGLModel.hpp"
#include "Camera.hpp"

/**
 * @brief class to manage OpenGLModel
 * store all OpenGLModel inside to avoid multiples loadings
 *
 */
class ModelsManager {
	public:
		virtual ~ModelsManager();

		static ModelsManager	&get();
		bool	init(Camera const &cam);

	private:
		ModelsManager();
		ModelsManager(ModelsManager const &src);
		ModelsManager &operator=(ModelsManager const &rhs);

		std::unordered_map<std::string, OpenGLModel *>	_models;
		bool	_initDone;
};

#endif  // MODELSMANAGER_HPP_
