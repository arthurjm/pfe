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
    RangeImage() : _data(nullptr) {}

    /**
     * Range image must be a raw binary format (.bin) with (h * w * 6) as dimension
     * @param fileName location of range image
     * @param width width of the range image
     * @param height height of the range image
     **/
    RangeImage(string fileName, int width = 1024, int height = 64);

    /**
     * Create BGR image from XYZ coordinates 
     * @return an opencv Mat
     * */
    cv::Mat createImageFromXYZ();

    /**
     * Create gray image from depth
     * @return an opencv Mat
     * */
    cv::Mat createImageFromDepth();

    /**
     * Create gray image from remission
     * @return an opencv Mat
     * */
    cv::Mat createImageFromRemission();

    /** 
     * Access _data with read only permission
     * @return an const pointer of _data
     * */
    const riVertex* getData();

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

    void interpolationBGR(vector<uchar> &dataColor, int halfsizeX, int halfsizeY);

    /**
     * Transform a range image to openCV matrice 
     * @param dataColor 
     * @param type only CV_8UC3 and CV_8UC1 are allowed
     * @return openCV matrice
     **/
    cv::Mat createCvMat(vector<uchar> dataColor, int type = CV_8UC3);


    /**
     * Apply closing morphology to input image
     * 
     * @param img input matrices with format CV_8UC3
     * @return an opencv Mat
     * */
    cv::Mat morphClose(cv::Mat img);

    /**
     * Apply dilation morphology to input image
     * 
     * @param img input matrices with format CV_8UC3
     * @return an opencv Mat
     * */
    cv::Mat morphDilate(cv::Mat img);

    riVertex *_data;
    int _width;
    int _height;

    // x, y, z, depth
    float _minValue[4]; 
    float _maxValue[4]; 
};

#endif