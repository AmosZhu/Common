/*
 * Logger.hpp
 *
 *  Created on: May 29, 2016
 *      Author: amos
 *
 * Reference: http://www.drdobbs.com/cpp/a-lightweight-logger-for-c/240147505?pgno=3
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <fstream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <mutex>

namespace Logger {

enum log_level {
	DEBUG = 0, WARNING, ERROR,
};

/*
 * Policy definition
 */
class log_policy_interface {
public:
	virtual void Open(const std::string& name) = 0;
	virtual void Close() = 0;
	virtual void Write(const std::string& msg) = 0;
};

class file_log_policy: public log_policy_interface {

public:
	file_log_policy();
	~file_log_policy();

	void Open(const std::string& _name);
	void Close();
	void Write(const std::string& _msg);

private:
	std::unique_ptr<std::ofstream> m_ostream;
};

file_log_policy::file_log_policy() :
		m_ostream(new std::ofstream()) {
}

void file_log_policy::Open(const std::string& _name) {
	m_ostream->open(_name.c_str(), std::ios_base::binary | std::ios_base::out);
	if (!m_ostream->is_open()) {
		throw(std::runtime_error("LOGGER: Unable to open an output stream"));
	}
}

void file_log_policy::Close() {
	if (m_ostream) {
		m_ostream->close();
	}
}

void file_log_policy::Write(const std::string& _msg) {
	(*m_ostream) << _msg << std::endl;
}

file_log_policy::~file_log_policy() {
	this->Close();
}

/*
 * Logger class
 */

template<typename log_policy>
class Logger {

public:
	Logger(const std::string _path);
	template<log_level level, typename ... Args> void print(Args ... args);
	void doprint();
	template<typename First, typename ... Rest> void doprint(First _param1, Rest ... args);

private:
	std::string get_time();
private:
	log_policy* m_policy;
	std::mutex m_writeLocker;
	std::stringstream m_logStream;
};

template<typename log_policy>
Logger<log_policy>::Logger(const std::string _path) {
	m_policy = new log_policy();
	if (!m_policy) {
		throw std::runtime_error("LOGGER: Unable to create the logger instance");
	}
	m_policy->Open(_path);
	m_policy->Write("============================================================================");
	m_policy->Write("				");
	m_policy->Write("============================================================================");
}

template<typename log_policy>
std::string Logger<log_policy>::get_time() {
	std::string time_str;
	time_t raw_time;
	time(&raw_time);
	time_str = ctime(&raw_time);
	//without the newline character
	return time_str.substr(0, time_str.size() - 1);
}

template<typename log_policy>
template<log_level level, typename ... Args> void Logger<log_policy>::print(Args ... args) {
	m_writeLocker.lock();
	m_logStream << "<" << this->get_time();
	switch (level) {
	case DEBUG: {
		m_logStream << " Debug";
	}
	break;
	case WARNING: {
		m_logStream << " Warning";
	}
	break;
	case ERROR: {
		m_logStream << " Error";
	}
	break;
	}
	m_logStream << ">\t";
	doprint(args...);
	m_writeLocker.unlock();
}

template<typename log_policy>
void Logger<log_policy>::doprint() {
	m_policy->Write(m_logStream.str());
	m_logStream.str("");
}

template<typename log_policy>
template<typename First, typename ... Rest> void Logger<log_policy>::doprint(First _param1, Rest ... args) {
	m_logStream << _param1;
	doprint(args...);
}
}

#endif /* LOGGER_HPP_ */
