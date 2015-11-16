/*
 * timer.hpp
 *
 *  Created on: Aug 6, 2015
 *      Author: darwini5
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
using namespace std;

class pf_Timer {
	timespec _s, _e;

	timespec diff(timespec start, timespec end)
	{
		timespec temp;
		if ((end.tv_nsec-start.tv_nsec)<0) {
			temp.tv_sec = end.tv_sec-start.tv_sec-1;
			temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
		} else {
			temp.tv_sec = end.tv_sec-start.tv_sec;
			temp.tv_nsec = end.tv_nsec-start.tv_nsec;
		}
		return temp;
	}
public:
	void start() {
		clock_gettime(CLOCK_MONOTONIC, &_s);
	}

	double nanostop() {
		clock_gettime(CLOCK_MONOTONIC, &_e);
		timespec _d = diff(_s,_e);
		return _d.tv_sec * pow(10,9) + _d.tv_nsec;
	}
};

/*
class Timer {
private:

    timeval startTime;

public:

    void start(){
    	clock_gettime(CLOCK_MONOTONIC,NULL);
        gettimeofday(&startTime, NULL);
    }

    double stop(){
        timeval endTime;
        long seconds, useconds;
        double duration;

        gettimeofday(&endTime, NULL);

        seconds  = endTime.tv_sec  - startTime.tv_sec;
        useconds = endTime.tv_usec - startTime.tv_usec;

        duration = seconds + useconds/1000000.0;

        return duration;
    }

    static void printTime(double duration){
        printf("%5.6f seconds\n", duration);
    }
};
*/



#endif /* TIMER_HPP_ */
