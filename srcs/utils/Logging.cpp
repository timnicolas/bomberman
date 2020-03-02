#include "Logging.hpp"

Logging		logging;

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

Logging::Logging(Logging const &src) {
	*this = src;
}

Logging::~Logging() {
}

Logging & Logging::operator=(Logging const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void Logging::setLoglevel(eLoglevel loglevel) {
	_loglevel = loglevel;
}

void Logging::setLogColor(eLoglevel loglevel, std::string const & color) {
	_colors[loglevel] = color;
}

void Logging::setPrintFileLine(eLoglevel loglevel, bool printFileLine) {
	_printFileLine[loglevel] = printFileLine;
}
void Logging::setPrintFileLine(bool printFileLine) {
	for (int level = 0; level < static_cast<int>(NBLOG); level++) {
		setPrintFileLine(static_cast<eLoglevel>(level), printFileLine);
	}
}

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

std::string const &	Logging::getColor(eLoglevel loglevel) const { return _colors[loglevel]; }
eLoglevel			Logging::getLoglevel() const { return _loglevel; }
