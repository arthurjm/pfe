#ifndef __SH_DIST__
#define __SH_DIST__

#include "rangeimage.h"
#include <stdlib.h>
using namespace std;

// Macro type of data
#define SHDIST_RAW 0
#define SHDIST_GRAY 1
#define SHDIST_BGR 2
#define SHDIST_LAB 3

// Macro Distance
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
    * @param mod type od distance computation, check SHDIST macro 
    * @param spatial2D using the spatial distance 2D during the dist computing
    * @return the distance 
    **/
    int computeDist(int u, int v, int mod, bool spatial2D);

    /**
     * Merge data of u and pu in pu
     * @param u index of vertex u
     * @param pu index of parent vertex u
     * @param sizeU number of element merge in u 
     * @param sizePu number of element merge in pu
     * */
    void mergeData(int u, int pu, int sizeU, int sizePu);

private:
    /**
    * convert raw data at index idx to type define in constructor and return cv::Mat associate
    * @param idx index of raw data of the range image
    * @return cv::Mat of data convert
    * */
    cv::Mat convertRawDataToType(int idx);

    /**
     * calcul the euclidian distance beetween member of vecX and vecY
     * @param vecU  vector with the differents elements of X vertex
     * @param vecV  vector with the differents elements of V vertex
     * @return the euclidian distance of vecX and vecY
     * */
    int euclidianDistance(vector<float> vecU, vector<float> vecV);

    /**
     * calcul the mean distance beetween member of vecX and vecY
     * @param vecU  vector with the differents elements of X vertex
     * @param vecV  vector with the differents elements of V vertex
     * @return the mean distance of vecX and vecY
     * */
    int meanDistance(vector<float> vecU, vector<float> vecV);

    vector<int> _index;
    RangeImage _rangeimage;
    vector<cv::Mat> _vecData;
    int _type;
};

#endif