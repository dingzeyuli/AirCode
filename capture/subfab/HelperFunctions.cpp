// Copyright (C) 2016 Dingzeyu Li <lidingzeyu@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.


#include "HelperFunctions.h"

Timer::Timer()
{
  c_start = std::clock();
}

void Timer::end()
{
  c_end = std::clock();
}

double Timer::end_and_print()
{
  end();
  
  std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
  << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms \n";
  return 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
//  << "Wall clock time passed: "
//  << std::chrono::duration<double, std::milli>(t_end-t_start).count()
//  << " ms\n";
}


void get_xyz_from_index(int index, int& x, int& y, int& z, int x_size, int y_size, int z_size)
{
  z = floor (index / (x_size * y_size));
  int xy = index % (x_size * y_size);
  y = floor (xy / x_size);
  x = xy % x_size;
  std::cout << index << " " << x << " " << y  <<  " " << z << " " << xy;
}



// std::vector<int> sort_indexes(const std::vector<int> &v) {
//   
//   // initialize original index locations
//   std::vector<int> idx(v.size());
//   for (int i = 0; i != idx.size(); ++i) idx[i] = i;
//   
//   // sort indexes based on comparing values in v
//   std::sort(idx.begin(), idx.end(), [&v](int i1, int i2) {return v[i1] < v[i2];});
//   
//   return idx;
// }

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

/*
std::string listFiles( const char* path )
{
  DIR* dirFile = opendir( path );
  if ( dirFile )
  {
    struct dirent* hFile;
    errno = 0;
    while (( hFile = readdir( dirFile )) != NULL )
    {
      if ( !strcmp( hFile->d_name, "."  )) continue;
      if ( !strcmp( hFile->d_name, ".." )) continue;
      
      // in linux hidden files all start with '.'
      //if ( gIgnoreHidden && ( hFile->d_name[0] == '.' )) continue;
      
      // dirFile.name is the name of the file. Do whatever string comparison
      // you want here. Something like:
      if ( strstr( hFile->d_name, "impedance.txt" ))
      {
        // assume only one impedance file is in the directory
        closedir( dirFile );
        return hFile->d_name;
        //        printf( "found an .txt file: %s", hFile->d_name );
      }
    }
  }
}
*/

void wavelength_to_rgb_uint8(float wavelength, int* r, int* g, int* b)
{
  float fr, fg, fb;
  wavelength_to_rgb_float(wavelength, &fr, &fg, &fb);
  *r = round(fr * 255);
  *g = round(fg * 255);
  *b = round(fb * 255);
}

void wavelength_to_rgb_float(float wavelength, float* r, float* g, float* b)
{
  float Blue;
  float Green;
  float Red;
  float Factor;
  if(wavelength >= 350 && wavelength <= 439){
   Red  = -(wavelength - 440.0f) / (440.0f - 350.0f);
   Green = 0.0;
   Blue = 1.0;
  }else if(wavelength >= 440 && wavelength <= 489){
   Red  = 0.0;
   Green = (wavelength - 440.0f) / (490.0f - 440.0f);
   Blue = 1.0;
  }else if(wavelength >= 490 && wavelength <= 509){
   Red = 0.0;
   Green = 1.0;
   Blue = -(wavelength - 510.0f) / (510.0f - 490.0f);
  }else if(wavelength >= 510 && wavelength <= 579){
   Red = (wavelength - 510.0f) / (580.0f - 510.0f);
   Green = 1.0;
   Blue = 0.0;
  }else if(wavelength >= 580 && wavelength <= 644){
   Red = 1.0;
   Green = -(wavelength - 645.0f) / (645.0f - 580.0f);
   Blue = 0.0;
  }else if(wavelength >= 645 && wavelength <= 780){
   Red = 1.0;
   Green = 0.0;
   Blue = 0.0;
  }else{
   Red = 0.0;
   Green = 0.0;
   Blue = 0.0;
  }
  if(wavelength >= 350 && wavelength <= 419){
   Factor = 0.3 + 0.7*(wavelength - 350.0f) / (420.0f - 350.0f);
  }else if(wavelength >= 420 && wavelength <= 700){
     Factor = 1.0;
  }else if(wavelength >= 701 && wavelength <= 780){
   Factor = 0.3 + 0.7*(780.0f - wavelength) / (780.0f - 700.0f);
  }else{
   Factor = 0.0;
 }
  *r = factorAdjust(Red, Factor);
  *g = factorAdjust(Green, Factor);
  *b = factorAdjust(Blue, Factor);
}

float factorAdjust(float Color, float Factor){
  if(Color == 0.0){
    return 0;
   }else{
    float gamma = 1.0;
    float intensityMax = 1.0;
    return intensityMax * pow(Color * Factor, gamma);
   }
}


