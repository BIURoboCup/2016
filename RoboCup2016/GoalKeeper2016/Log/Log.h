#include "../Includes.h"
#include "opencv2/flann/logger.h"

class Log
{
public:
	// Singletone
	static Log* GetInstance();
	~Log();

	void Info(string message, string category);
	void Warning(string message, string category);
	void Error(string message, string category);
	void Fatal(string message, string category);

private:
	Log();
	static Log* m_instance;
	string GetCurrentTimestampString();
	string FormatMessage(string message, string category);
};


