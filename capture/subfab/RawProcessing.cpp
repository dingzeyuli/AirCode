// Copyright (C) 2016 Dingzeyu Li <lidingzeyu@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include "RawProcessing.h"

using namespace cv;

RawProcessing::RawProcessing(int width_, int height_)
{
  b = 0.1;
  width = width_;
  height = height_;

  max_image = NULL;
  min_image = NULL;
  Ld = NULL;
  Lg = NULL;
}

RawProcessing::~RawProcessing()
{
  if (max_image != NULL)
  {
    delete max_image;
    delete min_image;
    delete Ld;
    delete Lg;
  }
}

void RawProcessing::CompareMinMax(const Mat& current_capture)
{
  if (max_image == NULL)
  {
    max_image = new Mat(current_capture.clone());
    min_image = new Mat(current_capture.clone());
    Lg = new Mat(current_capture.clone());
    Ld = new Mat(current_capture.clone());
    *max_image = (int) 0;
    *min_image = (int) 65535;
    //*Lg = (ushort) 0;
    //*Ld = (ushort) 0;
  }

  for(int i = 0; i < current_capture.rows; i++)
  {
      const ushort* current = current_capture.ptr<ushort>(i);
      ushort* max_row = max_image->ptr<ushort>(i);
      ushort* min_row = min_image->ptr<ushort>(i);
      for(int j = 0; j < current_capture.cols; j++)
      {
        if (max_row[j] < current[j])
          max_row[j] = current[j];
        if (min_row[j] > current[j])
          min_row[j] = current[j];
      }
      //sum += std::max(Mi[j], 0.);
  }
  //Dcout << max << " " << min;
}

void RawProcessing::ComputeLocalGlobal()
{
  for(int i = 0; i < max_image->rows; i++)
  {
      ushort* max_row = max_image->ptr<ushort>(i);
      ushort* min_row = min_image->ptr<ushort>(i);
      ushort* Ld_row = Ld->ptr<ushort>(i);
      ushort* Lg_row = Lg->ptr<ushort>(i);
      for(int j = 0; j < max_image->cols; j++)
      {
        b=0;
        //if (max_row[j] < min_row[j])
        //  cout << i << " " << j << " " << max_row[j] << " " << min_row[j] ;
        //assert(max_row[j] >= min_row[j]);
        //int max_diff = max_row[j] - min_row[j];
        //if (max_diff < 0)
        //  max_diff = 0;
        //else if (max_diff > 65535)
        //  max_diff = 65535;
        Ld_row[j] = (ushort) max_row[j] - min_row[j];
        Lg_row[j] = (ushort) ((max_row[j] - Ld_row[j]) * 2 );
      }
  }
}


void RawProcessing::BayerDemosaic(Mat& bayer_image, Mat& rgb_image)
{
  FlyCapture2::Image validate_image(
      bayer_image.rows,
      bayer_image.cols,
      2* bayer_image.cols,
      bayer_image.data,
      bayer_image.rows * bayer_image.cols * 2, // two bytes
      FlyCapture2::PIXEL_FORMAT_RAW16,
      FlyCapture2::BGGR);

  FlyCapture2::Error err = validate_image.Convert(FlyCapture2::PIXEL_FORMAT_BGR16, &rgbImage);
	if (err != FlyCapture2::PGRERROR_OK)
	{
	  err.PrintErrorTrace();
	}
	int rowBytes = (double)rgbImage.GetDataSize() / (double)rgbImage.GetRows();

  rgb_image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_16UC3, rgbImage.GetData(), rowBytes);

}
