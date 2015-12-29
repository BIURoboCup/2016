#ifndef MYTHREADCLASS_H_
#define MYTHREADCLASS_H_

#include "IncludesFile.h"
#define MAX_VALID_TIME_BALL_LOCATION (3*1000*1000)


extern myTimeval processStartTime;
extern bool exitProgram;

class MyThreadClass
{

private:

	static void * InternalThreadEntryFunc(void * This);
	static void* schedualerThreadEntryFunc(void * This);
	pthread_t _thread;
	sigset_t m_mask;
	bool m_sleeping;

protected:
	/** Implement this method in your subclass with the code you want your thread to run. */
	virtual void InternalThreadFunc() = 0;
	void wait(unsigned int ns);

public:
	MyThreadClass(); //empty
	bool StartInternalThread();
	void WaitForInternalThreadToExit();
	void SignalMyThread();
	virtual ~MyThreadClass();
	double GetCurrentTime();
};

#endif /* MYTHREADFUNC_H_ */
