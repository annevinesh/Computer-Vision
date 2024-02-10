// Vinesh Krishna Anne


// Defining functions
#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <map>
#include <float.h>
#include <math.h>
#include "Header.h"

using namespace cv;
using namespace std;


// Threshold function
Mat threshold(Mat& image) {
    Mat processedImage, grayscale;
    processedImage = Mat(image.size(), CV_8UC1);
    cvtColor(image, grayscale, COLOR_BGR2GRAY);
    GaussianBlur(grayscale, grayscale, cv::Size(3, 3), 0, 0);
    for (int i = 0; i < grayscale.rows; i++) {
        for (int j = 0; j < grayscale.cols; j++) {
            if (grayscale.at<uchar>(i, j) <= 125) {
                processedImage.at<uchar>(i, j) = 255;
            }
            else {
                processedImage.at<uchar>(i, j) = 0;
            }
        }
    }
    return processedImage;
}


// Cleanup function
Mat cleanup(Mat& image) {
    Mat processedImage;
    const Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(25, 25));
    morphologyEx(image, processedImage, MORPH_CLOSE, kernel);
    return processedImage;
}


// Segmenting the processed image
Mat getRegions(Mat& image, Mat& labeledRegions, Mat& stats, Mat& centroids, vector<int>& topNLabels) {
    Mat processedImage;
    int nLabels = connectedComponentsWithStats(image, labeledRegions, stats, centroids);
    Mat areas = Mat::zeros(1, nLabels - 1, CV_32S);
    Mat sortedIdx;
    for (int i = 1; i < nLabels; i++) {
        int area = stats.at<int>(i, CC_STAT_AREA);
        areas.at<int>(i - 1) = area;
    }
    if (areas.cols > 0) {
        sortIdx(areas, sortedIdx, SORT_EVERY_ROW + SORT_DESCENDING);
    }
    vector<Vec3b> colors(nLabels, Vec3b(0, 0, 0));
    int N = 3; 
    N = (N < sortedIdx.cols) ? N : sortedIdx.cols;
    int THRESHOLD = 5000; 
    for (int i = 0; i < N; i++) {
        int label = sortedIdx.at<int>(i) + 1;
        if (stats.at<int>(label, CC_STAT_AREA) > THRESHOLD) {
            colors[label] = Vec3b(rand() % 256, rand() % 256, rand() % 256);
            topNLabels.push_back(label);
        }
    }
    processedImage = Mat::zeros(labeledRegions.size(), CV_8UC3);
    for (int i = 0; i < processedImage.rows; i++) {
        for (int j = 0; j < processedImage.cols; j++) {
            int label = labeledRegions.at<int>(i, j);
            processedImage.at<Vec3b>(i, j) = colors[label];
        }
    }
    return processedImage;
}


// Computing region
RotatedRect getBoundingBox(Mat& region, double x, double y, double alpha) {
    int maxX = INT_MIN, minX = INT_MAX, maxY = INT_MIN, minY = INT_MAX;
    for (int i = 0; i < region.rows; i++) {
        for (int j = 0; j < region.cols; j++) {
            if (region.at<uchar>(i, j) == 255) {
                int projectedX = (i - x) * cos(alpha) + (j - y) * sin(alpha);
                int projectedY = -(i - x) * sin(alpha) + (j - y) * cos(alpha);
                maxX = max(maxX, projectedX);
                minX = min(minX, projectedX);
                maxY = max(maxY, projectedY);
                minY = min(minY, projectedY);
            }
        }
    }
    int lengthX = maxX - minX;
    int lengthY = maxY - minY;

    Point centroid = Point(x, y);
    Size size = Size(lengthX, lengthY);

    return RotatedRect(centroid, size, alpha * 180.0 / CV_PI);
}



void drawLine(Mat& image, double x, double y, double alpha, Scalar color) {
    double length = 100.0;
    double edge1 = length * sin(alpha);
    double edge2 = sqrt(length * length - edge1 * edge1);
    double xPrime = x + edge2, yPrime = y + edge1;

    arrowedLine(image, Point(x, y), Point(xPrime, yPrime), color, 3);
}

void drawBoundingBox(Mat& image, RotatedRect boundingBox, Scalar color) {
    Point2f rect_points[4];
    boundingBox.points(rect_points);
    for (int i = 0; i < 4; i++) {
        line(image, rect_points[i], rect_points[(i + 1) % 4], color, 3);
    }
}

void calcHuMoments(Moments mo, vector<double>& huMoments) {
    double hu[7];
    HuMoments(mo, hu);

    for (double d : hu) {
        huMoments.push_back(d);
    }
    return;
}

double euclideanDistance(vector<double> features1, vector<double> features2) {
    double sum1 = 0, sum2 = 0, sumDifference = 0;
    for (int i = 0; i < features1.size(); i++) {
        sumDifference += (features1[i] - features2[i]) * (features1[i] - features2[i]);
        sum1 += features1[i] * features1[i];
        sum2 += features2[i] * features2[i];
    }
    return sqrt(sumDifference) / (sqrt(sum1) + sqrt(sum2));
}


// Classifying objects
string classifier(vector<vector<double>> featureVectors, vector<string> classNames, vector<double> currentFeature) {
    double THRESHOLD = 0.15;
    double distance = DBL_MAX;
    string className = " ";
    for (int i = 0; i < featureVectors.size(); i++) {
        vector<double> dbFeature = featureVectors[i];
        string dbClassName = classNames[i];
        double curDistance = euclideanDistance(dbFeature, currentFeature);
        if (curDistance < distance && curDistance < THRESHOLD) {
            className = dbClassName;
            distance = curDistance;
        }
    }
    return className;
}

string classifierKNN(vector<vector<double>> featureVectors, vector<string> classNames, vector<double> currentFeature, int K) {
    double THRESHOLD = 0.15;
    vector<double> distances;
    for (int i = 0; i < featureVectors.size(); i++) {
        vector<double> dbFeature = featureVectors[i];
        double distance = euclideanDistance(dbFeature, currentFeature);
        if (distance < THRESHOLD) {
            distances.push_back(distance);
        }
    }

    string className = " ";
    if (distances.size() > 0) {
        vector<int> sortedIdx;
        sortIdx(distances, sortedIdx, SORT_EVERY_ROW + SORT_ASCENDING);
        vector<string> firstKNames;
        int s = sortedIdx.size();
        map<string, int> nameCount;
        int range = min(s, K); 
        for (int i = 0; i < range; i++) {
            string name = classNames[sortedIdx[i]];
            if (nameCount.find(name) != nameCount.end()) {
                nameCount[name]++;
            }
            else {
                nameCount[name] = 1;
            }
        }

        int count = 0;
        for (map<string, int>::iterator it = nameCount.begin(); it != nameCount.end(); it++) {
            if (it->second > count) {
                className = it->first;
                count = it->second;
            }
        }
    }
    return className;
}


// Naming the detected objects to a key
string getClassName(char c) {
    std::map<char, string> myMap{
            {'w', "wallet"}, {'c', "card"}, {'k', "keys"}, {'m', "mobile"}, {'g', "pencap"}, {'p', "pen"}, {'e', "earphones"}, {'h', "hanky"}, 
            {'s', "spects"}, {'b', "bitcoin"},
    };
    return myMap[c];
}


// Writing the label and HU moments data to CSV file
void writeToCSV(string filename, vector<string> classNamesDB, vector<vector<double>> featuresDB) {
    ofstream csvFile;
    csvFile.open(filename, ofstream::trunc);
    for (int i = 0; i < classNamesDB.size(); i++) {
        csvFile << classNamesDB[i] << ",";
        for (int j = 0; j < featuresDB[i].size(); j++) {
            csvFile << featuresDB[i][j];
            if (j != featuresDB[i].size() - 1) {
                csvFile << ",";
            }
        }
        csvFile << ";";
    }
}


// To classify the data again
void loadFromCSV(string filename, vector<string>& classNamesDB, vector<vector<double>>& featuresDB) {
    
    ifstream csvFile(filename);
    if (csvFile.is_open()) {
        
        string line;
        while (getline(csvFile, line)) {
            vector<string> currLine; 
            int pos = 0;
            string token;
            while ((pos = line.find(",")) != string::npos) {
                token = line.substr(0, pos);
                currLine.push_back(token);
                line.erase(0, pos + 1);
            }
            currLine.push_back(line);

            vector<double> currFeature;
            if (currLine.size() != 0) {
                classNamesDB.push_back(currLine[0]);
                for (int i = 1; i < currLine.size(); i++) {
                    currFeature.push_back(stod(currLine[i]));
                }
                featuresDB.push_back(currFeature);
            }
        }
    }
}