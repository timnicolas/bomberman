#pragma once

#include <string>
#include <vector>

namespace file {
	bool	isFile(std::string const & filename);
	bool	isDir(std::string const & path);

	bool	mkdir(std::string const & path, bool silent = false);
	bool 	rm(std::string const & path, bool silent = false);

	std::vector<std::string>	ls(std::string const & path, bool silent = false);
}
