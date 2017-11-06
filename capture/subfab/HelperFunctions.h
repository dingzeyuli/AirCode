// Copyright (C) 2016 Dingzeyu Li <lidingzeyu@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#ifndef HelperFunctions_hpp
#define HelperFunctions_hpp


#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>    // std::sort

//#include <dirent.h>
//#include <glog/logging.h>
//#include <gflags/gflags.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <complex>


#define MONOCHROME  true


// http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}


// http://stackoverflow.com/questions/16605967/set-precision-of-stdto-string-when-converting-floating-point-values
template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
  std::ostringstream out;
  out << std::setprecision(n) << a_value;
  return out.str();
}



// http://stackoverflow.com/questions/236129/split-a-string-in-c

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);


// http://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder

std::string listFiles( const char* path );


// https://github.com/JamesLaverack/photon-tracer/blob/master/src/WavelengthToRGB.cpp
void wavelength_to_rgb_float(float wavelength, float* r, float* g, float *b);
void wavelength_to_rgb_uint8(float wavelength, int* r, int* g, int *b);
float factorAdjust(float Color, float Factor);


// http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes

//std::vector<int> sort_indexes(const std::vector<int> &v);


// http://stackoverflow.com/questions/25389480/how-to-write-read-an-eigen-matrix-from-binary-file
//template<class Matrix>
//void write_eigen_binary(const Matrix& matrix, const char* filename);

class Timer
{
public:
  Timer();
  void end();
  double end_and_print();
  
private:
  std::clock_t c_start;
  
  std::clock_t c_end;

};

#endif /* HelperFunctions_hpp */


