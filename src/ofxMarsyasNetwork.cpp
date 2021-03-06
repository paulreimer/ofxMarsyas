#include "ofxMarsyasNetwork.h"
#include "ofMath.h"

Marsyas::MarSystemManager ofxMarsyasNetwork::mng;

ofxMarsyasNetwork::ofxMarsyasNetwork(string name)
: Marsyas::Series(name)
{
	targetRate = rate = 0;
#ifdef _POSIX_VERSION
	targetPriority = priority = 1;
#endif
}

ofxMarsyasNetwork::~ofxMarsyasNetwork()
{
	stop();
}

void ofxMarsyasNetwork::threadedFunction() 
{
	while( isThreadRunning() )
	{
#ifdef _POSIX_VERSION
		if (priority != targetPriority)
		{
			run();
			priority = targetPriority;
		}
#endif

		if (lock())
		{
			tick();
			thisTick = ofGetSystemTime();
      // update calculated rate
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

#ifdef _POSIX_VERSION
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

#ifdef _POSIX_VERSION
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

