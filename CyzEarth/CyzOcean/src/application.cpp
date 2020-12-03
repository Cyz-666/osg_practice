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

    osg::ArgumentParser arguments(&argc,argv);
    
    unsigned int helpType = 0;
    
    float windx = 0.8f, windy = 0.8f;		// original 1.1
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

    osg::Vec3 initialCameraPosition(0,-10,20);

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

    root->addChild( scene->getScene() );

    if (loadedModel.valid())
    {
        loadedModel->setNodeMask( scene->getOceanScene()->getNormalSceneMask()    | 
                                  scene->getOceanScene()->getReflectedSceneMask() | 
                                  scene->getOceanScene()->getRefractedSceneMask() |
                                  CAST_SHADOW | RECEIVE_SHADOW );

        scene->getOceanScene()->addChild(loadedModel.get());
    }

    if (testCollision)
    {
        osgDB::Registry::instance()->getDataFilePathList().push_back("resources/boat");
        //const std::string filename = "boat.3DS";
        const std::string filename = "craft-small.3DS";
        osg::ref_ptr<osg::Node> boat = osgDB::readNodeFile(filename);

        if(boat.valid())
        {
            boat->setNodeMask( scene->getOceanScene()->getNormalSceneMask()    | 
                               scene->getOceanScene()->getReflectedSceneMask() | 
                               scene->getOceanScene()->getRefractedSceneMask() |
                               CAST_SHADOW | RECEIVE_SHADOW );

            osg::ref_ptr<osg::MatrixTransform> boatTransform = new osg::MatrixTransform;
			osg::Matrix boatMatrix = osg::Matrix::translate(osg::Vec3f(0.0f, 360.0f, 0.0f)) * osg::Matrix::scale(0.015, 0.015, 0.015);
			boatTransform->addChild(boat.get());
            boatTransform->setMatrix(boatMatrix);
            boatTransform->setUpdateCallback( new CyzBoatPositionCallback(scene->getOceanScene()) );

            scene->getOceanScene()->addChild(boatTransform.get());   
        }
    }

    //------------------------------------------------------------------------
    // Set up the viewer
    //------------------------------------------------------------------------

    osg::ref_ptr<osgViewer::ViewerBase> viewer;
    osgViewer::View* view = 0;
    if (compositeViewer)
    {
        viewer = new osgViewer::CompositeViewer;
        view = new osgViewer::View;

        // If testing a composite viewer, we'll make a splitscreen window with 
        // 2 views side by side.
        view->setUpViewInWindow( 50, 50, 1600, 768, 0 );
        view->getCamera()->setViewport(0, 0, 800, 768);
        double fovy, aspectRatio, zNear, zFar;
        view->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
        view->getCamera()->setProjectionMatrixAsPerspective(fovy, 800.0/768.0, zNear, zFar);

        // Create a second view
        osg::ref_ptr<osgViewer::View> view2 = new osgViewer::View;
        view2->getCamera()->setGraphicsContext(view->getCamera()->getGraphicsContext());
        view2->getCamera()->setViewport(800, 0, 800, 768);
        osgGA::TrackballManipulator* tb = new osgGA::TrackballManipulator;
        tb->setHomePosition( osg::Vec3f(0.f,0.f,20.f), osg::Vec3f(0.f,20.f,20.f), osg::Vec3f(0,0,1) );
        view2->setCameraManipulator( tb );
        view2->setSceneData(root.get());
        view2->addEventHandler( new osgGA::StateSetManipulator( view2->getCamera()->getOrCreateStateSet() ) );

        if (disableEffectsForSecondView)
        {
            scene->getOceanScene()->enableRTTEffectsForView(view2.get(), false);
        }

        // The order here is important, the last view added to the viewer (so 
        // presumably the last one to be rendered by the viewer) controls the LOD 
        // of ocean tiles, as well as which view gets refraction effects 
        // (refraction and height map). Reflection doesn't seem to depend on this 
        // at all, it works correctly in both views.
        osgViewer::CompositeViewer* compositeViewer = dynamic_cast<osgViewer::CompositeViewer*>(viewer.get());

        if (firstViewLast)
        {
            // If we explicitly asked that the main view be added last to the 
            // viewer, we'll see that the refraction applies to it, and it 
            // controls ocean tile LOD.
            compositeViewer->addView(view2.get());
            compositeViewer->addView(view);
        }
        else
        {
            // The "normal" case in most applications I've seen is that the 
            // main view is created and added first, and then inset views are
            // created and added. So without the --firstViewLast argument, this
            // is what we'll demonstrate. The main view won't have refraction
            // effects, the inset view will.
            compositeViewer->addView(view);
            compositeViewer->addView(view2.get());
        }

        // Post-render RTT effects don't work at all when using multiple 
        // views. This is not an immediate concern for us because we don't
        // use them anyways in our software.
        scene->getOceanScene()->enableGodRays(false);
        scene->getOceanScene()->enableUnderwaterDOF(false);
        scene->getOceanScene()->enableDistortion(false);
        scene->getOceanScene()->enableGlare(false);
    }
    else
    {
        osgViewer::Viewer* singleViewer = new osgViewer::Viewer;
        viewer = singleViewer;
        view = singleViewer;

        // Otherwise, a window with a single view.
        view->setUpViewInWindow( 150, 150, 1024, 768, 0 );
    }

    view->setSceneData( root.get() );

    view->addEventHandler( new osgViewer::StatsHandler );
    view->addEventHandler( new osgGA::StateSetManipulator( view->getCamera()->getOrCreateStateSet() ) );

    osg::ref_ptr<TextHUD> hud = new TextHUD;
    // Add the HUD to the main view (if compositeViewer == true there will be a second one)
    view->getCamera()->addChild( hud->getHudCamera() );

    view->addEventHandler(scene->getOceanSceneEventHandler());
    view->addEventHandler(scene->getOceanSurface()->getEventHandler());

    view->addEventHandler( new SceneEventHandler(scene.get(), hud.get(), view, initialCameraPosition ) );
    view->addEventHandler( new osgViewer::HelpHandler );
    view->getCamera()->setName("MainCamera");

    viewer->realize();

    while(!viewer->done())
    {
        viewer->frame();    
    }

    return 0;
}
