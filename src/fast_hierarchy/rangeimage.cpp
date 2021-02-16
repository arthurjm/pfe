#include "rangeimage.h"
#include <assert.h>

using namespace std;

RangeImage::RangeImage(string fileName, int width, int height)
    : _data(nullptr), _width(width), _height(height)
{
    _data = (riVertex *)malloc(sizeof(riVertex) * width * height);
    assert(_data);
    loadRangeImage(fileName);
}

void RangeImage::loadRangeImage(string fileName)
{
    // (height*width * DIM) => height & width & DIM data for each element (pixel)
    nc::NdArray<float> ncArray = nc::fromfile<float>(fileName);

    for (int i = 0; i < 4; i++)
    {
        _minValue[i] = FLT_MAX;
        _maxValue[i] = FLT_MIN;
    }

    int size = _height * _width;
    for (int i = 0; i < size; ++i)
    {
        int n_index = i * DIM;
        _data[i].x = ncArray[n_index];
        _data[i].y = ncArray[n_index + 1];
        _data[i].z = ncArray[n_index + 2];
        _data[i].remission = ncArray[n_index + 3];
        _data[i].depth = ncArray[n_index + 4];
        _data[i].label = ncArray[n_index + 5];

        _minValue[0] = min(_minValue[0], _data[i].x);
        _minValue[1] = min(_minValue[1], _data[i].y);
        _minValue[2] = min(_minValue[2], _data[i].z);
        _minValue[3] = min(_minValue[3], _data[i].depth);

        _maxValue[0] = max(_maxValue[0], _data[i].x);
        _maxValue[1] = max(_maxValue[1], _data[i].y);
        _maxValue[2] = max(_maxValue[2], _data[i].z);
        _maxValue[3] = max(_maxValue[3], _data[i].depth);
    }
    // cout << "min x : " << _minValue[0] << " | max x : " << _maxValue[0] << endl;
    // cout << "min y : " << _minValue[1] << " | max y : " << _maxValue[2] << endl;
    // cout << "min z : " << _minValue[2] << " | max z : " << _maxValue[2] << endl;
}

vector<uchar> RangeImage::normalizedValue(vector<int> idx)
{
    vector<uchar> normVal;
    normVal.reserve(_width * _height * idx.size());
    vector<float> min(idx.size()), max(idx.size());
    for (size_t i = 0; i < idx.size(); i++)
    {

        if (idx.at(i) != 4)
        {
            min.at(i) = _minValue[idx.at(i)];
            max.at(i) = _maxValue[idx.at(i)];
        }
        else if (idx.at(i) == 4)
        {
            min.at(i) = 0;
            max.at(i) = 1;
        }
        else
        {
            cerr << "invalide index in normalizedValue function" << endl;
            return normVal;
        }
    }
    int size = _width * _height;
    float val, remission;
    for (int i = 0; i < size; i++)
    {
        remission = *((float *)(_data) + i * DIM + 4);
        //cout << i / _width << " |" << i % _width<< endl;

        for (size_t j = 0; j < idx.size(); j++)
        {
            if (remission != -1.f)
            {
                val = *((float *)(_data) + i * DIM + idx.at(j));
                val = roundf((val - min.at(j)) / (max.at(j) - min.at(j)) * 255);
                //val *= remission;
                normVal.push_back((uchar)val);
            }
            else
                normVal.push_back((uchar)0);
        }
    }
    return normVal;
}

void RangeImage::interpolation(vector<uchar> &dataColor, int halfsizeX, int halfsizeY, int nbIter, bool BGR)
{
    int nbChannel = 1;
    if (BGR)
        nbChannel = 3;
    for (int iter = 0; iter < nbIter; iter++)
    {
        for (int i = 0; i < _height; i++)
        {
            for (int j = 0; j < _width; j++)
            {
                if (dataColor.at(i * _width * nbChannel + j * nbChannel) == 0 && 
                (!BGR || (dataColor.at(i * _width * nbChannel + j * nbChannel + 1) == 0 
                && dataColor.at(i * _width * nbChannel + j * nbChannel + 2) == 0)))
                {
                    int sumB = 0;
                    int sumG = 0;
                    int sumR = 0;
                    int sum = 0;
                    for (int y = i - halfsizeY; y <= i + halfsizeY; y++)
                    {
                        for (int x = j - halfsizeX; x <= j + halfsizeX; x++)
                        {
                            if (x < _width && y < _height && x >= 0 && y >= 0)
                            {
                                if (x != j || y != i)
                                {
                                    if (dataColor.at(y * _width * nbChannel + x * nbChannel) != 0 
                                    || (BGR && ( dataColor.at(y * _width * nbChannel + x * nbChannel + 1) != 0 
                                    || dataColor.at(y * _width * nbChannel + x * nbChannel + 2) != 0)))
                                        {
                                            sumB += dataColor.at(y * _width * nbChannel + x * nbChannel);

                                            if (BGR)
                                            {
                                                sumG += dataColor.at(y * _width * nbChannel + x * nbChannel + 1);
                                                sumR += dataColor.at(y * _width * nbChannel + x * nbChannel + 2);
                                            }
                                            sum++;
                                        }
                                }
                            }
                        }
                    }
                    if (sum != 0)
                    {
                        dataColor.at(i * _width * nbChannel + j * nbChannel) = sumB / sum;

                        if (BGR)
                        {
                            dataColor.at(i * _width * nbChannel + j * nbChannel + 1) = sumG / sum;
                            dataColor.at(i * _width * nbChannel + j * nbChannel + 2) = sumR / sum;
                        }
                    }
                }
            }
        }
    }
}

cv::Mat RangeImage::createBGRFromColorMap(int idx, bool interpolate, bool closing)
{
    vector<uchar> dataColor = normalizedValue({idx});
    if (interpolate)
        interpolation(dataColor, 0, 2, 4, false);
    cv::Mat img = createCvMat(dataColor, CV_8UC1);
    cv::Mat img2;
    cv::applyColorMap(img, img2, cv::COLORMAP_HSV);

    if (closing)
        img2 = morphClose(img2);
    return img2;
}

cv::Mat RangeImage::createImageFromXYZ()
{
    vector<int> idx = {0, 1, 2};
    vector<uchar> dataColor = normalizedValue(idx);
    interpolation(dataColor, 0, 2, 4, true);
    cv::Mat img = createCvMat(dataColor);
    cv::Mat img2;
    cv::applyColorMap(img, img2, cv::COLORMAP_HSV);
    //img2 = morphClose(img2);
    return img2;
}

cv::Mat RangeImage::createCvMat(vector<uchar> dataColor, int type)
{

    if (!(type == CV_8UC3 || type == CV_8UC1))
        cerr << "invalide CV_TYPE in createCvMat function" << endl;

    cv::Mat m = cv::Mat(_height, _width, type);

    size_t size = _height * _width;
    if (type == CV_8UC3)
        size *= 3;

    if (dataColor.size() == size)
        memcpy(m.data, dataColor.data(), dataColor.size() * sizeof(uchar));
    else
        cerr << "invalide dataColor in createCvMat function" << endl;

    if (type == CV_8UC1)
    {
        cv::Mat tmp = cv::Mat(_height, _width, CV_8UC3);
        cv::cvtColor(m, tmp, CV_GRAY2BGR);
        m = tmp;
    }
    return m;
}

cv::Mat RangeImage::morphClose(cv::Mat img)
{
    // Test : Apply dilation
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(1, 3));
    cv::Mat img_close;
    cv::morphologyEx(img, img_close, cv::MORPH_CLOSE, element);
    return img_close;
}

cv::Mat RangeImage::morphDilate(cv::Mat img)
{
    // Test : Apply dilation
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(1, 3));
    cv::Mat img_dilate;
    cv::dilate(img, img_dilate, element);
    return img_dilate;
}

const riVertex *RangeImage::getData()
{
    return _data;
}
