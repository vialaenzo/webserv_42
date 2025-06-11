#include "Log.hpp"

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <wchar.h>

#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "colors.hpp"

bool Log::_logState = true;
bool Log::_logFileState = false;
bool Log::_logDebugState = false;

std::string Log::_logFileName = Log::_fileNameGenerator();

std::map<Log::LogStep, std::string> Log::_logStepMap = Log::_selectLogStepStr();

std::map<Log::LogStep, std::string> Log::_LogStepColor =
	Log::_selectLogStepColor();

/*
#desc : Genrate the log file name

#return : std::string
*/
std::string Log::_fileNameGenerator(void) {
	std::time_t t = std::time(NULL);
	char buffer[80];

	std::string format = "webserv_%Y-%m-%d_%H-%M-%S.log";
	std::strftime(buffer, sizeof(buffer), format.c_str(), std::localtime(&t));
	return (std::string(buffer));
}

std::map<Log::LogStep, std::string> Log::_selectLogStepColor(void) {
	std::map<Log::LogStep, std::string> Colorization;

	Colorization[Log::FATAL] = RED;
	Colorization[Log::ERROR] = ORANGE;
	Colorization[Log::INFO] = GREEN;
	Colorization[Log::DEBUG] = CYAN;

	return (Colorization);
}

std::map<Log::LogStep, std::string> Log::_selectLogStepStr(void) {
	std::map<Log::LogStep, std::string> logStepCouple;

	logStepCouple[Log::FATAL] = "FATAL";
	logStepCouple[Log::ERROR] = "ERROR";
	logStepCouple[Log::INFO] = "INFO";
	logStepCouple[Log::DEBUG] = "DEBUG";

	return (logStepCouple);
}

std::string Log::_logFormater(Log::LogStep step, const std::string msg,
							  std::string time, bool color_on) {
	std::string formatedMsg;

	if (color_on == true) formatedMsg += Log::getLogStepColor(step);
	formatedMsg += "[" + Log::getLogStepMap(step) + "]\t" + time + " : " + msg;
	if ((step == Log::FATAL) && errno != 0)
		formatedMsg += ": " + static_cast<std::string>(std::strerror(errno));
	if (color_on == true) formatedMsg += RESET;

	return (formatedMsg);
}

void Log::_printMsgFormater(Log::LogStep step, const std::string msg,
							std::string time) {
	std::cout << Log::_logFormater(step, msg, time) << std::endl;
}

void Log::_InLogFile(Log::LogStep step, const char *msg, std::string time) {
	if (mkdir("logs", 0777) == -1 && errno != EEXIST) {
		std::cerr << "Error: " << std::strerror(errno) << std::endl;
		return;
	}
	int file = open(("logs/" + Log::getLogFileName()).c_str(),
					O_CREAT | O_WRONLY | O_APPEND, 0666);
	std::string log = Log::_logFormater(step, msg, time, false);
	if (write(file, log.c_str(), log.size()) == -1)
		Log::log(Log::ERROR, "writing failed");
	if (write(file, "\n", 1) == -1) Log::log(Log::ERROR, "writing failed");
	close(file);
}

void Log::log(Log::LogStep level, const char *msg, ...) {
	// Check if the Log is enabled and the log level is valid
	bool Log_ON = Log::getLogState();
	bool Debug_ON = (level != Log::DEBUG || Log::getLogDebugState());

	if (!Log_ON || !Debug_ON) return;

	// Create a buffer and initialize variable argument list
	std::vector<char> buffer(1024);
	va_list args;
	va_start(args, msg);

	// Format the message
	int size = vsnprintf(buffer.data(), buffer.size(), msg, args);
	va_end(args);

	// Check for formatting errors
	if (size < 0) return;

	// Resize the buffer if necessary
	if (static_cast<size_t>(size) >= buffer.size()) {
		buffer.resize(size + 1);  // Resize to fit the message
		va_start(args, msg);
		vsnprintf(buffer.data(), buffer.size(), msg, args);
		va_end(args);
	}

	// Get the current time and format it
	std::time_t currentTime = std::time(0);
	char timeBuffer[80];
	std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S",
				  std::localtime(&currentTime));

	// Log the message to the console
	Log::_printMsgFormater(level, buffer.data(), timeBuffer);

	// If logging to a file is enabled, log to file
	if (Log::getLogFileState())
		Log::_InLogFile(level, buffer.data(), timeBuffer);

	// Throw a fatal exception if the log level is FATAL
	if (level == Log::FATAL) throw std::runtime_error(buffer.data());
}

/* _____ ______ _______ _______ ______ _____   _____
  / ____|  ____|__   __|__   __|  ____|  __ \ / ____|
 | |  __| |__     | |     | |  | |__  | |__) | (___
 | | |_ |  __|    | |     | |  |  __| |  _  / \___ \
 | |__| | |____   | |     | |  | |____| | \ \ ____) |
  \_____|______|  |_|     |_|  |______|_|  \_\_____/

													  */

bool Log::getLogState(void) { return (Log::_logState); }

bool Log::getLogFileState(void) { return (Log::_logFileState); }

bool Log::getLogDebugState(void) { return (Log::_logDebugState); }

std::string Log::getLogFileName(void) { return (Log::_logFileName); }

std::string Log::getLogStepMap(Log::LogStep level) {
	return (Log::_logStepMap[level]);
}

std::string Log::getLogStepColor(Log::LogStep level) {
	return (Log::_LogStepColor[level]);
}

/* _____ ______ _______ _______ ______ _____   _____
  / ____|  ____|__   __|__   __|  ____|  __ \ / ____|
 | (___ | |__     | |     | |  | |__  | |__) | (___
  \___ \|  __|    | |     | |  |  __| |  _  / \___ \
  ____) | |____   | |     | |  | |____| | \ \ ____) |
 |_____/|______|  |_|     |_|  |______|_|  \_\_____/

													  */

void Log::setLogState(bool state) { Log::_logState = state; }

void Log::setLogFileState(bool state) { Log::_logFileState = state; }

void Log::setLogDebugState(bool state) { Log::_logDebugState = state; }
