

#include "MyThreadClass.h"

MyThreadClass::MyThreadClass() {}
MyThreadClass::~MyThreadClass() {}

void *MyThreadClass::InternalThreadEntryFunc(void * This)
{
	sigemptyset( & ((MyThreadClass *)This)->m_mask );
	sigaddset( & ((MyThreadClass *)This)->m_mask, SIGUSR1 );
	pthread_sigmask( SIG_BLOCK, & ((MyThreadClass *)This)->m_mask, NULL );
	((MyThreadClass *)This)->InternalThreadFunc();
	return NULL;
}


/** Returns true if the thread was successfully started, false if there was an error starting the thread */
bool MyThreadClass::StartInternalThread()
{
	return (pthread_create(&_thread, NULL, InternalThreadEntryFunc, this) == 0);
}

/** Will not return until the internal thread has exited. */
void MyThreadClass::WaitForInternalThreadToExit()
{
	(void) pthread_join(_thread, NULL);
}

void MyThreadClass::SignalMyThread(){
	if (m_sleeping) {
		printf("wake up\n");
		pthread_kill(_thread,SIGUSR1);
		printf("after kill wake up\n");
	}
}

void MyThreadClass::wait(unsigned int us){
	if (us<=1) return;
	timespec timeout;
	timeout.tv_sec = 0;
	timeout.tv_nsec = us*1000;
	m_sleeping=true;
	sigtimedwait( & m_mask, NULL, & timeout );
	m_sleeping=false;
}

double MyThreadClass::GetCurrentTime()
{
	TimeStamp currentTime;
	return (currentTime.update() - processStartTime) / (1000*1000.0);
}
