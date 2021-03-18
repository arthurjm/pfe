#ifndef __RANGE_IMAGE__
#define __RANGE_IMAGE__

#include <stdlib.h>
#include <string>

#include "NumCpp.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

#include <iostream>
#include <chrono>
#include <ctime>

#define DIM 6
// define the index of each canal in riVertex structure the raw data
// or the combination of canal x,y,z for the display
#define RI_X 0
#define RI_Y 1
#define RI_Z 2
#define RI_DEPTH 3
#define RI_REMISSION 4
#define RI_LABEL 5
#define RI_XYZ 6
// define the kernel half-size
#define RI_INTERPOLATE_HS_X 0
#define RI_INTERPOLATE_HS_Y 2

#define WIDTH 1024
#define HEIGHT 64

// define the default value for projection pointcloud
#define FOV_UP 3.0
#define FOV_DOWN -25.0

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
     * Destructor 
     * */
    ~RangeImage();

    /**
     * Default constructor
     * */
    RangeImage() : _data(nullptr) {}

    /**
     * Copy constructor
     * @param ri lvalue of RangeImage object
     * */
    RangeImage(RangeImage &ri);

    /**
     * Move constructor
     * @param ri rvalue of RangeImage object
     * */
    RangeImage(RangeImage &&ri);

    /**
     * Range image must be a raw binary format (.bin) with (h * w * 6) as dimension
     * @param data Range image values (x, y, z, depth, remission, label)
     * @param width width of the range image
     * @param height height of the range image
     **/
    RangeImage(riVertex *data, int width = WIDTH, int height = HEIGHT);

    /** 
     * Copy assignement 
     * @param ri lvalue of RangeImage object
     * @return a reference to this object
     * */
    RangeImage &operator=(const RangeImage &ri);

    /**
     * Move assignement 
     * @param ri rvalue of RangeImage object
     * @return a reference to this object
     * */
    RangeImage &operator=(RangeImage &&ri);

    /**
     * Create a gray image according to the associate attribut at idx index,
     * use opencv to apply color map and return the cv::Mat corresponding.
     * @param idx indicate the attribut
     * @param isGray boolean to indicate which color to generate (Gray/BGR)
     * @param interpolate boolean to activate interpolation
     * @param closing boolean to activate closing morphologie
     * @param equalHist boolean to activate equalize histogram
     * @return an cv::Mat contain the range image
     * */
    cv::Mat createColorMat(std::vector<int> idx, bool isGray = false, bool interpolate = false, bool closing = false, bool equalHist = false);

    /** 
     * Access _data with read only permission
     * @return an const pointer of _data
     * */
    const riVertex *getData();

    /**
    * Create a cv::Mat of raw data corresponding at the index   
    * @param index index of the data (RI_X, RI_Y, ...)
    * @return a cv::Mat with raw data 
    * */
    cv::Mat getRawDataFromIndex(int index);

    /**
     * Normalize all the data associated with a pixel in range of 0 and 1
     * ignore invalid data where remission = -1
     * @param pixel index of pixel in range image
     * @return a normalized riVertex
     * */
    riVertex getNormalizedValue(int pixelIndex);

    /** 
     * Change the label at the index in the range image data
     * @param index in range image 
     * @param label new label for the range image 
     * */
    void setLabel(int index, int label);

    /** 
     * Access the height of the range image
     * @return the height
     * */
    int getHeight();

    /** 
     * Access the width of the range image
     * @return the width
     * */
    int getWidth();

    /**
     * Access to the array contain the normalized and interpolated raw data (x,y,z,remission)
     * Values with remission == -1 and label == -1 are interpolated
     * @return const std::vector<float> *
     * */
    const std::vector<float> *getNormalizedAndInterpolatedData();

private:
    /**
     * Set label to -2 for unwanted components which contain no information 
     * (ex : dead pixels on the bottom)
     * it should be used for future interpolation 
     * */
    void separateInvalidComposant();

    /**
     * According to idx, normalize the associated data in range between 0 and 1 and assign it to an vector
     * @param idx an verctor helps to indicate the attribute of riVertex
     * @return an vector of normalized data
     * */
    std::vector<float> normalizedValue(std::vector<int> idx);

    /**
     * Apply interpolation on dead pixels (remission == -1 and label != -2).
     * Allowed number of components are 1,2,3,4
     * @param dataColor an array contains image information 
     * @param haflsizeX halfsize X of the kernel 
     * @param halfsizeY halfsize Y of the kernel 
     * @param nbComponent indicate the number of components in dataColor
     * */
    void interpolation(std::vector<float> &data, int halfsizeX, int halfsizeY, int nbComponent);

    /**
     * Transform a data vector to an openCV matrice 
     * @param data vector contain the information of the image generated 
     * @param type only CV_8UC3 and CV_8UC1 are allowed
     * @return openCV matrice
     **/
    cv::Mat createCvMat(std::vector<uchar> data, int type = CV_8UC3);

    /**
     * Apply closing morphology to input image
     * @param img input matrices with format CV_8UC3
     * @return an opencv Mat
     * */
    cv::Mat morphClose(cv::Mat img);

    /**
     * Apply opening morphology to input image
     * @param img input matrices with format CV_8UC3
     * @return an opencv Mat
     * */
    cv::Mat morphOpen(cv::Mat img);
    /**
     * Apply dilation morphology to input image
     * @param img input matrices with format CV_8UC3
     * @return an opencv Mat
     * */
    cv::Mat morphDilate(cv::Mat img);
    /**
     * Apply erosion morphology to input image
     * @param img input matrices with format CV_8UC3
     * @return an opencv Mat
     * */
    cv::Mat morphErode(cv::Mat img);

    // contain the raw datas of the range image
    riVertex *_data;
    // contain normalized and intepolated raw data
    std::vector<float> _normalizedAndInterpolatedData;

    int _width = WIDTH;
    int _height = HEIGHT;

    // stock the minimum and maximum of canal x, y, z, depth (use to normalized)
    float _minValue[4];
    float _maxValue[4];
};

#endif