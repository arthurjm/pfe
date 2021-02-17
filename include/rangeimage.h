#ifndef __RANGE_IMAGE__
#define __RANGE_IMAGE__

#include <stdlib.h>
#include <string>

#include "NumCpp.hpp"
#include "pointcloud.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#define DIM 6
#define RI_X 0
#define RI_Y 1
#define RI_Z 2
#define RI_DEPTH 3
#define RI_REMISSION 4
#define RI_LABEL 5
#define HEIGHT 64
#define WIDTH 1024
#define FOV_UP 3.0
#define FOV_DOWN -25.0

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

    RangeImage(PointCloud cp, int height = HEIGHT, int width = WIDTH,
               float proj_fov_up = FOV_UP, float proj_fov_down = FOV_DOWN);
    /**
     * Create BGR image from XYZ coordinates 
     * @return an opencv Mat
     * */
    cv::Mat createImageFromXYZ();

    /**
     * Create a gray image according to the associate attribut at idx index,
     * use opencv to apply color map and return the cv::Mat corresponding.
     * @param idx indicate the attribut
     * @param  interpolation boolean to activate interpolation
     * @param closing boolean to activate closing morphologie
     * @return an uchar array
     * */
    cv::Mat createBGRFromColorMap(int idx, bool interpolate = false, bool closing = false);

    /** 
     * Access _data with read only permission
     * @return an const pointer of _data
     * */
    const riVertex *getData();

    /** 
     * Access the height of the range image
     * @return
     * */
    int getHeight();

    int getWidth();

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
     * Apply interpolation on dead pixels (remission == -1).
     * @param dataColor an array contains image information 
     * @param haflsizeX halfsize X of the kernel 
     * @param halfsizeY halfsize Y of the kernel 
     * @param nbIter number of iteration
     * @param BGR boolean indicate if dataColor is a gray or color image
     * */
    void interpolation(vector<uchar> &dataColor, int halfsizeX, int halfsizeY, int nbIter, bool BGR);

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

    void pointCloudProjection(PointCloud cp, float proj_fov_up, float proj_fov_down);

    riVertex *_data;
    int _width;
    int _height;

    // x, y, z, depth
    float _minValue[4];
    float _maxValue[4];
};

#endif