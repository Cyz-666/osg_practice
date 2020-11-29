#pragma once
#ifndef CYZ_EARTH_MANIPULATOR_H_
#define CYZ_EARTH_MANIPULATOR_H_

#include <osgEarth/EarthManipulator>
#include "osgEarth/Viewpoint"

class CyzEarthManipulator : public osgEarth::EarthManipulator
{
public:
	CyzEarthManipulator();
	~CyzEarthManipulator();

	// handle events, return true if handled, false otherwise.
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

private:
	void flyToBuston();
	void flyToChina();
private:
	const osgEarth::Viewpoint _bustonViewpoint = osgEarth::Viewpoint("buston", -71.0763, 42.34425, 0, 24.261, -21.6, 3450.0);
	const osgEarth::Viewpoint _ChinaViewpoint = osgEarth::Viewpoint("China", 104.21, 28.42, 0, 0, -90, 10000000);
};

#endif //CYZ_EARTH_MANIPULATOR_H_