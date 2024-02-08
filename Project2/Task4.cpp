/*  Vinesh Krishna Anne
	Task4 - File for Texture and Color Matching */

#include<iostream>
#include<vector>
#include<algorithm>
#include<opencv2/opencv.hpp>
#include "Header.h"

int main(int arc, char* argv[]) {
	//reading target image and extracting freature set
	cv::Mat targetimage = cv::imread(argv[1]);
	std::vector<float> fset = taska4(argv[1]);
	//reading the feature vectors file
	char filename[256];
	std::vector<char*> imagepath;
	std::vector<std::vector<float>> data;
	strcpy(filename, argv[2]);
	readcsv(filename, imagepath, data, 0);
	//computing the distance metric and storing in a vector
	//using maximum intersection as the distance metrics
	std::vector<std::vector<std::string>> matchedimages;
	for (int i = 0; i < data.size(); i++) {
		int temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0, temp5 = 0;
		for (int j = 0; j < 258; j++) {
			temp1 = temp1 + std::min(fset[j], data[i][j]);
		}
		for (int j = 259; j < 515; j++) {
			temp2 = temp2 + std::min(fset[j], data[i][j]);
		}
		for (int j = 516; j < 772; j++) {
			temp3 = temp3 + std::min(fset[j], data[i][j]);
		}
		for (int j = 773; j < 1029; j++) {
			temp4 = temp4 + std::min(fset[j], data[i][j]);
		}
		for (int j = 1030; j < data[i].size(); j++) {
			temp5 = temp5 + std::min(fset[j], data[i][j]);
		}
		int temp6 = std::max({ temp1, temp2, temp3, temp4, temp5 });
		float temp = (3 * temp1 + 7 * temp6) / 10.0;
		matchedimages.push_back({ std::to_string(temp), imagepath[i] });
	}
	//sorting the vector from most similar image to least similar image
	std::sort(matchedimages.begin(), matchedimages.end(),
		[](std::vector<std::string>& a, std::vector<std::string>& b)
		{
			return stoi(a[0]) > stoi(b[0]);
		});
	//printing the sorted image paths
	//for (int i = 0;i<1107;i++)
	//std::cout << matched_images[i][1]<<"\n";
	//reading the top 3 images
	cv::Mat image1 = cv::imread(matchedimages[1][1]);
	cv::Mat image2 = cv::imread(matchedimages[2][1]);
	cv::Mat image3 = cv::imread(matchedimages[3][1]);
	//displaying the top 3 images
	while (1) {
		cv::imshow("target_image", targetimage);
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