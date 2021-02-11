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
    }
}