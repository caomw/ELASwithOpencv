//
// Created by zsk on 16-8-19.
//
#include<iostream>
#include<algorithm>
#include<fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iomanip>
#include "elas.h"

using namespace std;


void LoadImages(const string &strPathToSequence, vector<string> &vstrImageLeft,
                vector<string> &vstrImageRight, vector<double> &vTimestamps);

int main(int argc, char **argv)
{

    if(argc != 2)
    {
        cerr << endl << "Usage: path_to_sequence" << endl;
        return 1;
    }

    // Retrieve paths to images
    vector<string> vstrImageLeft;
    vector<string> vstrImageRight;
    vector<double> vTimestamps;
    LoadImages(string(argv[1]), vstrImageLeft, vstrImageRight, vTimestamps);

    const int nImages = vstrImageLeft.size();

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;
    cout << "Images in the sequence: " << nImages << endl << endl;

    // Main loop
    cv::Mat imLeft, imRight;
    for(int ni=0; ni<nImages; ni++)
    {
        // Read left and right images from file
        imLeft = cv::imread(vstrImageLeft[ni],CV_LOAD_IMAGE_UNCHANGED);
        imRight = cv::imread(vstrImageRight[ni],CV_LOAD_IMAGE_UNCHANGED);
        double tframe = vTimestamps[ni];

        if(imLeft.empty())
        {
            cerr << endl << "Failed to load image at: "
                 << string(vstrImageLeft[ni]) << endl;
            return 1;
        }

        const int width = imLeft.cols,
        height = imLeft.rows;
        const int32_t dims[3] = {width,height,width};
        Elas::parameters param;
        param.postprocess_only_left = false;
        Elas elas(param);
        cv::Mat D1(imLeft.rows,imLeft.cols,CV_32FC1),D2(imLeft.rows,imLeft.cols,CV_32FC1);
        float *D1_data = (float*)D1.data,
        *D2_data = (float*)D2.data;
        elas.process(imLeft.data,imRight.data,D1_data,D2_data,dims);
        float disp_max = 0;
        for (int32_t i=0; i<width*height; i++) {
            if (D1_data[i]>disp_max) disp_max = D1_data[i];
            if (D2_data[i]>disp_max) disp_max = D2_data[i];
        }

        // copy float to uchar
        cv::Mat D1u(height,width,CV_8UC1),D2u(height,width,CV_8UC1);
        for (int32_t i=0; i<width*height; i++) {
            D1u.data[i] = (uint8_t)max(255.0*D1_data[i]/disp_max,0.0);
            D2u.data[i] = (uint8_t)max(255.0*D2_data[i]/disp_max,0.0);
        }

        cv::imshow("aaa",D2u);
        cv::waitKey(33);


        cout << "-----------------------" << endl;
        cout << "img idx:" << ni << endl;

    }


    return 0;
}

void LoadImages(const string &strPathToSequence, vector<string> &vstrImageLeft,
                vector<string> &vstrImageRight, vector<double> &vTimestamps)
{
    ifstream fTimes;
    string strPathTimeFile = strPathToSequence + "/times.txt";
    fTimes.open(strPathTimeFile.c_str());
    while(!fTimes.eof())
    {
        string s;
        getline(fTimes,s);
        if(!s.empty())
        {
            stringstream ss;
            ss << s;
            double t;
            ss >> t;
            vTimestamps.push_back(t);
        }
    }

    string strPrefixLeft = strPathToSequence + "/image_0/";
    string strPrefixRight = strPathToSequence + "/image_1/";

    const int nTimes = vTimestamps.size();
    vstrImageLeft.resize(nTimes);
    vstrImageRight.resize(nTimes);

    for(int i=0; i<nTimes; i++)
    {
        stringstream ss;
        ss << setfill('0') << setw(6) << i;
        vstrImageLeft[i] = strPrefixLeft + ss.str() + ".png";
        vstrImageRight[i] = strPrefixRight + ss.str() + ".png";
    }
}
