#ifndef __RANGE_IMAGE__
#define __RANGE_IMAGE__

#include <stdlib.h>
#include <string>

#include "NumCpp.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

#define DIM 6

using namespace std;

typedef struct riVertex
{
    float x;
    float y;
    float z;
    float depth;
    float remission;
    float label;
} riVertex;

class RangeImage
{

public:
    /**
     * Range image must be a raw binary format (.bin) with (h * w * 6) as dimension
     * @param fileName location of range image
     * @param width width of the range image
     * @param height height of the range image
     **/
    RangeImage(string fileName, int width = 1024, int height = 64);

    /**
     * @return an vector BGR from XYZ
     * */
    cv::Mat createMatFromXYZ();

private:
    /**
     * Save data of the range image in _data structure and update min/max values of different attributes
     * @param fileName location of range image
     **/
    void loadRangeImage(string fileName);
    
    /**
     * According to idx, normalize the associated data and assign it to an vector
     * @param idx an verctor helps to indicate the attribute of riVertex
     * @return an vector of normalized data
     * */
    vector<uchar> normalizedValue(vector<int> idx);

    /**
     * Transform a range image to openCV matrice 
     * @param 
     * @return openCV matrice
     **/
    cv::Mat createCvMat(vector<uchar> dataBGR);

    riVertex *_data;
    int _width;
    int _height;

    // x, y, z, depth
    float _minValue[4]; 
    float _maxValue[4]; 
};

#endif