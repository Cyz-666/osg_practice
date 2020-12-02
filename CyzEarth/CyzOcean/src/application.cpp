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

    std::string terrain_shader_basename = "terrain";

    if (useShadows){
        terrain_shader_basename = "terrain_lispsm";
    }

    osgOcean::ShaderManager::instance().enableShaders(!disableShaders);
    osg::ref_ptr<Scene> scene = new Scene(windDirection, windSpeed, depth, reflectionDamping, scale, isChoppy, choppyFactor, crestFoamHeight, useVBO, terrain_shader_basename);

    osg::ref_ptr<osg::Group> root;

    if (useShadows)
    {
        osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene;
        root = shadowedScene;

        shadowedScene->setCastsShadowTraversalMask(CAST_SHADOW);
        shadowedScene->setReceivesShadowTraversalMask(RECEIVE_SHADOW);

        osgShadow::ShadowSettings* shadowSettings = new osgShadow::ShadowSettings;
        shadowSettings->setTextureSize(osg::Vec2s(1024,1024));
        shadowSettings->setBaseShadowTextureUnit(7);
        shadowSettings->setLightNum(scene->getLight()->getLightNum());
        shadowSettings->setShaderHint(osgShadow::ShadowSettings::NO_SHADERS);
        shadowSettings->setDebugDraw(useDebugDraw);

        osgShadow::ViewDependentShadowMap* shadowTechnique = new osgShadow::ViewDependentShadowMap;
        shadowedScene->setShadowSettings(shadowSettings);
        shadowedScene->setShadowTechnique(shadowTechnique);

        if (!disableShaders)
        {
            osg::Shader* vs = osgDB::readShaderFile(osg::Shader::VERTEX,"osgOcean_ocean_scene_lispsm.vert");
            osg::ref_ptr<osg::Program> program = new osg::Program;
            
            if (vs)
            {
                program->addShader(vs);
            }
            else
            {
                osg::notify(osg::WARN) << "osgOcean: Could not read shader from file: osgOcean_ocean_scene_lispsm.vert"  << std::endl;
            }

            osg::Shader* fs = osgDB::readShaderFile(osg::Shader::FRAGMENT,"osgOcean_ocean_scene_lispsm.frag");

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

    if (disableShaders)
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
    }

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
