/*
	Vinesh Krishna Anne 

	Functions to read and write CSV files with a specific format
  */
#ifndef CVS_UTIL_H
#define CVS_UTIL_H
#include<vector>
#include<opencv2/opencv.hpp>
 /*
	Each line of the csv file is a filename in the first column, followed by numeric data for the remaining columns,
	each line of the csv file has to have the same number of columns.
	Given a filename, and image filename and features, by default the function will append a line of data to the CSV format file.  
	If reset_file is true, then it will open the file in 'write'
	mode and clear the existing contents.

	The image filename is written to the first position in the row of
	data. The values in image_data are all written to the file as
	floats.

	The function returns a non-zero value in case of an error.
   */
int appendcsv(char* filename, char* image_filename, std::vector<float>& image_data, int reset_file = 0);
int readcsv(char* filename, std::vector<char*>& filenames, std::vector<std::vector<float>>& data, int echo_file = 0);
/*
  Given a file with the format of a string as the first column and
  floating point numbers as the remaining columns, this function
  returns the filenames as a std::vector of character arrays, and the
  remaining data as a 2D std::vector<float>.

  filenames will contain all of the image file names.
  data will contain the features calculated from each image.

  If echo_file is true, it prints out the contents of the file as read
  into memory.

  The function returns a non-zero value if something goes wrong.
 */
std::vector<float> task1(char* filepath);
std::vector<float> task2(char* filespath);
std::vector<float> task3(char* filepath);
std::vector<float> taska4(char* filepath);
std::vector<float> taskb4(char* filepath);
#endif
#pragma once