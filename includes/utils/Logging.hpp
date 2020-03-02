#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>

#define COLOR_EOC		"\x1B[0m"
#define COLOR_RED		"\x1B[31m"
#define COLOR_GREEN		"\x1B[32m"
#define COLOR_YELLOW	"\x1B[33m"
#define COLOR_BLUE		"\x1B[34m"
#define COLOR_MAGENTA	"\x1B[35m"
#define COLOR_CYAN		"\x1B[36m"
#define COLOR_WHITE		"\x1B[37m"
#define COLOR_F_RED		"\x1B[41m"
#define COLOR_F_GREEN	"\x1B[42m"
#define COLOR_F_YELLOW	"\x1B[43m"
#define COLOR_F_BLUE	"\x1B[44m"
#define COLOR_F_MAGENTA	"\x1B[45m"
#define COLOR_F_CYAN	"\x1B[46m"
#define COLOR_F_WHITE	"\x1B[47m"
#define COLOR_CLEAR		"\033[H\033[2J"
#define COLOR_BOLD		"\e[1m"
#define COLOR_DIM		"\e[2m"
#define COLOR_ITALIC	"\e[3m"
#define COLOR_ULINE		"\e[4m"
#define COLOR_ULINE_R	"\e[24m"

enum	eLoglevel {
	LOGDEBUG, LOGINFO, LOGSUCCESS, LOGWARN, LOGERROR, LOGFATAL, NBLOG
};

#define log_(level, x) { std::stringstream ss; ss << x; logging.log(level, ss.str(), __FILE__, __LINE__); }

#define logDebug(x) log_(LOGDEBUG, x)
#define logInfo(x) log_(LOGINFO, x)
#define logSuccess(x) log_(LOGSUCCESS, x)
#define logWarn(x) log_(LOGWARN, x)
#define logErr(x) log_(LOGERROR, x)
#define logFatal(x) log_(LOGFATAL, x)
#define logErrExit(x) log_(LOGERROR, x)
#define logFatalExit(x) log_(LOGFATAL, x)

class Logging {
	public:
		Logging();
		Logging(Logging const &src);
		virtual ~Logging();

		Logging &			operator=(Logging const &rhs);

		void				setLoglevel(eLoglevel loglevel);  // don't cout under minLevel
		void				setLogColor(eLoglevel loglevel, std::string const & color);
		void				setPrintFileLine(eLoglevel loglevel, bool printFileLine);
		void				setPrintFileLine(bool printFileLine);

		void				log(eLoglevel level, std::string message, std::string file = "", int line = -1);

		std::string const &	getColor(eLoglevel loglevel) const;
		eLoglevel			getLoglevel() const;

	private:
		std::string		_colors[NBLOG];
		bool			_printFileLine[NBLOG];
		eLoglevel		_loglevel;
};

extern Logging		logging;
