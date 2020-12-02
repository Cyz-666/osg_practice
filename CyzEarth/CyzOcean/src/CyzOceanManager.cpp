#include "windows.h"

#include "CyzOceanManager.h"
#include "OpenThreads/ScopedLock"


CyzOceanManager* CyzOceanManager::_instance = NULL;
OpenThreads::Mutex CyzOceanManager::_mutex;

CyzOceanManager::CyzOceanManager()
{

}

CyzOceanManager::~CyzOceanManager()
{

}

CyzOceanManager* CyzOceanManager::getInstance()
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> scopedLocker(_mutex);
	if (_instance == NULL)
	{
		_instance = new CyzOceanManager();
	}
	return _instance;
}

std::string CyzOceanManager::getTerrainShaderBaseName(bool useShadows)
{
	std::string terrain_shader_basename = "terrain";

	if (useShadows) {
		terrain_shader_basename = "terrain_lispsm";
	}

	return terrain_shader_basename;
}