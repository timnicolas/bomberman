#include <fstream>
#include "FileUtils.hpp"

namespace file {
	/**
	 * @brief Try if a file exist
	 *
	 * @param filename The file to check
	 * @return true If the file exist
	 */
	bool isFile(std::string const & filename) {
		std::ifstream ifile(filename);
		if (ifile)
			return true;
		return false;
	}
}  // namespace file
