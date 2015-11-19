#ifndef MYTIMEVAL_H_
#define MYTIMEVAL_H_

#include <iostream>
#include <sys/time.h>
using namespace std;

class myTimeval: public timeval
{
public:
	long operator - (const myTimeval& other); //get difference between 2 myTimeval in usec
	myTimeval operator + (const long& usec);  //add usec
	myTimeval operator +=(const long& usec);
	myTimeval operator =(const long& usec);
	friend ostream& operator << (ostream& out,const myTimeval& me); //print "seconds:us"
	bool operator == (const myTimeval& other);
	bool operator != (const myTimeval& other);

	myTimeval update(); //update this myTimeval to the current time
};

class TimeStamp: public myTimeval {
public:
	TimeStamp();  //creates a TimeStamp with the current time. see example below
};


/*
usage:
	myTimeval mtv;
	mtv.update;
	long u_sec=100;
	usleep(u_sec);

	long u_secPassed=TimeStamp()-mtv;

	cout<<TimeStamp()<<" something important happend now!\t";
	cout<<"check again in "<<mtv+u_secPassed<<endl;
	usleep(u_sec);

*/
#endif
