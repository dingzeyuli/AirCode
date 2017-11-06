// Copyright (C) 2016 Dingzeyu Li <lidingzeyu@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include "CameraCapture2.h"
#include "RawProcessing.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ctime>
#include <Windows.h>

using namespace std;

#define MONOCHROME 1

bool draw2 = false;

int CalcMinMain2()
{
	CameraCapture2 camcap;

	// PARAMETERS:
	int projector_width = 800, projector_height = 600;

    // set the camera shutter time, in milliseconds
	camcap.shutter_time = 200;
	
	// checkerboard pattern and sweeping interval
	double chessboard_frequency = 4;
	int displacement_step = 1;

	// number of averging 
	double total_images = 16;
	
	// discussed in our paper, we use red light to extract global component
	camcap.r = 255; camcap.b = 0; camcap.g = 0;

	int trial_num = 229;

	char filename[100];
	char result_folder[100] = "results/";

	std::vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);

	std::vector<cv::Mat> graycode_imgs, captured_graycode_imgs;
	cv::Mat decoded_cols, decoded_rows, mask;
	int n_cols, n_rows, col_shift, row_shift, sl_thresh = 65535 / 2;
	bool sl_scan_cols = true, sl_scan_rows = true;

	cv::waitKey(1000);

	cv::Mat img, display_image, white_image, cap_img;

	int total_displacement = (chessboard_frequency * 2);

	cout << 250.0 * 64 / chessboard_frequency << endl;
	img = camcap.GenerateChessBoard(chessboard_frequency, 4000 / chessboard_frequency, 4000 / chessboard_frequency);

	white_image = cv::Mat(projector_height, projector_width, CV_8UC3);
	white_image.setTo(cv::Vec3b(0, 0, 0));
	cv::Rect roi = cv::Rect(0, 0, projector_width, projector_height);
	img(roi).copyTo(white_image(roi));

	cv::namedWindow("Projector", 0);
	cv::moveWindow("Projector", 2000, 40);
	cv::setWindowProperty("Projector", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	cv::imshow("Projector", white_image);
	HWND win_handle = FindWindow(0, L"Projector");
	if (!win_handle)
	{
		printf("Failed FindWindow\n");
	}
	SetClassLongPtr(win_handle, GCLP_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(0, 0, 0)));

	cv::waitKey(1000);

	cv::namedWindow("indirect", 0);
	cv::moveWindow("indirect", 500, 10);
	cv::resizeWindow("indirect", 400, 300);
	cv::namedWindow("direct", 0);
	cv::moveWindow("direct", 1000, 10);
	cv::resizeWindow("direct", 400, 300);

	cv::waitKey(10);

	camcap.ConnectCamera();
	FlyCapture2::Image rawImage;

	RawProcessing rp(camcap.width, camcap.height);

	unsigned int rowBytes;

	cv::waitKey(400);
	cap_img = camcap.CaptureOneFrame();


	char command[100];
	int r, g, b;

	sprintf(command, "mkdir .\\results\\trial%i", (int)trial_num);
	system(command);
	srand(time(0));

	rowBytes = (double)rawImage.GetDataSize() / (double)rawImage.GetRows();
	cv::Mat image = cv::Mat(rawImage.GetRows(), rawImage.GetCols(), CV_16UC1, rawImage.GetData(), rowBytes);

	// projecting black image
	cout << "Capturing black levels" << endl;
	double total_black_images = 10;
	r = 0; g = 0; b = 0;
	white_image.setTo(cv::Vec3b(b, g, r));
	cv::imshow("Projector", white_image);

	cv::waitKey(2000);
	cv::Mat black_avg = cv::Mat(image.clone());
	black_avg.setTo(cv::Scalar::all(0));
	black_avg = camcap.CaptureAverageImage(total_images);
	sprintf(filename, "results/trial%i/black_level.png", (int)trial_num);
	cv::imwrite(filename, black_avg, compression_params);
	cv::waitKey(1000);

	cout << "Capturing vignetting basis" << endl;
	r = 255; g = 0; b = 0;
	white_image(roi).setTo(cv::Vec3b(b, g, r));
	cv::imshow("Projector", white_image);

	cv::waitKey(1000);
	cv::Mat vignet_avg = cv::Mat(image.clone());
	vignet_avg.setTo(cv::Scalar::all(0));
	vignet_avg = camcap.CaptureAverageImage(total_images);
	sprintf(filename, "results/trial%i/vignet_level.png", (int)trial_num);
	cv::imwrite(filename, vignet_avg, compression_params);

	cv::Mat rgb_image;

	cv::Mat Ld, Lg;
	cv::Mat im_max, im_min;

	camcap.r = r;
	camcap.g = g;
	camcap.b = b;
	img = camcap.GenerateChessBoard(chessboard_frequency, 2 * round(2000 / chessboard_frequency) + 1, 2 * round(2000 / chessboard_frequency) + 1);
	
	int base_x = 1650;
	int base_y = 0;

	// shuffle the sweeping order of checkerboard pattern
	std::vector<std::pair<int, int> > displacements;
	int pair_index = 0;
	for (int x_displacement = 0; x_displacement < total_displacement; x_displacement += displacement_step)
		for (int y_displacement = 0; y_displacement < total_displacement; y_displacement += displacement_step)
		{
			displacements.push_back(std::pair<int, int>(x_displacement, y_displacement));
		}
	std::random_shuffle(displacements.begin(), displacements.end());

	cv::waitKey(3000);

	black_avg.convertTo(black_avg, CV_32FC1);
	vignet_avg.convertTo(vignet_avg, CV_32FC1);

	cv::Mat temp;
	int key;
	display_image = cv::Mat(projector_height, projector_width, CV_8UC3);
	display_image.setTo(cv::Scalar::all(0));
	for (pair_index = 0; pair_index < displacements.size(); ++pair_index)
	{
		
		int x_displacement = displacements[pair_index].first;
		int y_displacement = displacements[pair_index].second;
		//moveWindow("Projector", base_x + x_displacement, base_y + y_displacement);
		cout << pair_index << "/" << displacements.size() << " " << x_displacement << " " << y_displacement << endl;
		temp = img(cv::Rect(y_displacement, x_displacement, projector_width, projector_height));
		temp(roi).copyTo(display_image(roi));
		cv::imshow("Projector", display_image);
		key = cv::waitKey(1000);
		if (key)
			key = 0;
		cv::Mat avg;

		avg = camcap.CaptureAverageImage(total_images);
		avg.convertTo(avg, CV_32FC1);
		avg = (avg - black_avg);// *32768 / vignet_avg;
		avg.convertTo(avg, CV_16UC1);

		rp.CompareMinMax(avg);
		if (MONOCHROME)
		{
			rgb_image = avg.clone();
		}
		else
		{
			rp.BayerDemosaic(avg, rgb_image);
		}

		sprintf(filename, "results/trial%i/frames/%i-%i.png", (int)trial_num, x_displacement, y_displacement);
		cv::imwrite(filename, rgb_image, compression_params);
		if (MONOCHROME)
			im_min = rp.min_image->clone();
		else
		{
			rp.BayerDemosaic(*(rp.min_image), rgb_image);
			im_min = rgb_image.clone();
		}

		if (MONOCHROME)
			im_max = rp.max_image->clone();
		else
		{
			rp.BayerDemosaic(*(rp.max_image), rgb_image);
			im_max = rgb_image.clone();
		}

		float b = 0.01;
		Ld = (im_max - im_min) / (1 - b);
		cv::imshow("direct", im_max);
		Lg = (im_max - Ld) * 2 / (1 + b);
		cv::imshow("indirect", im_min);
	}

	sprintf(filename, "results/trial%i/max_image.png", (int)trial_num);
	cv::imwrite(filename, im_max, compression_params);
	sprintf(filename, "results/trial%i/min_image.png", (int)trial_num);
	cv::imwrite(filename, im_min, compression_params);
	sprintf(filename, "results/trial%i/Lg.png", (int)trial_num);
	cv::imwrite(filename, Lg, compression_params);
	sprintf(filename, "results/trial%i/Ld.png", (int)trial_num);
	cv::imwrite(filename, Ld, compression_params);


	cv::Point2i cam_point, proj_point;

	if (MONOCHROME)
		im_min = im_min;
	else
		cv::cvtColor(im_min, im_min, CV_BGR2GRAY);



	delete rp.min_image;
	delete rp.max_image;
	delete rp.Lg;
	delete rp.Ld;
	rp.max_image = NULL;
	return 0;

}