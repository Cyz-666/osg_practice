#pragma once
#ifndef CYZ_ARGUMENT_MANAGER_H_
#define CYZ_ARGUMENT_MANAGER_H_

#include "windows.h"
#include "osg/NodeCallback"
#include "osgOcean/OceanScene"

#include "OpenThreads/ScopedLock"

struct CyzOceanArgument
{
	double windx = 0.8;
	double windy = 0.8;		// original 1.1
	double windSpeed = 12.f;
	double depth = 1000000.f;
	double reflectionDamping = 0.35f;
	double scale = 1e-8;
	bool isChoppy = true;
	double choppyFactor = -2.5f;
	double crestFoamHeight = 2.2f;
	double oceanSurfaceHeight = -10.0f;
	bool testCollision = true;
	bool disableShaders = false;
	bool useVBO = false;
	bool compositeViewer = false;
	bool firstViewLast = false;
	bool disableEffectsForSecondView = false;
	bool useShadows = false;
	bool useDebugDraw = false;

};

class CyzArgumentManager
{
public:
	static CyzArgumentManager* getInstance();
	~CyzArgumentManager();

	osg::ArgumentParser* getArgumentParser(int argc, char *argv[]);
private:
	CyzArgumentManager();

private:
	static OpenThreads::Mutex _mutex;
	static CyzArgumentManager* _instance;
	bool _useArgumentFlag;
};

#endif // CYZ_ARGUMENT_MANAGER_H_