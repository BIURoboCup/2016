#include "Log.h"

using namespace cvflann;

Log* Log::m_instance = NULL;
Log* Log::GetInstance()
{
    if(m_instance == NULL)
    {
    	m_instance = new Log();
        return m_instance;
    }
    else
    {
        return m_instance;
    }
}

void Log::Info(string message, string category)
{
	Logger::info(FormatMessage(message,category).c_str());
}

void Log::Warning(string message, string category)
{
	Logger::warn(FormatMessage(message,category).c_str());
}
void Log::Error(string message, string category)
{
	Logger::error(FormatMessage(message,category).c_str())
}

void Log::Fatal(string message, string category)
{
	Logger::fatal(FormatMessage(message,category).c_str());
}

string Log::FormatMessage(string message, string category)
{
	char* x = new char[message.length() + category.length() + 32];

	sprintf(x, "[%s] %s - %s", GetCurrentTimestampString().c_str(), category.c_str(), message.c_str());

	string str = x;
	delete[] x;

	return str;
}

Log::Log()
{
	string logPath = "/home/robot/Logs/log_" + GetCurrentTimestampString();
	Logger::setDestination(logPath.c_str());
	Logger::setLevel(4);
}

Log::~Log()
{
}

string Log::GetCurrentTimestampString()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);

	string timestamp(buffer);
	return timestamp;
}
