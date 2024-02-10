// Vinesh Krishna Anne
// Project-3: 2-D object detection
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "Header.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "check input" << endl;
        exit(-1);
    }
    vector<string> classNamesDB;
    vector<vector<double>> featuresDB;
    loadFromCSV(argv[1], classNamesDB, featuresDB);
    VideoCapture* capdev;
    capdev = new VideoCapture(0);
    if (!capdev->isOpened()) {
        cout << "Unable to open video device\n";
        return -1;
    }
    
    namedWindow("Video", 1);

    Mat frame;
    bool training = false;

    while (true) {
        *capdev >> frame;
        if (frame.empty()) {
            cout << "frame is empty\n";
            break;
        }

        char key = waitKey(10); 

        //
        if (key == 't') {
            training = !training;
            if (training) {
                cout << "Training Mode" << endl;
            }
            else {
                cout << "Inference Mode" << endl;
            }
        }

        //Calling threshold function and displaying in seperate window
        namedWindow("threshFrame", 2);
        Mat thresholdFrame = threshold(frame);
        imshow("threshFrame", thresholdFrame);
        //Calling cleanup function and displaying in seperate window
        namedWindow("cleanupFrame", 3);
        Mat cleanupFrame = cleanup(thresholdFrame);
        imshow("cleanupFrame", cleanupFrame);
        //Calling region fuction and displaying in seperate window
        namedWindow("regionFrame", 4);
        Mat labeledRegions, stats, centroids;
        vector<int> topNLabels;
        Mat regionFrame = getRegions(cleanupFrame, labeledRegions, stats, centroids, topNLabels);
        imshow("regionFrame", regionFrame);

        for (int n = 0; n < topNLabels.size(); n++) {
            int label = topNLabels[n];
            Mat region;
            region = (labeledRegions == label);
            
            Moments m = moments(region, true);
            double centroidX = centroids.at<double>(label, 0);
            double centroidY = centroids.at<double>(label, 1);
            double alpha = 1.0 / 2.0 * atan2(2 * m.mu11, m.mu20 - m.mu02);
            
            RotatedRect boundingBox = getBoundingBox(region, centroidX, centroidY, alpha);
            drawLine(frame, centroidX, centroidY, alpha, Scalar(0, 0, 255));
            drawBoundingBox(frame, boundingBox, Scalar(0, 255, 0));

            
            vector<double> huMoments;
            calcHuMoments(m, huMoments);
            //training mode
            if (training) {
                namedWindow("Current Region", WINDOW_AUTOSIZE);
                imshow("Current Region", region);
                cout << "Input the class for this object." << endl;
                char k = waitKey(0);
                string className = getClassName(k);
                featuresDB.push_back(huMoments);
                classNamesDB.push_back(className);
                if (n == topNLabels.size() - 1) {
                    training = false;
                    cout << "Inference Mode" << endl;
                    destroyWindow("Current Region");
                }
            }
            //Defining class name
            else {
                string className;
                if (!strcmp(argv[2], "n")) { 
                    className = classifier(featuresDB, classNamesDB, huMoments);
                }
                else if (!strcmp(argv[2], "k")) {
                    className = classifierKNN(featuresDB, classNamesDB, huMoments, 5);
                }
                putText(frame, className, Point(centroids.at<double>(label, 0), centroids.at<double>(label, 1)), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255), 3);
            }
        }
        // Live Video display window
        imshow("Video", frame); 
        // key 'q' to exit the process
        if (key == 'q') {
            //writing to CSV file
            writeToCSV(argv[1], classNamesDB, featuresDB);
            break;
        }
    }
    return 0;
}