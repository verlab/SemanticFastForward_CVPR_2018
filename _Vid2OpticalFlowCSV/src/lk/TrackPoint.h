#pragma once

class TrackPoint
{
public:
	cv::Point2d _curLocation;
	cv::Point2d _newLocation;
	double _tx, _ty;
	bool _trackingValid;
};

