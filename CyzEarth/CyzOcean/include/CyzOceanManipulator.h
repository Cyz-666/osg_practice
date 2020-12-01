#pragma once
#ifndef CYZ_OCEAN_MANIPULATOR_H_
#define CYZ_OCEAN_MANIPULATOR_H_

#include "osgGA/CameraManipulator"
#include "osgGA/GUIEventAdapter"
#include "osgGA/GUIActionAdapter"

#include "osgOcean/OceanScene"

class CyzOceanManipulator : public osgGA::CameraManipulator
{
public:
	CyzOceanManipulator(osg::Node* boat);
	~CyzOceanManipulator();
	/** Handle events, return true if handled, false otherwise. */
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

private:
	// ǰ��
	bool processMoveForward();

	// ����
	bool processMoveBackward();

	// ����
	bool processMoveLeft();

	// ����
	bool processMoveRight();

	// ��ת��
	bool processTurnLeft();

	// ��ת��
	bool processTurnRight();

private:
	osg::Node* _boatNode;
	osgOcean::OceanScene* _oceanScene;
};
#endif // CYZ_OCEAN_MANIPULATOR_H_