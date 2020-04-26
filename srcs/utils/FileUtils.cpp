#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
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
		return boost::filesystem::is_regular_file(filename);
	}

	/**
	 * @brief Try if it's a directory
	 *
	 * @param path The directory path
	 * @return true If the directory exist
	 */
	bool	isDir(std::string const & path) {
		return boost::filesystem::is_directory(path);
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
			boost::filesystem::create_directories(path);
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
			boost::filesystem::remove_all(path);
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

	/**
	 * @brief List files in a folder
	 *
	 * @param path The folder path
	 * @param silent If true, don't show warning and errors
	 * @return std::vector<std::string> The list of all filenames
	 */
	std::vector<std::string>	ls(std::string const & path, bool silent) {
		std::vector<std::string> list;

		if (!isDir(path)) {
			if (!silent)
				logErr("In file::ls: " << path << " is not a valid directory");
			return list;
		}

		boost::filesystem::directory_iterator end_itr;
		for (boost::filesystem::directory_iterator itr(path); itr != end_itr; ++itr) {
			if (boost::filesystem::is_regular_file(itr->path())) {
				std::string currentFile = itr->path().string();
				list.push_back(currentFile);
			}
		}
		return list;
	}

}  // namespace file
