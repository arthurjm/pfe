#ifndef __SH_DIST__
#define __SH_DIST__

#include "rangeimage.h"
#include <stdlib.h>
using namespace std;

#define SHDIST_RAW 0
#define SHDIST_GRAY 1
#define SHDIST_BGR 2
#define SHDIST_LAB 3

#define SHDIST_EUCLIDIAN 0
#define SHDIST_MEAN 1

class SHDist
{
public:
    /**
    * 
    **/
    SHDist(RangeImage &ri, int type, vector<int> idx);
    ~SHDist();

    /**
    * Compute the distance between u and v according to data and spatial weight
    * @param u first index
    * @param v second index 
    * @param spatial2D using the spatial distance 2D during the dist computing
    * @return the distance  
    **/
    int computeDist(int u, int v, int mod, bool spatial2D);

private:
    cv::Mat convertRawDataToType(int idx);
    int euclidianDistance(vector<float> vecX, vector<float> vecY);
    int meanDistance(vector<float> vecX, vector<float> vecY);
    vector<int> _index;
    RangeImage _rangeimage;
    vector<cv::Mat> _vecData;
    int _type;
};

#endif