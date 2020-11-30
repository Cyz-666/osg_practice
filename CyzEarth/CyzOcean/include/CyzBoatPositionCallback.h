#pragma once
#ifndef CYZ_BOAT_POSITION_CALLBACK_H_
#define CYZ_BOAT_POSITION_CALLBACK_H_

#include "windows.h"
#include "osg/NodeCallback"
#include "osgOcean/OceanScene"


class CyzBoatPositionCallback : public osg::NodeCallback
{
public:
	CyzBoatPositionCallback(osgOcean::OceanScene* oceanScene);
	~CyzBoatPositionCallback();

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

private:
	osg::Matrix getMoveBoatMat(osg::Matrix mat);

	double getXDistance(const osg::Vec3f& pos);

	double getYDistance(const osg::Vec3f& pos);

	double getRotationAngle();
private:
	// 1. ����y������Զ(���)���о���
	const double Y_MAX_TRAVEL_DISTENCE = 65000;
	const double Y_MIN_TRAVEL_DISTANCE = 40000;

	// 2. ����x������Զ(���)���о���
	const double X_MAX_TRAVEL_DISTENCE = 5000;
	const double X_MIN_TRAVEL_DISTENCE = 4500;

	// 3. �����ƶ�����
	const double MOVE_STEP = 1000;

	// 4. Y(X)����Ҫ���ӵľ���
	static double yAddDistance;
	static double xAddDistance;

	// 5. ����ģ�͵Ĵ�С
	const osg::Matrix _scale = osg::Matrix::scale(0.004, 0.004, 0.004);

	// ת�䶯������
	// �ж��Ƿ�����ת��
	bool _isTurning;

	std::string _direction;

	osg::observer_ptr<osgOcean::OceanScene> _oceanScene;
};

#endif // CYZ_BOAT_POSITION_CALLBACK_H_