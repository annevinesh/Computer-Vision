#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
    Mat image = imread("C:/Users/Vinesh/Desktop/bmbe.jpg");

    while 
        (char a = waitKey(0)) 
    {
        imshow("Display image", image);

        if (a == 'q') 
        {
            break;
        }
        else if (a == 'c') 
        {
            Mat image = imread("C:/Users/Vinesh/Desktop/bmbe.jpg", IMREAD_GRAYSCALE);
            imshow("Grayscale", image);
        }
        
    }
    return(0);
}