#include <opencv2/opencv.hpp>
#include "Header.h"
int port = 0;
int main(int argc, char* argv[]) {
	cv::VideoCapture* capdev;

	// open the video device
	capdev = new cv::VideoCapture(0);
	if (!capdev->isOpened()) {
		printf("Unable to open video device\n");
		return(-1);
	}

	// get some properties of the image
	cv::Size refS((int)capdev->get(cv::CAP_PROP_FRAME_WIDTH),
		(int)capdev->get(cv::CAP_PROP_FRAME_HEIGHT));
	printf("Expected size: %d %d\n", refS.width, refS.height);

	cv::namedWindow("Video", 1); // identifies a window
	cv::Mat frame, src, dst, dst1, temp1, grey_frame, blurred_frame;

	for (;;) {
		*capdev >> frame;
		char key = cv::waitKey(10);

		if (frame.empty()) {
			printf("Frame is empty\n");
			break;
		}
		if (key == 'n') {
			port = 1;
		}
		if (key == 'g') {
			port = 2;
		}
		if (key == 's') {
			cv::imwrite("saved.jpg", frame);
		}
		if (key == 'h') {
			port = 4;
		}
		if (key == 'b') {
			port = 5;
		}
		if (key == 'x') {
			port = 6;
		}
		if (key == 'y') {
			port = 7;
		}
		if (key == 'm') {
			port = 8;
		}
		if (key == 'l') {
			port = 9;
		}
		if (key == 'c') {
			port = 10;
		}
		if (port == 1) {
			imshow("Video", frame);
		}

		else if (port == 2) {
			cvtColor(frame, dst, cv::COLOR_RGBA2GRAY, 0);
			imshow("Video", dst);
		}
		else  if (port == 4) {
			greyscale(frame, dst);
			imshow("Video", dst);
		}
		else if (port == 5) {
			blur5x5(frame, dst);
			imshow("Video", dst);
		}
		else if (port == 6) {
			sobelX3x3(frame, temp1);
			convertScaleAbs(temp1, dst);
			imshow("Video", dst);
		}
		else if (port == 7) {
			sobelY3x3(frame, temp1);
			convertScaleAbs(temp1, dst);
			imshow("Video", dst);
		}
		else if (port == 8) {
			sobelX3x3(frame, dst1);
			sobelY3x3(frame, temp1);
			magnitude(dst1, temp1, dst);

			imshow("Video", dst);
		}
		else if (port == 9) {
			blur5x5(frame, src);
			blurQuantize(src, dst, 15);
			imshow("Video", dst);
		}
		else if (port == 10) {
			cartoon(frame, dst, 15, 15);
			imshow("Video", dst);
		}
		else
			imshow("Video", frame);

		if (key == 'q') {
			exit(0);
		}

	}
	delete capdev;
	return(0);
}