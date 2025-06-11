#ifndef LOG_HPP
#define LOG_HPP

#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>

#include <ctime>
#include <map>
#include <string>

class Log {
public:
	// Levels of log
	enum LogStep {
		DEBUG = 0,
		INFO,
		ERROR,
		FATAL,
	};

	static void log(LogStep level, const char *msg, ...);

	// SETTERS
	static void setLogState(bool state);
	static void setLogFileState(bool state);
	static void setLogDebugState(bool state);

	// GETTERS
	static bool getLogState(void);
	static bool getLogFileState(void);
	static bool getLogDebugState(void);
	static std::string getLogFileName(void);
	static std::string getLogStepMap(LogStep level);
	static std::string getLogStepColor(LogStep level);

private:
	static bool _logState;
	static bool _logFileState;
	static bool _logDebugState;
	static std::string _logFileName;
	static std::map<Log::LogStep, std::string> _logStepMap;
	static std::map<Log::LogStep, std::string> _LogStepColor;

	// UTILS
	static std::string _fileNameGenerator(void);
	static std::map<Log::LogStep, std::string> _selectLogStepStr(void);
	static std::map<Log::LogStep, std::string> _selectLogStepColor(void);
	static std::string _logFormater(Log::LogStep level, const std::string msg,
									std::string time, bool color_on = true);
	static void _printMsgFormater(Log::LogStep level, const std::string msg,
								  std::string time);
	static void _InLogFile(LogStep level, const char *msg, std::string time);
};

#endif
