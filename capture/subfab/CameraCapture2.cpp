// Copyright (C) 2016 Dingzeyu Li <lidingzeyu@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <Windows.h>

#include "CameraCapture2.h"
#include "RawProcessing.h"
#include "HelperFunctions.h"

//
// Software trigger the camera instead of using an external hardware trigger
//
#define SOFTWARE_TRIGGER_CAMERA

using namespace FlyCapture2;
using namespace std;

CameraCapture2::CameraCapture2()
{
  shutter_time = 10;
}

inline void CameraCapture2::PrintError()
{
  cout << "ERROR!!"; err.PrintErrorTrace();
}

void CameraCapture2::PrintCameraInfo(CameraInfo* pCamInfo)
{
	cout << endl;
	cout << "*** CAMERA INFORMATION ***" << endl;
	cout << "Serial number -" << pCamInfo->serialNumber << endl;
	cout << "Camera model - " << pCamInfo->modelName << endl;

	cout << "Camera vendor - " << pCamInfo->vendorName << endl;
	cout << "Sensor - " << pCamInfo->sensorInfo << endl;
	cout << "Resolution - " << pCamInfo->sensorResolution << endl;
	cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
	cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl << endl;
}

int CameraCapture2::StopAndDisconnectCamera()
{
	// Stop capturing images
	err = cam.StopCapture();
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}

	// Disconnect the camera
	err = cam.Disconnect();
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}
	return 0;
}

int CameraCapture2::ConnectCamera()
{
	unsigned int numCameras;
	err = busMgr.GetNumOfCameras(&numCameras);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}

	std::cout << "Number of cameras detected: " << numCameras << std::endl;

	PGRGuid guid;
	err = busMgr.GetCameraFromIndex(0, &guid);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}

	// Connect to a camera
	err = cam.Connect(&guid);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}

	// Power on the camera
	const unsigned int k_cameraPower = 0x610;
	const unsigned int k_powerVal = 0x80000000;
	err = cam.WriteRegister(k_cameraPower, k_powerVal);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}

	const unsigned int millisecondsToSleep = 100;
	unsigned int regVal = 0;
	unsigned int retries = 10;

	// Wait for camera to complete power-up
	do
	{
#if defined(WIN32) || defined(WIN64)
		Sleep(millisecondsToSleep);
#else
		usleep(millisecondsToSleep * 1000);
#endif
		err = cam.ReadRegister(k_cameraPower, &regVal);
		if (err == PGRERROR_TIMEOUT)
		{
			// ignore timeout errors, camera may not be responding to
			// register reads during power-up
		}
		else if (err != PGRERROR_OK)
		{
			cout << "ERROR!!"; err.PrintErrorTrace();
			return -1;
		}

		retries--;
	} while ((regVal & k_powerVal) == 0 && retries > 0);

	// Check for timeout errors after retrying
	if (err == PGRERROR_TIMEOUT)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}

	// Get the camera information
	CameraInfo camInfo;
	err = cam.GetCameraInfo(&camInfo);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}

	PrintCameraInfo(&camInfo);
	err = SetCamProperties();
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}

	// Start capturing images
	err = cam.StartCapture();
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return -1;
	}


#ifdef SOFTWARE_TRIGGER_CAMERA
	if (!CheckSoftwareTriggerPresence())
	{
		cout << "SOFT_ASYNC_TRIGGER not implemented on this camera!  Stopping application" << endl;
		return -1;
	}
#else	
	cout << "Trigger the camera by sending a trigger pulse to GPIO" << triggerMode.source << endl;
#endif

}

inline void CameraCapture2::SetOneCamProperty(PropertyType property_type, float abs_value, bool abs_control)
{
  //Define the property to adjust.
	prop.type = property_type;
	
  //Ensure the property is on.
	prop.onOff = true;

	//Ensure auto-adjust mode is off.
	prop.autoManualMode = false;

  //Ensure the property is set up to use absolute value control.
	prop.absControl = abs_control;

  //Set the absolute value of brightness to 0.5%.
	prop.absValue = abs_value;
	
  //Set the property.
	err = cam.SetProperty(&prop);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return;
	}
  return;
}

inline void CameraCapture2::SetOneCamProperty(PropertyType property_type, float value_a, float value_b, bool abs_control)
{
  //Define the property to adjust.
	prop.type = property_type;
	
  //Ensure the property is on.
	prop.onOff = true;

	//Ensure auto-adjust mode is off.
	prop.autoManualMode = false;

  //Ensure the property is set up to use absolute value control.
	prop.absControl = abs_control;

  //Set the absolute value of brightness to 0.5%.
	prop.valueA = value_a;
	prop.valueB = value_b;
	
  //Set the property.
	err = cam.SetProperty(&prop);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return;
	}
  return;
}

Error CameraCapture2::SetCamProperties()
{
	const Mode k_fmt7Mode = MODE_7;
	PixelFormat k_fmt7PixFmt;
	if (MONOCHROME)
		k_fmt7PixFmt = PIXEL_FORMAT_RAW16;
	else
		k_fmt7PixFmt = PIXEL_FORMAT_RAW16;


	// Query for available Format 7 modes
	Format7Info fmt7Info;
	bool supported;
	fmt7Info.mode = k_fmt7Mode;
	err = cam.GetFormat7Info(&fmt7Info, &supported);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return err;
	}

	if ((k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0)
	{
		// Pixel format not supported!
		std::cout << "Pixel format is not supported" << std::endl;
		return err;
	}

	Format7ImageSettings fmt7ImageSettings;
	fmt7ImageSettings.mode = k_fmt7Mode;
	fmt7ImageSettings.offsetX = 0;
	fmt7ImageSettings.offsetY = 0;
	fmt7ImageSettings.width = fmt7Info.maxWidth;
	fmt7ImageSettings.height = fmt7Info.maxHeight;
	fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;

	width = fmt7Info.maxWidth;
	height = fmt7Info.maxHeight;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	err = cam.ValidateFormat7Settings(
		&fmt7ImageSettings,
		&valid,
		&fmt7PacketInfo);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return err;
	}

	if (!valid)
	{
		// Settings are not valid
		cout << "Format7 settings are not valid" << endl;
		return err;
	}

	// Set the settings to the camera
	err = cam.SetFormat7Configuration(
		&fmt7ImageSettings,
		fmt7PacketInfo.recommendedBytesPerPacket);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return err;
	}


	// Get current trigger settings
	err = cam.GetTriggerMode(&triggerMode);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return err;
	}

	// Set camera to trigger mode 0
	triggerMode.onOff = true;
	triggerMode.mode = 0;
	triggerMode.parameter = 0;

#ifdef SOFTWARE_TRIGGER_CAMERA
	// A source of 7 means software trigger
	triggerMode.source = 7;
#else
	// Triggering the camera externally using source 0.
	triggerMode.source = 0;
#endif

	err = cam.SetTriggerMode(&triggerMode);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return err;
	}

	// Poll to ensure camera is ready
	bool retVal = PollForTriggerReady();
	if (!retVal)
	{
		cout << endl;
		cout << "Error polling for trigger ready!" << endl;
		return err;
	}

	// Get the camera configuration

	FC2Config config;
	err = cam.GetConfiguration(&config);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return err;
	}

	// Set the grab timeout to 5 seconds

	config.grabTimeout = 5000;

	// Set the camera configuration

	err = cam.SetConfiguration(&config);
	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return err;
	}


	SetOneCamProperty(BRIGHTNESS, 0.5);

	//banana 120
	SetOneCamProperty(SHUTTER, shutter_time);

	SetOneCamProperty(GAIN, 0.0);

	SetOneCamProperty(AUTO_EXPOSURE, -3.5);

	SetOneCamProperty(SHARPNESS, 1500, false);

	SetOneCamProperty(GAMMA, 1.0);

	SetOneCamProperty(FRAME_RATE, 5.0);

	printf("No error\n");

	if (!MONOCHROME)
		SetOneCamProperty(WHITE_BALANCE, 284, 783, false);

	return err;
}

bool CameraCapture2::PollForTriggerReady()
{
  Camera* pCam = &(this->cam);
	const unsigned int k_softwareTrigger = 0x62C;
	unsigned int regVal = 0;
	
  do
	{
		err = pCam->ReadRegister(k_softwareTrigger, &regVal);

		if (err != PGRERROR_OK)

		{
			cout << "ERROR!!"; err.PrintErrorTrace();
			return false;
		}
	} while ((regVal >> 31) != 0);

	return true;
}

bool CameraCapture2::FireSoftwareTrigger()
{
  Camera* pCam = &(this->cam);
	const unsigned int k_softwareTrigger = 0x62C;
	const unsigned int k_fireVal = 0x80000000;
	err = pCam->WriteRegister(k_softwareTrigger, k_fireVal);

	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return false;
	}
	return true;
}

bool CameraCapture2::CheckSoftwareTriggerPresence()
{
  Camera* pCam = &(this->cam);
	const unsigned int k_triggerInq = 0x530;
	unsigned int regVal = 0;
	err= pCam->ReadRegister(k_triggerInq, &regVal);

	if (err != PGRERROR_OK)
	{
		cout << "ERROR!!"; err.PrintErrorTrace();
		return false;
	}
	
  if ((regVal & 0x10000) != 0x10000)
	{
		return false;
	}

  return true;
}

cv::Mat CameraCapture2::CaptureOneFrame()
{
  bool captured_image = false;
  Image rawImage;
  cv::Mat image;
  while(captured_image == false)
  {
    #ifdef SOFTWARE_TRIGGER_CAMERA
		// Check that the trigger is ready
		PollForTriggerReady();

		// Fire software trigger
		bool retVal = FireSoftwareTrigger();
		if (!retVal)
		{
			cout << "Error firing software trigger";
			continue;
		}
    #endif
		
    // Retrieve an image
		cam.RetrieveBuffer(&rawImage);
		if (err != PGRERROR_OK)
		{
			cout << "ERROR!!"; err.PrintErrorTrace();
      continue;
		}

		
    if (MONOCHROME)
    {
      unsigned int rowBytes = (double)rawImage.GetDataSize() / (double)rawImage.GetRows();
		  image = cv::Mat(rawImage.GetRows(), rawImage.GetCols(), CV_16UC1, rawImage.GetData(), rowBytes);
    }
    else 
    {
      Image rgbImage;
      // Convert the raw image
      err = rawImage.Convert(PIXEL_FORMAT_BGR16, &rgbImage);
      if (err != PGRERROR_OK)
      {
			  cout << "ERROR!!"; err.PrintErrorTrace();
      	continue;
      }
      
      // convert to OpenCV Mat
      unsigned int rowBytes = (double)rgbImage.GetDataSize() / (double)rgbImage.GetRows();
      cv::Mat img = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_16UC3, rgbImage.GetData(), rowBytes);
      img.copyTo(image);
    }
    captured_image = true;
  }

  return  image;
}

cv::Mat CameraCapture2::CaptureAverageImage(int total_images)
{
  FlyCapture2::Image rawImage;
  cv::Mat avg = cv::Mat(height, width, CV_32FC1, cv::Scalar(0));

  for (int i = 0; i < total_images; i++)
  {
    cv::Mat img = CaptureOneFrame();
	img.convertTo(img, CV_32F);
	avg = avg + img / total_images;
    cv::waitKey(100);
  }
  avg.convertTo(avg, CV_16UC1);

  return avg;
} 

cv::Mat CameraCapture2::GenerateChessBoard(int blockSize, int num_cols, int num_rows)
{
	using namespace cv;
	Size imageSize = Size(blockSize * num_cols, blockSize * num_rows);
	Mat chessBoard(imageSize, CV_8UC3, Scalar::all(0));
	unsigned char color = 0;
	bool c = 1;
	bool begin_c = 1;

	for (int i = 0; i < imageSize.height; i = i + blockSize)
	{
		begin_c = 1 - begin_c;
		c = 1 - begin_c;
		color = ~color;
		for (int j = 0; j < imageSize.width; j = j + blockSize)
		{
			Mat ROI = chessBoard(Rect(j, i, blockSize, blockSize));
			if (c == 1)
				ROI.setTo(Scalar(b, g, r));
			else
				ROI.setTo(Scalar(0, 0, 0));

			color = ~color;
			c = 1 - c;
		}
	}
	return chessBoard;
}