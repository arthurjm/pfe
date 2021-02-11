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

        for (size_t j = 0; j < idx.size(); j++)
        {
            if (remission != -1.f)
            {
                val = *((float *)(_data) + i * DIM + idx.at(j));
                val = roundf((val - min.at(j)) / (max.at(j) - min.at(j)) * 255);
                val *= remission;
                normVal.push_back((uchar)val);
            }
            else
                normVal.push_back((uchar)0);
        }
    }
    return normVal;
}

cv::Mat RangeImage::createMatFromXYZ()
{
    vector<int> idx = {0, 1, 2};
    vector<uchar> dataBGR = normalizedValue(idx);
    return createCvMat(dataBGR);
}

cv::Mat RangeImage::createCvMat(vector<uchar> dataBGR)
{
    cv::Mat m = cv::Mat(_height, _width, CV_8UC3); // initialize matrix of uchar of 3-channel where you will store vec data
    size_t size = _height * _width * 3;
    if (dataBGR.size() == size)
        memcpy(m.data, dataBGR.data(), dataBGR.size() * sizeof(uchar));
    else
        cerr << "invalide dataBGR in createCvMat function" << endl;

    return m;
}