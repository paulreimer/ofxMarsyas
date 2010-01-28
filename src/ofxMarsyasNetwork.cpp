#include "ofxMarsyasNetwork.h"

Marsyas::MarSystemManager ofxMarsyasNetwork::mng;

ofxMarsyasNetwork::ofxMarsyasNetwork(string name)
: Marsyas::Series(name)
{
	targetRate = rate = 0;
	targetPriority = priority = 1;
}

ofxMarsyasNetwork::~ofxMarsyasNetwork()
{
	stop();
}

void ofxMarsyasNetwork::threadedFunction() 
{
	while( isThreadRunning() )
	{
		if (priority != targetPriority)
		{
			run();
			priority = targetPriority;
		}

		if (lock())
		{
			tick();
			thisTick = ofGetSystemTime();
			rate = ofLerp(rate, 1000.0/(thisTick-lastTick), 0.001);		
			lastTick = thisTick;

			update();
			unlock();
		}
		else {
			ofSleepMillis(20);
		}

		if (targetRate>0)
			ofSleepMillis(1000.0/targetRate);
	}
}

void ofxMarsyasNetwork::run() 
{
	if (isThreadRunning())
		stopThread();

#ifndef TARGET_WIN32
	pthread_attr_t tattr;
	int oldprio, policy;
	sched_param param;

	pthread_getschedparam(pthread_self(), &policy, &param);
	oldprio = param.sched_priority;

	// set a new priority
	param.sched_priority *= oldprio * targetPriority;

	// set the new scheduling param
	pthread_attr_setschedparam (&tattr, &param);  
#endif

	startThread(true, false); // blocking, non-verbose

#ifndef TARGET_WIN32
	// restore the old priority to prevent scaling from escalating
	param.sched_priority = oldprio;
	// set the old scheduling param
	pthread_attr_setschedparam(&tattr, &param);
#endif
}

void ofxMarsyasNetwork::stop() 
{
	stopThread();	
}

