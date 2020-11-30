#include "CyzArgumentManager.h"

OpenThreads::Mutex CyzArgumentManager::_mutex;
CyzArgumentManager* CyzArgumentManager::_instance = NULL;

CyzArgumentManager::CyzArgumentManager()
	: _useArgumentFlag(false)
{

}

CyzArgumentManager::~CyzArgumentManager()
{

}

CyzArgumentManager* CyzArgumentManager::getInstance()
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> locker(_mutex);
	if (_instance == NULL)
	{
		_instance = new CyzArgumentManager();
	}
	return _instance;
}

osg::ArgumentParser* CyzArgumentManager::getArgumentParser(int argc, char *argv[])
{
	osg::ArgumentParser* arguments = new osg::ArgumentParser(&argc, argv);
	arguments->getApplicationUsage()->setApplicationName(arguments->getApplicationName());
	arguments->getApplicationUsage()->setDescription(arguments->getApplicationName() + " is an example of osgOcean.");
	arguments->getApplicationUsage()->setCommandLineUsage(arguments->getApplicationName() + " [options] ...");
	arguments->getApplicationUsage()->addCommandLineOption("--windx <x>", "Wind X direction. Default 1.1");
	arguments->getApplicationUsage()->addCommandLineOption("--windy <y>", "Wind Y direction. Default 1.1");
	arguments->getApplicationUsage()->addCommandLineOption("--windSpeed <speed>", "Wind speed. Default: 12");
	arguments->getApplicationUsage()->addCommandLineOption("--depth <depth>", "Depth. Default: 10000");
	arguments->getApplicationUsage()->addCommandLineOption("--isNotChoppy", "Set the waves not choppy (by default they are).");
	arguments->getApplicationUsage()->addCommandLineOption("--choppyFactor <factor>", "How choppy the waves are. Default: 2.5");
	arguments->getApplicationUsage()->addCommandLineOption("--crestFoamHeight <height>", "How high the waves need to be before foam forms on the crest. Default: 2.2 ");
	arguments->getApplicationUsage()->addCommandLineOption("--oceanSurfaceHeight <z>", "Z position of the ocean surface in world coordinates. Default: 0.0");
	arguments->getApplicationUsage()->addCommandLineOption("--testCollision", "Test ocean surface collision detection by making a boat float on its surface.");
	arguments->getApplicationUsage()->addCommandLineOption("--disableShaders", "Disable use of shaders for the whole application. Also disables most visual effects as they depend on shaders.");

	return arguments;
}