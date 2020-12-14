/*
* This source file is part of the osgOcean library
* 
* Copyright (C) 2009 Kim Bale
* Copyright (C) 2009 The University of Hull, UK
* 
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.

* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
* http://www.gnu.org/copyleft/lesser.txt.
*/

#include <windows.h>
#include <string>
#include <vector>
#include <iostream>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <osgViewer/ViewerEventHandlers>

#include <osgGA/StateSetManipulator>

#include <osg/Notify>

#include <osgDB/ReadFile>

#include <osgShadow/ShadowedScene>
#include <osgShadow/ViewDependentShadowMap>

#include <osgOcean/OceanScene>
#include <osgOcean/Version>
#include <osgOcean/ShaderManager>

#include "SceneEventHandler.h"
#include "Scene.h"
#include "TextHUD.h"

#include "CyzBoatPositionCallback.h"
#include "CyzArgumentManager.h"
#include "CyzOceanManager.h"

int main(int argc, char *argv[])
{
	osg::notify(osg::NOTICE) << "osgOcean " << osgOceanGetVersion() << std::endl << std::endl;

	osg::ArgumentParser arguments(&argc, argv);

	unsigned int helpType = 0;

	float windx = 0.8f, windy = -0.8f;		// original 1.1
	osg::Vec2f windDirection(windx, windy);

	float windSpeed = 12.f;
	float depth = 1000000.f;
	float reflectionDamping = 0.35f;
	float scale = 1e-8;
	bool isChoppy = true;
	float choppyFactor = 2.5f;
	choppyFactor = -choppyFactor;
	float crestFoamHeight = 2.2f;
	double oceanSurfaceHeight = -10.0f;
	bool testCollision = true;  // false origianl
	bool disableShaders = false;
	bool useVBO = false;
	bool compositeViewer = false;	// false original
	bool firstViewLast = false;
	bool disableEffectsForSecondView = false;
	bool useShadows = false;		// false (original)
	bool useDebugDraw = false;

	// 初始视角（y正方向朝向屏幕外侧）
	osg::Vec3 initialCameraPosition(0, -50, 60);

	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(arguments);


	//------------------------------------------------------------------------
	// Set up the scene
	//------------------------------------------------------------------------

	std::string terrain_shader_basename = CyzOceanManager::getInstance()->getTerrainShaderBaseName(useShadows);
	osgOcean::ShaderManager::instance().enableShaders(!disableShaders);
	osg::ref_ptr<Scene> scene = new Scene(windDirection, windSpeed, depth, reflectionDamping, scale, isChoppy, choppyFactor, crestFoamHeight, useVBO, terrain_shader_basename);

	osg::ref_ptr<osg::Group> root = CyzOceanManager::getInstance()->getShadowRoot(scene, useShadows, disableShaders, useDebugDraw);
	if (root == NULL) return 1;

	CyzOceanManager::getInstance()->initScene(scene, disableShaders);

	scene->getOceanScene()->setOceanSurfaceHeight(oceanSurfaceHeight);

	root->addChild(scene->getScene());

	if (loadedModel.valid())
	{
		loadedModel->setNodeMask(scene->getOceanScene()->getNormalSceneMask() |
			scene->getOceanScene()->getReflectedSceneMask() |
			scene->getOceanScene()->getRefractedSceneMask() |
			CAST_SHADOW | RECEIVE_SHADOW);

		scene->getOceanScene()->addChild(loadedModel.get());
	}


	CyzOceanManager::getInstance()->loadBoat(scene, testCollision);

	//------------------------------------------------------------------------
	// Set up the viewer
	//------------------------------------------------------------------------

	osg::ref_ptr<osgViewer::ViewerBase> viewer;
	osgViewer::View* view = 0;

	osgViewer::Viewer* singleViewer = new osgViewer::Viewer;
	viewer = singleViewer;
	view = singleViewer;

	// Otherwise, a window with a single view.
	view->setUpViewInWindow(150, 150, 1024, 768, 0);
	view->setSceneData(root.get());

	view->addEventHandler(new osgViewer::StatsHandler);
	view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));

	osg::ref_ptr<TextHUD> hud = new TextHUD;
	// Add the HUD to the main view (if compositeViewer == true there will be a second one)
	view->getCamera()->addChild(hud->getHudCamera());

	view->addEventHandler(scene->getOceanSceneEventHandler());
	//view->addEventHandler(scene->getOceanSurface()->getEventHandler());

	// 海洋场景控制显示
	view->addEventHandler(new SceneEventHandler(scene.get(), hud.get(), view, initialCameraPosition));
	
	view->addEventHandler(new osgViewer::HelpHandler);
	view->getCamera()->setName("MainCamera");

	viewer->realize();

	while (!viewer->done())
	{
		viewer->frame();
	}

	return 0;
}
