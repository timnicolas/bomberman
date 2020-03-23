#include <filesystem>
#include <fstream>
#include <iostream>
#include "FileUtils.hpp"
#include "Logging.hpp"

namespace file {
	/**
	 * @brief Try if a file exist
	 *
	 * @param filename The file to check
	 * @return true If the file exist
	 */
	bool isFile(std::string const & filename) {
		return std::filesystem::is_regular_file(filename);
	}

	/**
	 * @brief Try if it's a directory
	 *
	 * @param path The directory path
	 * @return true If the directory exist
	 */
	bool	isDir(std::string const & path) {
		return std::filesystem::is_directory(path);
	}

	/**
	 * @brief Create a directory
	 *
	 * @param path The path of the directory
	 * @param silent If true, don't show warning and errors
	 * @return false if failed to create directories
	 */
	bool	mkdir(std::string const & path, bool silent) {
		bool		error = false;
		std::string	msg = "";
		try {
			std::filesystem::create_directories(path);
			if (file::isDir(path) == false)
				error = true;
		}
		catch (std::exception const & e) {
			error = true;
			msg = std::string(" -> ") + e.what();
		}
		if (error) {
			if (!silent)
				logErr("Unable to create dir: " << path << msg);
			return false;
		}
		return true;
	}

	/**
	 * @brief Remove a file or a directory
	 *
	 * @param path The path to remove
	 * @param silent If true, don't show warning and errors
	 * @return false if failed to remove directory
	 */
	bool rm(std::string const & path, bool silent) {
		bool		error = false;
		std::string	msg = "";
		try {
			std::filesystem::remove_all(path);
			if (file::isDir(path))
				error = true;
		}
		catch (std::exception const & e) {
			error = true;
			msg = std::string(" -> ") + e.what();
		}
		if (error) {
			if (!silent)
				logErr("Unable to create dir: " << path << msg);
			return false;
		}
		return true;
	}

}  // namespace file
