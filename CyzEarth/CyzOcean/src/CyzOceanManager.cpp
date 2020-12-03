#include "windows.h"

#include "CyzOceanManager.h"
#include "OpenThreads/ScopedLock"

#include "osgShadow/ViewDependentShadowMap"

#include "osgOcean/ShaderManager"


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

osg::Group* CyzOceanManager::getShadowRoot(Scene* scene, bool useShadows, bool disableShaders, bool useDebugDraw)
{
	osg::Group* root = NULL;
	if (useShadows)
	{
		osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene;
		root = shadowedScene;

		shadowedScene->setCastsShadowTraversalMask(CAST_SHADOW);
		shadowedScene->setReceivesShadowTraversalMask(RECEIVE_SHADOW);

		osgShadow::ShadowSettings* shadowSettings = new osgShadow::ShadowSettings;
		shadowSettings->setTextureSize(osg::Vec2s(1024, 1024));
		shadowSettings->setBaseShadowTextureUnit(7);
		shadowSettings->setLightNum(scene->getLight()->getLightNum());
		shadowSettings->setShaderHint(osgShadow::ShadowSettings::NO_SHADERS);
		shadowSettings->setDebugDraw(useDebugDraw);

		osgShadow::ViewDependentShadowMap* shadowTechnique = new osgShadow::ViewDependentShadowMap;
		shadowedScene->setShadowSettings(shadowSettings);
		shadowedScene->setShadowTechnique(shadowTechnique);

		if (!disableShaders)
		{
			osg::Shader* vs = osgDB::readShaderFile(osg::Shader::VERTEX, "osgOcean_ocean_scene_lispsm.vert");
			osg::ref_ptr<osg::Program> program = new osg::Program;

			if (vs)
			{
				program->addShader(vs);
			}
			else
			{
				osg::notify(osg::WARN) << "osgOcean: Could not read shader from file: osgOcean_ocean_scene_lispsm.vert" << std::endl;
			}

			osg::Shader* fs = osgDB::readShaderFile(osg::Shader::FRAGMENT, "osgOcean_ocean_scene_lispsm.frag");

			if (fs)
			{
				program->addShader(fs);
			}
			else
			{
				osg::notify(osg::WARN) << "osgOcean: Could not read shader from file: osgOcean_ocean_scene_lispsm.frag" << std::endl;
			}

			if (program->getNumShaders() == 2)
			{
				shadowedScene->getOrCreateStateSet()->setAttributeAndModes(program.get(), osg::StateAttribute::ON);
			}

			osg::Program* sceneProgram = osgOcean::ShaderManager::instance().createProgram("scene_shader",
				"osgOcean_ocean_scene_lispsm.vert", "osgOcean_ocean_scene_lispsm.frag", "", "");
			scene->getOceanScene()->setDefaultSceneShader(sceneProgram);
		}

		scene->getOceanScene()->getOceanTechnique()->setNodeMask(scene->getOceanScene()->getOceanTechnique()->getNodeMask() & ~CAST_SHADOW & ~RECEIVE_SHADOW);
		scene->getOceanScene()->getOceanCylinder()->getParent(0)->setNodeMask(scene->getOceanScene()->getOceanCylinder()->getParent(0)->getNodeMask() & ~CAST_SHADOW & ~RECEIVE_SHADOW);
	}
	else
	{
		root = new osg::Group;
	}
	return root;
}

bool CyzOceanManager::initScene(Scene* scene, bool disableShaders)
{
	if (!disableShaders) return false;

	if (scene)
	{
		// Disable all special effects that depend on shaders.

		// These depend on fullscreen RTT passes and shaders to do their effects.
		scene->getOceanScene()->enableDistortion(false);
		scene->getOceanScene()->enableGlare(false);
		scene->getOceanScene()->enableUnderwaterDOF(false);

		// These are only implemented in the shader, with no fixed-pipeline equivalent
		scene->getOceanScene()->enableUnderwaterScattering(false);
		// For these two, we might be able to use projective texturing so it would
		// work on the fixed pipeline?
		scene->getOceanScene()->enableReflections(false);
		scene->getOceanScene()->enableRefractions(false);
		scene->getOceanScene()->enableGodRays(false);  // Could be done in fixed pipeline?
		scene->getOceanScene()->enableSilt(false);     // Could be done in fixed pipeline?

		return true;
	}
	return false;
}