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

#define RI_X 0
#define RI_Y 1
#define RI_Z 2
#define RI_DEPTH 3
#define RI_REMISSION 4
#define RI_LABEL 5
#define RI_XYZ 6
#define RI_INTERPOLATE_HS_X 0
#define RI_INTERPOLATE_HS_Y 2

#define WIDTH 1024
#define HEIGHT 64

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
     * @param data Range image values (x, y, z, depth, remission, label)
     * @param width width of the range image
     * @param height height of the range image
     **/
    RangeImage(riVertex *data, int width = WIDTH, int height = HEIGHT);
    RangeImage(string pc, string labelFile, int width = WIDTH, int height = HEIGHT);
    /**
     * Create a gray image according to the associate attribut at idx index,
     * use opencv to apply color map and return the cv::Mat corresponding.
     * @param idx indicate the attribut
     * @param isGray boolean to indicate which color to generate (Gray/BGR)
     * @param interpolation boolean to activate interpolation
     * @param closing boolean to activate closing morphologie
     * @param equalHist boolean to activate equalize histogram
     * @return an uchar array
     * */
    cv::Mat createColorMat(std::vector<int> idx, bool isGray = false, bool interpolate = false, bool closing = false, bool equalHist = false);

    void pointCloudProjection(vector<float> scan_x, vector<float> scan_y,
                              vector<float> scan_z, vector<float> scan_remission, vector<uint16_t> labels,
                              float proj_fov_up, float proj_fov_down);
    /** 
     * Access _data with read only permission
     * @return an const pointer of _data
     * */
    const riVertex *getData();

    /**
    * return a cv::Mat of raw data corresponding at the index information  
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
     * change the label at the index in the range image data
     * @param index in range image 
     * @param label new label for the range image 
     * */
    void setLabel(int index, int label);

    /** 
     * Access the height of the range image
     * @return
     * */
    int getHeight();

    int getWidth();

    const std::vector<float> *getNormalizedData();

    /**
     * Save the current range image as binary file
     * */
    void save(std::string filename);

private:
    /**
     * Save data of the range image in _data structure and update min/max values of different attributes
     * @param fileName location of range image
     **/
    void loadRangeImage(std::string fileName);
    /**
     * Set label to -2 for unwanted components (ex : dead pixels on the bottom)
     * */
    void separateInvalideComposant();

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
     * Transform a range image to openCV matrice 
     * @param dataColor 
     * @param type only CV_8UC3 and CV_8UC1 are allowed
     * @return openCV matrice
     **/
    cv::Mat createCvMat(std::vector<uchar> dataColor, int type = CV_8UC3);

    /**
     * Apply closing morphology to input image
     * 
     * @param img input matrices with format CV_8UC3
     * @return an opencv Mat
     * */
    cv::Mat morphClose(cv::Mat img);

    cv::Mat morphOpen(cv::Mat img);
    /**
     * Apply dilation morphology to input image
     * 
     * @param img input matrices with format CV_8UC3
     * @return an opencv Mat
     * */
    cv::Mat morphDilate(cv::Mat img);

    cv::Mat morphErode(cv::Mat img);

    riVertex *_data;
    std::vector<float> _normalizedData;

    int _width = WIDTH;
    int _height = HEIGHT;

    // x, y, z, depth
    float _minValue[4];
    float _maxValue[4];
};

#endif