#include <opencv2/opencv.hpp>
#include "Header.h"
int greyscale(cv::Mat& src, cv::Mat& dst) {
	dst = cv::Mat(src.rows, src.cols, CV_8UC1);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			cv::Vec3b scale = src.at<cv::Vec3b>(i, j);
			int gray = (scale[0] + scale[1] + scale[2]) / 3;
			dst.at<uchar>(i, j) = gray;
		}
	}
	return 0;
}

int blur5x5(cv::Mat& src, cv::Mat& dst) {
	cv::Mat smudge;
	src.copyTo(smudge);
	cv::Vec3b value1;
	cv::Vec3i value2 = { 0,0,0 };
	cv::Vec3i value3 = { 0,0,0 };
	for (int i = 2; i < src.rows - 2; i++) {
		for (int j = 2; j < src.cols - 2; j++) {
			for (int c = 0; c < 3; c++) {
				value2[c] = src.at<cv::Vec3b>(i - 2, j - 2)[c] + src.at<cv::Vec3b>(i - 2, j - 1)[c] * 2 +
					src.at<cv::Vec3b>(i - 2, j)[c] * 4 + src.at<cv::Vec3b>(i - 2, j + 1)[c] * 2 + src.at<cv::Vec3b>(i - 2, j + 2)[c];
				value2[c] /= 10;
				value1[c] = (unsigned char)value2[c];
				smudge.at<cv::Vec3b>(i, j)[c] = value1[c];
			}

		}
	}
	smudge.copyTo(dst);

	for (int j = 2; j < smudge.cols - 2; j++) {
		for (int i = 2; i < smudge.rows - 2; i++) {
			for (int c = 0; c < 3; c++) {
				value3[c] = smudge.at<cv::Vec3b>(i - 2, j - 2)[c] + smudge.at<cv::Vec3b>(i - 1, j - 2)[c] * 2 + smudge.at<cv::Vec3b>(i, j - 2)[c] * 4 +
					smudge.at<cv::Vec3b>(i + 1, j - 2)[c] * 2 + smudge.at<cv::Vec3b>(i + 2, j - 2)[c];
				value3[c] /= 10;
				dst.at<cv::Vec3b>(i, j)[c] = (unsigned char)value3[c];
			}
		}
	}
	return 0;
}

int sobelX3x3(cv::Mat& src, cv::Mat& dst)
{
    dst.create(src.size(), CV_16SC3);
    dst.setTo(cv::Scalar::all(0));
    int sobelx[3][3] = { {-1, 0, 1},
                         {-2, 0, 2},
                         {-1, 0, 1} };
    for (int i = 1; i < src.rows - 1; i++)
    {
        for (int j = 1; j < src.cols - 1; j++)
        {
            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    cv::Vec3b mx = src.at<cv::Vec3b>(i + k, j + l);
                    int sobelv = sobelx[k + 1][l + 1];
                    dst.at<cv::Vec3s>(i, j) += mx * sobelv;
                }
            }
        }
    }
    cv::convertScaleAbs(dst, dst);

    return 0;
}

int sobelY3x3(cv::Mat& src, cv::Mat& dst)
{
    dst.create(src.size(), CV_16SC3);
    dst.setTo(cv::Scalar::all(0));
    int sobely[3][3] = { {-1, -2, -1},
                         {0, 0, 0},
                         {1, 2, 1} };
    for (int i = 1; i < src.rows - 1; i++)
    {
        for (int j = 1; j < src.cols - 1; j++)
        {
            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    cv::Vec3b mx = src.at<cv::Vec3b>(i + k, j + l);
                    int sobelv = sobely[k + 1][l + 1];
                    dst.at<cv::Vec3s>(i, j) += mx * sobelv;
                }
            }
        }
    }
    cv::convertScaleAbs(dst, dst);

    return 0;
}

int magnitude(cv::Mat& sx, cv::Mat& sy, cv::Mat& dst)
{
    for (int i = 0; i < sx.rows; i++)
    {
        for (int j = 0; j < sx.cols; j++)
        {
            cv::Vec3s sxPixel = sx.at<cv::Vec3s>(i, j);
            cv::Vec3s syPixel = sy.at<cv::Vec3s>(i, j);
            cv::Vec3b& dstPixel = dst.at<cv::Vec3b>(i, j);
            for (int k = 0; k < 3; k++)
            {
                int magnitude = cvRound(sqrt(sxPixel[k] * sxPixel[k] + syPixel[k] * syPixel[k]));
                dstPixel[k] = cv::saturate_cast<uchar>(magnitude);
            }
        }
    }

    return 0;
}

int blurQuantize(cv::Mat & src, cv::Mat & dst, int levels)
{
    cv::GaussianBlur(src, dst, cv::Size(5, 5), 0);

    int bucket_size = 255 / levels;

    for (int i = 0; i < dst.rows; i++)
    {
        for (int j = 0; j < dst.cols; j++)
        {
            cv::Vec3b& pixel = dst.at<cv::Vec3b>(i, j);

            for (int k = 0; k < 3; k++)
            {
                int xt = pixel[k] / bucket_size;
                pixel[k] = xt * bucket_size;
            }
        }
    }

    return 0;
}

int cartoon(cv::Mat& src, cv::Mat& dst, int levels, int magThreshold)
{
    cv::Mat sx, sy, magnitudeImage;
    sobelX3x3(src, sx);
    sobelY3x3(src, sy);
    magnitude(sx, sy, magnitudeImage);

    cv::Mat blurredQuantizedImage;
    blurQuantize(src, blurredQuantizedImage, levels);

    dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC3);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            if (magnitudeImage.at<uchar>(i, j) > magThreshold) {
                dst.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            }
            else {
                dst.at<cv::Vec3b>(i, j) = blurredQuantizedImage.at<cv::Vec3b>(i, j);
            }
        }
    }
    return 0;
}
