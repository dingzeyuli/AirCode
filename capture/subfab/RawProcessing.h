// Copyright (C) 2016 Dingzeyu Li <lidingzeyu@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#ifndef RAW_PROCESSING_H 
#define RAW_PROCESSING_H 

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <FlyCapture2.h>
//#include <glog/logging.h>

class RawProcessing
{
  public:
    RawProcessing(int width_, int height_);
    ~RawProcessing();

    void CompareMinMax(const cv::Mat& current_capture);
    void ComputeLocalGlobal();
    void FlyCaptureToOpenCV();
    void BayerDemosaic(cv::Mat&, cv::Mat&);

    int width, height;

    float b;
    cv::Mat* max_image;
    cv::Mat* min_image;
    cv::Mat* Ld;
    cv::Mat* Lg;
    FlyCapture2::Image rgbImage;
};

#endif
