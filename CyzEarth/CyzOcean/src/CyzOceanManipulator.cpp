#include "windows.h"
#include "CyzOceanManipulator.h"

CyzOceanManipulator::CyzOceanManipulator(osg::Node* boat)
	: _boatNode(NULL)
{
	if (boat)
	{
		_boatNode = boat;
	}
}

CyzOceanManipulator::~CyzOceanManipulator()
{

}

bool CyzOceanManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
	{
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_A)
		{
		}
	}


	return false;
}

// 前进
bool CyzOceanManipulator::processMoveForward()
{
	osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(_boatNode);
	

	return false;
}

// 后退
bool CyzOceanManipulator::processMoveBackward()
{

	return false;
}

// 左移
bool CyzOceanManipulator::processMoveLeft()
{

	return false;
}

// 右移
bool CyzOceanManipulator::processMoveRight()
{

	return false;
}

// 左转弯
bool CyzOceanManipulator::processTurnLeft()
{

	return false;
}

// 右转弯
bool CyzOceanManipulator::processTurnRight()
{

	return false;
}