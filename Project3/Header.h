// Vinesh Krishna Anne

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Defined functions from functions file, to use in main file

Mat threshold(Mat& image);

Mat cleanup(Mat& image);

Mat getRegions(Mat& image, Mat& labeledRegions, Mat& stats, Mat& centroids, vector<int>& topNLabels);

RotatedRect getBoundingBox(Mat& region, double x, double y, double alpha);

void drawLine(Mat& image, double x, double y, double alpha, Scalar color);

void drawBoundingBox(Mat& image, RotatedRect boundingBox, Scalar color);

void calcHuMoments(Moments mo, vector<double>& huMoments);

double euclideanDistance(vector<double> features1, vector<double> features2);

string classifier(vector<vector<double>> featureVectors, vector<string> classNames, vector<double> currentFeature);

string classifierKNN(vector<vector<double>> featureVectors, vector<string> classNames, vector<double> currentFeature, int K);

string getClassName(char c);

void writeToCSV(string filename, vector<string> classNamesDB, vector<vector<double>> featuresDB);

void loadFromCSV(string filename, vector<string>& classNamesDB, vector<vector<double>>& featuresDB);