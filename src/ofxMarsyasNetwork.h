#pragma once

#include "ofThread.h"
#include "marsyas/Series.h"
#include "marsyas/MarSystemManager.h"

class ofxMarsyasNetwork
: public ofThread
, public Marsyas::Series
{
public:
	ofxMarsyasNetwork(string name = "net");
	virtual ~ofxMarsyasNetwork();

	virtual void setup()  {};
	virtual void update() {};
	virtual void draw()   {};
	
	void threadedFunction();
	void run();
	void stop();
	
	static Marsyas::MarSystemManager mng;

	float targetRate;	
	float rate;
	
	unsigned long thisTick;
	unsigned long lastTick;

#ifdef _POSIX_VERSION
	float targetPriority;
	float priority;
#endif
};

