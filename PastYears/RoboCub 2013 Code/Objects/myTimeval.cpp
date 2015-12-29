#include "myTimeval.h"

long myTimeval::operator - (const myTimeval& other) {
	long sec=tv_sec-other.tv_sec;
	long usec = (long)tv_usec - (long)other.tv_usec;

	return (sec*1000000+usec);
}

myTimeval myTimeval::operator + (const long& usec) {
	myTimeval my;
	my.tv_usec=(tv_usec+usec)%1000000;
	my.tv_sec=tv_sec+(tv_usec+usec)/1000000;
	return my;
}

myTimeval myTimeval::operator += (const long& usec) {
	long addedSec=(tv_usec+usec)/1000000;
	tv_usec=(tv_usec+usec)%1000000;
	tv_sec+=addedSec;
	return *this;
}

myTimeval myTimeval::operator =(const long& usec){
	long addedSec = (usec)/1000000;
	tv_usec = (usec)%1000000;
	tv_sec =addedSec;
	return *this;
}

bool myTimeval::operator == (const myTimeval& other)
{
	if ((tv_sec == other.tv_sec) && (tv_usec==other.tv_usec))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool myTimeval::operator != (const myTimeval& other)
{
	if ((tv_sec == other.tv_sec) && (tv_usec==other.tv_usec))
	{
		return false;
	}
	else
	{
		return true;
	}
}
myTimeval myTimeval::update() {
	gettimeofday(this,NULL);
	return *this;
}

ostream& operator << (ostream& out,const myTimeval& me){
	return	out<<me.tv_sec<<":"<<me.tv_usec;
}

TimeStamp::TimeStamp() {
	update();
}
