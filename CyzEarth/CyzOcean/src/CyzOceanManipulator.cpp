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

// ǰ��
bool CyzOceanManipulator::processMoveForward()
{
	osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(_boatNode);
	

	return false;
}

// ����
bool CyzOceanManipulator::processMoveBackward()
{

	return false;
}

// ����
bool CyzOceanManipulator::processMoveLeft()
{

	return false;
}

// ����
bool CyzOceanManipulator::processMoveRight()
{

	return false;
}

// ��ת��
bool CyzOceanManipulator::processTurnLeft()
{

	return false;
}

// ��ת��
bool CyzOceanManipulator::processTurnRight()
{

	return false;
}