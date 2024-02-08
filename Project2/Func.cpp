/*
    Vinesh Krishna Anne
    CS 5330 Pattern Recognition and Computer Vision
    Spring 2023

    CPP functions for reading CSV files with a specific format
    - first column is a string containing a filename or path
    - every other column is a number

    The function returns a std::vector of char* for the filenames and a 2D std::vector of floats for the data
*/
#include <cstdio>
#include <cstring>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Header.h"

/*
  reads a string from a CSV file. the 0-terminated string is returned in the char array os.

  The function returns false if it is successfully read. It returns true if it reaches the end of the line or the file.
 */
int getstring(FILE* fp, char os[]) {
    int p = 0;
    int eol = 0;

    for (;;) {
        char ch = fgetc(fp);
        if (ch == ',') {
            break;
        }
        else if (ch == '\n' || ch == EOF) {
            eol = 1;
            break;
        }
        // printf("%c", ch ); // uncomment for debugging
        os[p] = ch;
        p++;
    }
    // printf("\n"); // uncomment for debugging
    os[p] = '\0';

    return(eol); // return true if eol
}

int getint(FILE* fp, int* v) {
    char s[256];
    int p = 0;
    int eol = 0;

    for (;;) {
        char ch = fgetc(fp);
        if (ch == ',') {
            break;
        }
        else if (ch == '\n' || ch == EOF) {
            eol = 1;
            break;
        }

        s[p] = ch;
        p++;
    }
    s[p] = '\0'; // terminator
    *v = atoi(s);

    return(eol); // return true if eol
}

/*
  Utility function for reading one float value from a CSV file

  The value is stored in the v parameter

  The function returns true if it reaches the end of a line or the file
 */
int getfloat(FILE* fp, float* v) {
    char s[256];
    int p = 0;
    int eol = 0;

    for (;;) {
        char ch = fgetc(fp);
        if (ch == ',') {
            break;
        }
        else if (ch == '\n' || ch == EOF) {
            eol = 1;
            break;
        }

        s[p] = ch;
        p++;
    }
    s[p] = '\0'; // terminator
    *v = atof(s);

    return(eol); // return true if eol
}

/*
  Given a filename, and image filename, and the image features, by
  default the function will append a line of data to the CSV format
  file.  If reset_file is true, then it will open the file in 'write'
  mode and clear the existing contents.

  The image filename is written to the first position in the row of
  data. The values in image_data are all written to the file as
  floats.

  The function returns a non-zero value in case of an error.
 */
int appendcsv(char* filename, char* image_filename, std::vector<float>& image_data, int reset_file) {
    char buf[256];
    char mod[8];
    FILE* fp;

    strcpy(mod, "a");

    if (reset_file) {
        strcpy(mod, "w");
    }

    fp = fopen(filename, mod);
    if (!fp) {
        printf("Unable to open output file %s\n", filename);
        exit(-1);
    }

    // write the filename and the feature vector to the CSV file
    strcpy(buf, image_filename);
    std::fwrite(buf, sizeof(char), strlen(buf), fp);
    for (int i = 0; i < image_data.size(); i++) {
        char tmp[256];
        sprintf_s(tmp, ",%.4f", image_data[i]);
        std::fwrite(tmp, sizeof(char), strlen(tmp), fp);
    }

    std::fwrite("\n", sizeof(char), 1, fp); // EOL

    fclose(fp);

    return(0);
}

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
int readcsv(char* filename, std::vector<char*>& filenames, std::vector<std::vector<float>>& data, int echo_file) {
    FILE* fp;
    float fval;
    char imagefile[256];

    fp = fopen(filename, "r");
    if (!fp) {
        printf("Unable to open feature file\n");
        return(-1);
    }

    printf("Reading %s\n", filename);
    for (;;) {
        std::vector<float> dvec;


        // read the filename
        if (getstring(fp, imagefile)) {
            break;
        }
        // printf("Evaluting %s\n", filename);

        // read the whole feature file into memory
        for (;;) {
            // get next feature
            float eol = getfloat(fp, &fval);
            dvec.push_back(fval);
            if (eol) break;
        }
        // printf("read %lu features\n", dvec.size() );

        data.push_back(dvec);

        char* fname = new char[strlen(imagefile) + 1];
        strcpy(fname, imagefile);
        filenames.push_back(fname);
    }
    fclose(fp);
    printf("Finished reading CSV file\n");

    if (echo_file) {
        for (int i = 0; i < data.size(); i++) {
            for (int j = 0; j < data[i].size(); j++) {
                printf("%.4f  ", data[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    return(0);
}

std::vector<float> task1(char* filepath) {
    std::vector<float> fvec;

    //reading the image path
    cv::Mat image = cv::imread(filepath);

    //creating the reagion lof interest
    cv::Rect roi((image.rows / 2) - (9 / 2), (image.cols / 2) - (9 / 2), 9, 9);
    cv::Mat f = image(roi);

    //creating the feature vector
    for (int i = 0; i < f.rows; i++) {
        cv::Vec3b* a = f.ptr<cv::Vec3b>(i);
        for (int j = 0; j < f.cols; j++) {
            for (int c = 0; c < 3; c++) {
                fvec.push_back(a[j][c]);
            }
        }
    }
    return fvec;
}

std::vector<float> task2(char* filepath) {
    std::vector<float> fvec;

    //reading the image path
    cv::Mat image = cv::imread(filepath);

    //creating the blue and green 2D histogram
    cv::Mat histbg = cv::Mat::zeros(16, 16, CV_32SC1);
    for (int i = 0; i < image.rows; i++) {
        cv::Vec3b* imgptr = image.ptr<cv::Vec3b>(i);
        for (int j = 0; j < image.cols; j++) {
            int indexb = int((float(imgptr[j][0]) / 256) * 16);
            int indexg = int((float(imgptr[j][1]) / 256) * 16);

            int* histbgptr = histbg.ptr<int>(indexb);
            histbgptr[indexg] = histbgptr[indexg] + 1;
        }
    }

    //creating the feature vector
    for (int i = 0; i < histbg.rows; i++) {
        int* histbgptr = histbg.ptr<int>(i);
        for (int j = 0; j < histbg.cols; j++) {
            fvec.push_back(histbgptr[j]);
        }
    }
    return fvec;
}

std::vector<float> task3(char* filepath) {
    std::vector<float> fvec;

    //reading the image path
    cv::Mat image = cv::imread(filepath);
    const int size[] = { 8, 8, 8 };
    cv::Mat bgrhista(3, size, CV_32SC1, cv::Scalar(0));
    cv::Mat bgrhistb(3, size, CV_32SC1, cv::Scalar(0));

    //creating the region of interest
    cv::Rect roi((image.rows / 2) - (250 / 2), (image.cols / 2) - (350 / 2), 350, 250);
    cv::Mat imageb = image(roi);

    //creating the BGR 3D histogram from total image
    for (int i = 0; i < image.rows; i++) {
        cv::Vec3b* imageptra = image.ptr<cv::Vec3b>(i);
        for (int j = 0; j < image.cols; j++) {
            int indexb = int((float(imageptra[j][0]) / 256) * 8);
            int indexg = int((float(imageptra[j][1]) / 256) * 8);
            int indexr = int((float(imageptra[j][2]) / 256) * 8);

            int* bgrhistptra = bgrhista.ptr<int>(indexb);
            bgrhistptra[8 * (indexg)+indexr] += 1;
        }
    }

    //creating the BGR 3D histogram from inner image
    for (int i = 0; i < imageb.rows; i++) {
        cv::Vec3b* imageptrb = imageb.ptr<cv::Vec3b>(i);
        for (int j = 0; j < imageb.cols; j++) {
            int indexb = int((float(imageptrb[j][0]) / 256) * 8);
            int indexg = int((float(imageptrb[j][1]) / 256) * 8);
            int indexr = int((float(imageptrb[j][2]) / 256) * 8);

            int* bgrhistptrb = bgrhistb.ptr<int>(indexb);
            bgrhistptrb[8 * (indexg)+indexr] += 1;
        }
    }

    //creating the feature vector
    for (int i = 0; i < 8; i++) {
        int* bgrhistptra = bgrhista.ptr<int>(i);
        for (int j = 0; j < 8; j++) {
            for (int c = 0; c < 8; c++) {
                fvec.push_back(bgrhistptra[8 * j + c]);
            }
        }
    }
    fvec.push_back(0.1);      
    //pushing something random to distinguish both histograms 

    for (int i = 0; i < 8; i++) {
        int* bgrhistptrb = bgrhistb.ptr<int>(i);
        for (int j = 0; j < 8; j++) {
            for (int c = 0; c < 8; c++) {
                fvec.push_back(bgrhistptrb[8 * j + c]);
            }
        }
    }
    return fvec;
}

std::vector<float> taska4(char* filepath) {
    std::vector<float> fvec;

    //reading the image path
    cv::Mat img = cv::imread(filepath);

    //creating the BG 2D histogram for the image
    cv::Mat bg_hist = cv::Mat::zeros(16, 16, CV_32SC1);
    for (int i = 0; i < img.rows; i++) {
        cv::Vec3b* img_ptr = img.ptr<cv::Vec3b>(i);
        for (int j = 0; j < img.cols; j++) {
            int b_index = int((float(img_ptr[j][0]) / 256) * 16);
            int g_index = int((float(img_ptr[j][1]) / 256) * 16);

            int* bg_hist_ptr = bg_hist.ptr<int>(b_index);
            bg_hist_ptr[g_index] = bg_hist_ptr[g_index] + 1;
        }
    }

    cv::Mat gradX, gradY, gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);        //converting image into grayscale
    // Sobel filter for gradient in X direction
    Sobel(gray, gradX, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    // Sobel filter for gradient in Y direction
    Sobel(gray, gradY, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
    convertScaleAbs(gradX, gradX);
    convertScaleAbs(gradY, gradY);

    //creating the region of interest
    cv::Rect r1(0, 0, gradX.cols / 2, gradX.rows / 2);
    cv::Rect r2(0, 0, gradY.cols / 2, gradY.rows / 2);
    cv::Mat roi1 = gradX(r1);
    cv::Mat roi2 = gradY(r2);

    //creating 2D histogram with magnitude and orientation information
    cv::Mat hist = cv::Mat::zeros(16, 16, CV_32SC1);
    for (int i = 0; i < roi1.rows; i++) {
        cv::Vec3b* x_ptr1 = roi1.ptr<cv::Vec3b>(i);
        cv::Vec3b* y_ptr1 = roi2.ptr<cv::Vec3b>(i);

        for (int j = 0; j < roi1.cols; j++) {
            int magindex = int((std::sqrt(x_ptr1[j][0] * x_ptr1[j][0] + y_ptr1[j][0] * y_ptr1[j][0])) / 45.078);       //dividing into 8 bins 
            int orientationindex = int((atan2(y_ptr1[j][0], x_ptr1[j][0]) + 3.14) / 0.785);        //dividing into 8 bins

            cv::Vec3b* hist_ptr = hist.ptr<cv::Vec3b>(magindex);
            hist_ptr[orientationindex][0]++;
        }
    }

    //creating the feature vector
    for (int i = 0; i < bg_hist.rows; i++) {
        cv::Vec3b* bg_hist_ptr = bg_hist.ptr<cv::Vec3b>(i);
        for (int j = 0; j < bg_hist.cols; j++) {
            fvec.push_back(bg_hist_ptr[j][0]);
        }
    }
    fvec.push_back(0.1);        //pushing something random to distinguish the histograms
    for (int i = 0; i < hist.rows; i++) {
        cv::Vec3b* hist_ptr = hist.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hist.cols; j++) {
            fvec.push_back(hist_ptr[j][0]);
        }
    }
    fvec.push_back(0.1);        //pushing something random to distinguish the histograms
    for (int i = 0; i < hist.rows; i++) {
        cv::Vec3b* hist_ptr = hist.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hist.cols; j++) {
            fvec.push_back(hist_ptr[j][0]);
        }
    }
    fvec.push_back(0.1);        //pushing something random to distinguish the histograms
    for (int i = 0; i < hist.rows; i++) {
        cv::Vec3b* hist_ptr = hist.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hist.cols; j++) {
            fvec.push_back(hist_ptr[j][0]);
        }
    }
    fvec.push_back(0.1);        //pushing something random to distinguish the histograms
    for (int i = 0; i < hist.rows; i++) {
        cv::Vec3b* hist_ptr = hist.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hist.cols; j++) {
            fvec.push_back(hist_ptr[j][0]);
        }
    }

    return fvec;
}

std::vector<float> taskb4(char* filepath) {
    std::vector<float> fvec;

    //reading the image path
    cv::Mat img = cv::imread(filepath);

    cv::Mat bg_hist = cv::Mat::zeros(16, 16, CV_32SC1);

    //creating the BG 2D histogram
    for (int i = 0; i < img.rows; i++) {
        cv::Vec3b* img_ptr = img.ptr<cv::Vec3b>(i);
        for (int j = 0; j < img.cols; j++) {
            int b_index = int((float(img_ptr[j][0]) / 256) * 16);
            int g_index = int((float(img_ptr[j][1]) / 256) * 16);

            int* bg_hist_ptr = bg_hist.ptr<int>(b_index);
            bg_hist_ptr[g_index] = bg_hist_ptr[g_index] + 1;
        }
    }

    cv::Mat gradX, gradY, gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);        //converting image into grayscale

    // Sobel filter for gradient in X direction
    Sobel(gray, gradX, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    // Sobel filter for gradient in Y direction
    Sobel(gray, gradY, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
    convertScaleAbs(gradX, gradX);
    convertScaleAbs(gradY, gradY);

    //diving the image into 4 quadrants and creating the region of interests 
    cv::Rect r1_q1(0, 0, gradX.cols / 2, gradX.rows / 2);
    cv::Rect r2_q1(0, 0, gradY.cols / 2, gradY.rows / 2);
    cv::Rect r1_q2(gradX.cols / 2, 0, gradX.cols / 2, gradX.rows / 2);
    cv::Rect r2_q2(gradX.cols / 2, 0, gradY.cols / 2, gradY.rows / 2);
    cv::Rect r1_q3(0, gradX.rows / 2, gradX.cols / 2, gradX.rows / 2);
    cv::Rect r2_q3(0, gradX.rows / 2, gradY.cols / 2, gradY.rows / 2);
    cv::Rect r1_q4(gradX.cols / 2, gradX.rows / 2, gradX.cols / 2, gradX.rows / 2);
    cv::Rect r2_q4(gradX.cols / 2, gradX.rows / 2, gradY.cols / 2, gradY.rows / 2);
    cv::Mat roi1_q1 = gradX(r1_q1);
    cv::Mat roi2_q1 = gradY(r2_q1);
    cv::Mat roi1_q2 = gradX(r1_q2);
    cv::Mat roi2_q2 = gradY(r2_q2);
    cv::Mat roi1_q3 = gradX(r1_q3);
    cv::Mat roi2_q3 = gradY(r2_q3);
    cv::Mat roi1_q4 = gradX(r1_q4);
    cv::Mat roi2_q4 = gradY(r2_q4);

    //creating four 2D histograms with magnitude and orientation
    cv::Mat hist_q1 = cv::Mat::zeros(16, 16, CV_32SC1);
    cv::Mat hist_q2 = cv::Mat::zeros(16, 16, CV_32SC1);
    cv::Mat hist_q3 = cv::Mat::zeros(16, 16, CV_32SC1);
    cv::Mat hist_q4 = cv::Mat::zeros(16, 16, CV_32SC1);
    for (int i = 0; i < roi1_q1.rows; i++) {
        cv::Vec3b* x_ptr1 = roi1_q1.ptr<cv::Vec3b>(i);
        cv::Vec3b* y_ptr1 = roi2_q1.ptr<cv::Vec3b>(i);

        for (int j = 0; j < roi1_q1.cols; j++) {
            int mag_index = int((std::sqrt(x_ptr1[j][0] * x_ptr1[j][0] + y_ptr1[j][0] * y_ptr1[j][0])) / 45.078);
            int orientation_index = int((atan2(y_ptr1[j][0], x_ptr1[j][0]) + 3.14) / 0.785);

            cv::Vec3b* hist_q1_ptr = hist_q1.ptr<cv::Vec3b>(mag_index);
            hist_q1_ptr[orientation_index][0]++;
        }
    }
    for (int i = 0; i < roi1_q2.rows; i++) {
        cv::Vec3b* x_ptr1 = roi1_q2.ptr<cv::Vec3b>(i);
        cv::Vec3b* y_ptr1 = roi2_q2.ptr<cv::Vec3b>(i);

        for (int j = 0; j < roi1_q2.cols; j++) {

            int mag_index = int((std::sqrt(x_ptr1[j][0] * x_ptr1[j][0] + y_ptr1[j][0] * y_ptr1[j][0])) / 45.078);
            int orientation_index = int((atan2(y_ptr1[j][0], x_ptr1[j][0]) + 3.14) / 0.785);

            cv::Vec3b* hist_q2_ptr = hist_q2.ptr<cv::Vec3b>(mag_index);
            hist_q2_ptr[orientation_index][0]++;
        }
    }
    for (int i = 0; i < roi1_q3.rows; i++) {
        cv::Vec3b* x_ptr1 = roi1_q3.ptr<cv::Vec3b>(i);
        cv::Vec3b* y_ptr1 = roi2_q3.ptr<cv::Vec3b>(i);

        for (int j = 0; j < roi1_q3.cols; j++) {

            int magindex = int((std::sqrt(x_ptr1[j][0] * x_ptr1[j][0] + y_ptr1[j][0] * y_ptr1[j][0])) / 45.078);
            int orientationindex = int((atan2(y_ptr1[j][0], x_ptr1[j][0]) + 3.14) / 0.785);

            cv::Vec3b* hist_q3_ptr = hist_q3.ptr<cv::Vec3b>(magindex);
            hist_q3_ptr[orientationindex][0]++;
        }
    }
    for (int i = 0; i < roi1_q4.rows; i++) {
        cv::Vec3b* x_ptr1 = roi1_q4.ptr<cv::Vec3b>(i);
        cv::Vec3b* y_ptr1 = roi2_q4.ptr<cv::Vec3b>(i);

        for (int j = 0; j < roi1_q4.cols; j++) {

            int magindex = int((std::sqrt(x_ptr1[j][0] * x_ptr1[j][0] + y_ptr1[j][0] * y_ptr1[j][0])) / 45.078);
            int orientationindex = int((atan2(y_ptr1[j][0], x_ptr1[j][0]) + 3.14) / 0.785);

            cv::Vec3b* hist_q4_ptr = hist_q4.ptr<cv::Vec3b>(magindex);
            hist_q4_ptr[orientationindex][0]++;
        }
    }

    //creating the feature vector
    for (int i = 0; i < bg_hist.rows; i++) {
        cv::Vec3b* bg_hist_ptr = bg_hist.ptr<cv::Vec3b>(i);
        for (int j = 0; j < bg_hist.cols; j++) {
            fvec.push_back(bg_hist_ptr[j][0]);
        }
    }
    fvec.push_back(0.1);        //pushing something random to distinguish the histograms
    for (int i = 0; i < hist_q1.rows; i++) {
        cv::Vec3b* hist_q1_ptr = hist_q1.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hist_q1.cols; j++) {
            fvec.push_back(hist_q1_ptr[j][0]);
        }
    }
    fvec.push_back(0.1);        //pushing something random to distinguish the histograms
    for (int i = 0; i < hist_q2.rows; i++) {
        cv::Vec3b* hist_q2_ptr = hist_q2.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hist_q2.cols; j++) {
            fvec.push_back(hist_q2_ptr[j][0]);
        }
    }
    fvec.push_back(0.1);        //pushing something random to distinguish the histograms
    for (int i = 0; i < hist_q3.rows; i++) {
        cv::Vec3b* hist_q3_ptr = hist_q3.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hist_q3.cols; j++) {
            fvec.push_back(hist_q3_ptr[j][0]);
        }
    }
    fvec.push_back(0.1);        //pushing something random to distinguish the histograms
    for (int i = 0; i < hist_q4.rows; i++) {
        cv::Vec3b* hist_q4_ptr = hist_q4.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hist_q4.cols; j++) {
            fvec.push_back(hist_q4_ptr[j][0]);
        }
    }
    return fvec;
}