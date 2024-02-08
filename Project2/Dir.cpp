/*
  Vinesh Krishna Anne
    
  Code to identify image files in a directory
*/
#include <dirent.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "Header.h"
/*
  Given a directory on the command line, scans through the directory for image files.

  Prints out the full path name for each file.  This can be used as an argument to fopen or to cv::imread.
 */
int main(int argc, char* argv[]) {
    char dirname[256];
    char baseline[256];
    char histo[256];
    char multihisto[256];
    char TexNcolor[256];
    char buf[256];
    FILE* fp;
    DIR* dirp;
    struct dirent* dp;
    int i;
    // check for sufficient arguments
    if (argc < 2) {
        printf("usage: %s <directory path>\n", argv[0]);
        exit(-1);
    }
    // get the directory path
    strcpy(dirname, argv[1]);
    strcpy(baseline, "baseline.csv");
    strcpy(histo, "histo.csv");
    strcpy(multihisto, "multihisto.csv");
    strcpy(TexNcolor, "TexNcolor.csv");
    printf("Processing directory %s\n", dirname);
    // open the directory
    dirp = opendir(dirname);
    if (dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }
    // loop over all the files in the image file listing
    while ((dp = readdir(dirp)) != NULL) {
        // check if the file is an image
        if (strstr(dp->d_name, ".jpg") ||
            strstr(dp->d_name, ".png") ||
            strstr(dp->d_name, ".ppm") ||
            strstr(dp->d_name, ".tif")) {
            printf("processing image file: %s\n", dp->d_name);
            // build the overall filename
            strcpy(buf, dirname);
            strcat(buf, "/");
            strcat(buf, dp->d_name);
            printf("full path name: %s\n", buf);
            std::vector<float> task1vec = task1(buf);
            std::vector<float> task2vec = task2(buf);
            std::vector<float> task3vec = task3(buf);
            std::vector<float> task4vec = taskb4(buf);
            appendcsv(baseline, buf, task1vec);
            appendcsv(histo, buf, task2vec);
            appendcsv(multihisto, buf, task3vec);
            appendcsv(TexNcolor, buf, task4vec);
        }
    }
    printf("Terminating\n");
    return 0;
}