#include "Logging.hpp"

Logging		logging;

/**
 * @brief Construct a new Logging:: Logging object
 */
Logging::Logging()
: _loglevel(LOGDEBUG) {
	setLogColor(LOGDEBUG, COLOR_WHITE);
	setLogColor(LOGINFO, COLOR_WHITE);
	setLogColor(LOGSUCCESS, std::string(COLOR_GREEN) + COLOR_BOLD);
	setLogColor(LOGWARN, std::string(COLOR_YELLOW) + COLOR_BOLD);
	setLogColor(LOGERROR, std::string(COLOR_RED) + COLOR_BOLD);
	setLogColor(LOGFATAL, std::string(COLOR_RED) + COLOR_BOLD);

	setPrintFileLine(false);
}

/**
 * @brief Construct a new Logging:: Logging object
 *
 * @param src The object to do the copy
 */
Logging::Logging(Logging const &src) {
	*this = src;
}

/**
 * @brief Destroy the Logging:: Logging object
 */
Logging::~Logging() {
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return Logging& A reference to the copied object
 */
Logging & Logging::operator=(Logging const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief The the minimum log level to print
 *
 * logging.setLoglevel(LOGINFO);  // to discard logDebug
 *
 * @param loglevel The minimum log to print
 */
void Logging::setLoglevel(eLoglevel loglevel) {
	_loglevel = loglevel;
}

/**
 * @brief Set the color for a given log function
 *
 * @param loglevel The log to update
 * @param color The new color
 */
void Logging::setLogColor(eLoglevel loglevel, std::string const & color) {
	_colors[loglevel] = color;
}

/**
 * @brief Enable a given log function to print his file and line
 *
 * @param loglevel The log to update
 * @param printFileLine Enable or disable option
 */
void Logging::setPrintFileLine(eLoglevel loglevel, bool printFileLine) {
	_printFileLine[loglevel] = printFileLine;
}
/**
 * @brief Enable all log function to print his file and line
 *
 * @param printFileLine Enable or disable option
 */
void Logging::setPrintFileLine(bool printFileLine) {
	for (int level = 0; level < static_cast<int>(NBLOG); level++) {
		setPrintFileLine(static_cast<eLoglevel>(level), printFileLine);
	}
}

/**
 * @brief Log a mesage
 *
 * Normally, this function is called by defines (logDebug(...), logWarn(...), ...)
 *
 * @param level The log level
 * @param message The message
 * @param file The file of calling function
 * @param line the line of calling function
 */
void	Logging::log(eLoglevel level, std::string message, std::string file, int line) {
	if (level < _loglevel)
		return;
	std::stringstream ss;
	// start logging
	ss << _colors[level];
	if (level == LOGDEBUG) ss << "[DEBUG]";
	else if (level == LOGINFO) ss << "[INFO]";
	else if (level == LOGSUCCESS) ss << "[SUCCESS]";
	else if (level == LOGWARN) ss << "[WARN]";
	else if (level == LOGERROR) ss << "[ERROR]";
	else if (level == LOGFATAL) ss << "[FATAL]";
	if (_printFileLine[level])
		ss << "[" << file << " " << line << "]";
	ss << ": ";
	ss << message;
	ss << COLOR_EOC << std::endl;

	std::cout << ss.str();
}

/**
 * @brief Get the color of a log level
 *
 * @param loglevel The log level
 * @return std::string const& The color
 */
std::string const &	Logging::getColor(eLoglevel loglevel) const { return _colors[loglevel]; }
/**
 * @brief Get the first log level printed (all loglevel under are skipped)
 *
 * @return eLoglevel The first skipped log level
 */
eLoglevel			Logging::getLoglevel() const { return _loglevel; }
