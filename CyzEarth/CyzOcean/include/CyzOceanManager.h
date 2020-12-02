#pragma once
#ifndef CYZ_OCEAN_MANAGER_H_
#define CYZ_OCEAN_MANAGER_H_

#include "osgGA/CameraManipulator"
#include "osgGA/GUIEventAdapter"
#include "osgGA/GUIActionAdapter"

#include "osgOcean/OceanScene"

#include "OpenThreads/Mutex"

class CyzOceanManager 
{
public:
	~CyzOceanManager();
	CyzOceanManager* getInstance();

	// 
	std::string getTerrainShaderBaseName(bool useShadows);
private:
	CyzOceanManager();
private:
	static OpenThreads::Mutex _mutex;
	static CyzOceanManager* _instance;
};
#endif // CYZ_OCEAN_MANAGER_H_