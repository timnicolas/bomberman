#ifndef MODELSMANAGER_HPP_
#define MODELSMANAGER_HPP_

#include <unordered_map>

#include "OpenGLModel.hpp"
#include "Camera.hpp"

/**
 * @brief class to manage OpenGLModel
 * store all OpenGLModel inside to avoid multiples loadings
 */
class ModelsManager {
	public:
		virtual ~ModelsManager();
		static ModelsManager	&get();

		static bool			init(Camera const &cam);
		static OpenGLModel	&getModel(std::string const &name);

		// -- exceptions -------------------------------------------------------
		/**
		 * @brief Model manager exception
		 */
		class ModelsManagerException : public std::runtime_error {
			public:
				ModelsManagerException();
				explicit ModelsManagerException(const char* what_arg);
		};

	private:
		ModelsManager();
		ModelsManager(ModelsManager const &src);
		ModelsManager &operator=(ModelsManager const &rhs);

		bool		_init(Camera const &cam);
		OpenGLModel	&_getModel(std::string const &name);

		std::unordered_map<std::string, OpenGLModel *>	_models;  /**< All models */
		bool	_initDone;  /**< True if init is done */
};

#endif  // MODELSMANAGER_HPP_
