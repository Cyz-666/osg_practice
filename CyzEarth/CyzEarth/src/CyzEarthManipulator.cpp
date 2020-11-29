#include <windows.h>

#include "CyzEarthManipulator.h"

CyzEarthManipulator::CyzEarthManipulator()
{

}

CyzEarthManipulator::~CyzEarthManipulator()
{

}

bool CyzEarthManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	osgEarth::EarthManipulator::handle(ea, us);
	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
	{
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_B)
		{
			flyToBuston();
			return false;
		}

		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_C)
		{
			flyToChina();
			return false;
		}
	}

	//**************EVENT TYPES**********************
	//case osgGA::GUIEventAdapter::PUSH:
	//case osgGA::GUIEventAdapter::RELEASE:
	//case osgGA::GUIEventAdapter::DOUBLECLICK:
	//case osgGA::GUIEventAdapter::MOVE: // MOVE not currently bindable
	//case osgGA::GUIEventAdapter::DRAG:
	//case osgGA::GUIEventAdapter::KEYUP:
		
	
}

void CyzEarthManipulator::flyToBuston()
{
	setViewpoint(_bustonViewpoint, 5.0);
}

void CyzEarthManipulator::flyToChina()
{
	setViewpoint(_ChinaViewpoint, 1.0);
}