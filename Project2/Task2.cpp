 /* Vinesh Krishna Anne
	Task2 - File for Histogram Matching */
 
#include<iostream>
#include<vector>
#include<algorithm>
#include<opencv2/opencv.hpp>
#include "Header.h"

int main(int argc, char* argv[]) {
	//reading target image and extracting freature set
	cv::Mat targetimage = cv::imread(argv[1]);
	std::vector<float> fset = task2(argv[1]);
	//reading the feature vectors file
	char filename[256];
	std::vector<char*> imagepath;
	std::vector<std::vector<float>> data;
	strcpy(filename, argv[2]);
	readcsv(filename, imagepath, data, 0);
	//computing the distance metric and storing in a vector
	//using the maximum intersetion as the distance metrics 
	std::vector<std::vector<std::string>> matchedimages;
	for (int i = 0; i < data.size(); i++) {
		int temp = 0;
		for (int j = 0; j < data[i].size(); j++) {
			temp = temp + std::min(fset[j], data[i][j]);
		}
		matchedimages.push_back({ std::to_string(temp), imagepath[i] });
	}
	//sorting the vector from most similar image to least similar image
	std::sort(matchedimages.begin(), matchedimages.end(),
		[](std::vector<std::string>& a, std::vector<std::string>& b)
		{
			return stoi(a[0]) > stoi(b[0]);
		});
	//reading the top 3 images
	cv::Mat image1 = cv::imread(matchedimages[1][1]);
	cv::Mat image2 = cv::imread(matchedimages[2][1]);
	cv::Mat image3 = cv::imread(matchedimages[3][1]);
	//displaying the top 3 images
	while (1) {
		cv::imshow("targetimage", targetimage);
		cv::imshow("image1", image1);
		cv::imshow("image2", image2);
		cv::imshow("image3", image3);
		if (cv::waitKey() == 'q') {
			cv::destroyAllWindows();
			break;
		}
	}
	return 0;
}