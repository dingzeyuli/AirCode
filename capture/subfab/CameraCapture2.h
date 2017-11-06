// Copyright (C) 2016 Dingzeyu Li <lidingzeyu@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef CAMERA_CAPTURE_2_H
#define CAMERA_CAPTURE_2_H

#include <FlyCapture2.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class CameraCapture2
{
public:
	CameraCapture2();

	void PrintError();
	inline void PrintCameraInfo(FlyCapture2::CameraInfo*);
	int ConnectCamera();
	cv::Mat CaptureOneFrame();
	cv::Mat CaptureAverageImage(int total_number);
	int StopAndDisconnectCamera();

	bool PollForTriggerReady();
	bool FireSoftwareTrigger();
	bool CheckSoftwareTriggerPresence();

	FlyCapture2::Error SetCamProperties();
	inline void SetOneCamProperty(FlyCapture2::PropertyType, float, bool abs_control = true);
	inline void SetOneCamProperty(FlyCapture2::PropertyType, float, float, bool);
	cv::Mat CameraCapture2::GenerateChessBoard(int blockSize, int num_cols, int num_rows);

	FlyCapture2::Camera cam;

	int width, height;
	float shutter_time;
	int r, g, b;
private:
	FlyCapture2::BusManager busMgr;
	FlyCapture2::Error err;
	FlyCapture2::TriggerMode triggerMode;
	FlyCapture2::Property prop;

};

#endif