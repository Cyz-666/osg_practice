#pragma once
#ifndef CYZ_OCEAN_MANAGER_H_
#define CYZ_OCEAN_MANAGER_H_

#include "osg/Group"

#include "osgGA/CameraManipulator"
#include "osgGA/GUIEventAdapter"
#include "osgGA/GUIActionAdapter"

#include "osgOcean/OceanScene"

#include "OpenThreads/Mutex"

#include "osgShadow/ShadowedScene"

#include "Scene.h"

class CyzOceanManager 
{
public:
	~CyzOceanManager();
	static CyzOceanManager* getInstance();

	// 
	std::string getTerrainShaderBaseName(bool useShadows);

	osg::Group* getShadowRoot(Scene* scene, bool useShadows, bool disableShaders, bool useDebugDraw);

	bool initScene(Scene* scene, bool disableShaders);

	// ¼ÓÔØ´¬Ä£ÐÍ
	bool loadBoat(Scene* scene, bool testCollision);
private:
	CyzOceanManager();
private:
	static OpenThreads::Mutex _mutex;
	static CyzOceanManager* _instance;
};
#endif // CYZ_OCEAN_MANAGER_H_