#include "CyzBoatPositionCallback.h"

double CyzBoatPositionCallback::yAddDistance = 40000;
double CyzBoatPositionCallback::xAddDistance = 0;

CyzBoatPositionCallback::CyzBoatPositionCallback(osgOcean::OceanScene* oceanScene)
	: _oceanScene(oceanScene)
	, _direction("y+")
	, _isTurning(false)
{

}

CyzBoatPositionCallback::~CyzBoatPositionCallback()
{

}

osg::Matrix CyzBoatPositionCallback::getMoveBoatMat(osg::Matrix mat)
{
	static double addMov = 1000;
	static double addStep = 100;
	osg::Vec3d pos = mat.getTrans();

	osg::Vec3f normal;
	// Get the ocean surface height at the object's position, note
	// that this only considers one point on the object (the object's
	// geometric center) and not the whole object.
	float height = _oceanScene->getOceanSurfaceHeightAt(pos.x(), pos.y(), &normal);

	double posX = pos.x();
	double posY = pos.y();
	posY = posY + 40000 + addMov;
	height = height + 20;
	osg::Vec3d translatePos(posX, posY, height);
	mat.makeTranslate(translatePos);
	addMov += addStep;
	osg::Matrix rot;
	rot.makeIdentity();

	if (_direction == "y+")
	{
		if (posY < Y_MAX_TRAVEL_DISTENCE)
		{
			rot.makeRotate(normal.x(), osg::Vec3f(1.0f, 0.0f, 0.0f),
				normal.y(), osg::Vec3f(0.0f, 1.0f, 0.0f),
				osg::PI_2, osg::Vec3f(0.0f, 0.0f, 1.0f));	//
			addStep = 100;
		}
		else
		{
			_direction = "y-";
		}
	}
	if (_direction == "y-")
	{
		if (posY > Y_MIN_TRAVEL_DISTANCE)
		{
			rot.makeRotate(normal.x(), osg::Vec3f(1.0f, 0.0f, 0.0f),
				normal.y(), osg::Vec3f(0.0f, 1.0f, 0.0f),
				-osg::PI_2, osg::Vec3f(0.0f, 0.0f, 1.0f));	//
			addStep = -100;
		}
		else
		{
			_direction = "y+";
		}
	}

	osg::Matrix scale = osg::Matrix::scale(0.004, 0.004, 0.004);
	mat = rot * mat*scale;
	return mat;
}

void CyzBoatPositionCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	
	if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR) {
		osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);
		if (!mt || !_oceanScene.valid()) return;

		osg::NodePath nodePath = nv->getNodePath();
		double nodePathSize = nodePath.size();
		osg::Matrix mat = osg::computeLocalToWorld(nodePath);		
		mat = getMoveBoatMat(mat);
		mt->setMatrix(mat);
	}

	traverse(node, nv);

}


double CyzBoatPositionCallback::getXDistance(const osg::Vec3f& pos)
{
	if (_direction == "y+")
	{
		return X_MIN_TRAVEL_DISTENCE;
	}

	if (_direction == "y-")
	{
		return X_MAX_TRAVEL_DISTENCE;
	}

	if (_direction == "x+")
	{
		xAddDistance += MOVE_STEP;
		if (xAddDistance >= X_MAX_TRAVEL_DISTENCE)
		{
			xAddDistance = X_MAX_TRAVEL_DISTENCE;
			_direction = "y-";
		}
	}

	if (_direction == "x-")
	{
		xAddDistance -= MOVE_STEP;
		if (xAddDistance <= X_MIN_TRAVEL_DISTENCE)
		{
			xAddDistance = X_MIN_TRAVEL_DISTENCE;
			_direction = "y+";
		}
	}

	return xAddDistance;
}

double CyzBoatPositionCallback::getYDistance(const osg::Vec3f& pos)
{
	// 如果不到y方向最大值，继续向y方向前行
	if (_direction == "y+")
	{
		yAddDistance += pos.y();
		yAddDistance += MOVE_STEP;
		if (yAddDistance >= Y_MAX_TRAVEL_DISTENCE)
		{
			yAddDistance = Y_MAX_TRAVEL_DISTENCE;
			_direction = "x+";
		}
	}

	if (_direction == "y-")
	{
		yAddDistance -= MOVE_STEP;
		if (yAddDistance <= Y_MIN_TRAVEL_DISTANCE)
		{
			yAddDistance = Y_MIN_TRAVEL_DISTANCE;
			_direction = "x-";
		}
	}

	if (_direction == "x+")
	{
		return Y_MAX_TRAVEL_DISTENCE;
	}

	if (_direction == "x-")
	{
		return Y_MIN_TRAVEL_DISTANCE;
	}

	return yAddDistance;;
}

double CyzBoatPositionCallback::getRotationAngle()
{
	if (_direction == "y+")
	{
		return osg::PI_2;
	}

	if (_direction == "y-")
	{
		return -osg::PI_2;
	}

	if (_direction == "x+")
	{
		return osg::PI;
	}

	if (_direction == "x-")
	{
		return 0.0;
	}
	return 0.0;
}


/*
void CyzBoatPositionCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	static double addMov = 1000;
	if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR) {
		osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);
		if (!mt || !_oceanScene.valid()) return;

		osg::NodePath nodePath = nv->getNodePath();
		double nodePathSize = nodePath.size();
		osg::Matrix mat = osg::computeLocalToWorld(nodePath);
		osg::Vec3d pos = mat.getTrans();

		osg::Vec3f normal;
		// Get the ocean surface height at the object's position, note
		// that this only considers one point on the object (the object's
		// geometric center) and not the whole object.
		float height = _oceanScene->getOceanSurfaceHeightAt(pos.x(), pos.y(), &normal);

		mat.makeTranslate(osg::Vec3f(pos.x(), pos.y() + 40000 + addMov, height + 20));
		addMov += 10;
		osg::Matrix rot;
		rot.makeIdentity();
		
rot.makeRotate(normal.x(), osg::Vec3f(1.0f, 0.0f, 0.0f),
	normal.y(), osg::Vec3f(0.0f, 1.0f, 0.0f),
	osg::PI_2, osg::Vec3f(0.0f, 0.0f, 1.0f));	//

osg::Matrix scale = osg::Matrix::scale(0.004, 0.004, 0.004);
mat = rot * mat*scale;
//mat.makeRotate(osg::PI_2, osg::Z_AXIS);
mt->setMatrix(mat);
	}

	traverse(node, nv);

}

*/