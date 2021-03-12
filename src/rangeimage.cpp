#include "rangeimage.h"
#include <assert.h>

using namespace std;

RangeImage::RangeImage(riVertex *data, int width, int height)
    : _data(data), _width(width), _height(height)
{

    for (int i = 0; i < 4; i++)
    {
        _minValue[i] = FLT_MAX;
        _maxValue[i] = FLT_MIN;
    }

    for (int i = 0; i < _width * _height; i++)
    {
        _minValue[0] = min(_minValue[0], _data[i].x);
        _minValue[1] = min(_minValue[1], _data[i].y);
        _minValue[2] = min(_minValue[2], _data[i].z);
        _minValue[3] = min(_minValue[3], _data[i].depth);

        _maxValue[0] = max(_maxValue[0], _data[i].x);
        _maxValue[1] = max(_maxValue[1], _data[i].y);
        _maxValue[2] = max(_maxValue[2], _data[i].z);
        _maxValue[3] = max(_maxValue[3], _data[i].depth);
    }

    separateInvalideComposant();
    vector<int> component = {RI_X, RI_Y, RI_Z, RI_REMISSION};
    _normalizedData = normalizedValue(component);
    interpolation(_normalizedData, RI_INTERPOLATE_HS_X, RI_INTERPOLATE_HS_Y, component.size());
}

void RangeImage::separateInvalideComposant()
{
    int size = _height * _width;
    vector<uchar> tmpImg;
    tmpImg.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        if (_data[i].remission == -1)
            tmpImg.push_back(255);
        else
            tmpImg.push_back(0);
    }
    cv::Mat m = morphOpen(createCvMat(tmpImg, CV_8UC1));
    cv::cvtColor(m, m, cv::COLOR_BGR2GRAY);
    uchar *mask = m.data;
    for (int i = 0; i < size; ++i)
    {
        if (mask[i] == 255)
            _data[i].label = -2;
    }
}

vector<float> RangeImage::normalizedValue(vector<int> idx)
{
    vector<float> normVal;
    normVal.reserve(_width * _height * idx.size());
    vector<float> min(idx.size()), max(idx.size());
    for (size_t i = 0; i < idx.size(); i++)
    {

        if ((idx.at(i) < 4 && idx.at(i) >= 0) || idx.at(i) == RI_XYZ)
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
                val = (val - min.at(j)) / (max.at(j) - min.at(j));
                normVal.push_back(val);
            }
            else
                normVal.push_back(0);
        }
    }
    return normVal;
}

riVertex RangeImage::getNormalizedValue(int pixelIndex)
{
    riVertex riv;

    riv.remission = *((float *)(_data) + pixelIndex * DIM + RI_REMISSION);
    if (riv.remission != -1)
    {
        float val = *((float *)(_data) + pixelIndex * DIM + RI_X);
        val = (val - _minValue[RI_X]) / (_maxValue[RI_X] - _minValue[RI_X]);
        riv.x = val;

        val = *((float *)(_data) + pixelIndex * DIM + RI_Y);
        val = (val - _minValue[RI_Y]) / (_maxValue[RI_Y] - _minValue[RI_Y]);
        riv.y = val;

        val = *((float *)(_data) + pixelIndex * DIM + RI_Z);
        val = (val - _minValue[RI_Z]) / (_maxValue[RI_Z] - _minValue[RI_Z]);
        riv.z = val;

        val = *((float *)(_data) + pixelIndex * DIM + RI_DEPTH);
        val = (val - _minValue[RI_DEPTH]) / (_maxValue[RI_DEPTH] - _minValue[RI_DEPTH]);
        riv.depth = val;
    }
    else
    {
        riv.x = 0;
        riv.y = 0;
        riv.z = 0;
        riv.depth = 0;
        riv.remission = 0;
    }
    return riv;
}

void RangeImage::interpolation(vector<float> &data, int halfsizeX, int halfsizeY, int nbComponent)
{

    if (nbComponent != 1 && nbComponent != 3 && nbComponent != 4)
    {
        cerr << "invalid number of component in RangeImage::interpolation" << endl;
        exit(EXIT_FAILURE);
    }

    float *componentSum = (float *)calloc(nbComponent, sizeof(float));
    for (int i = 0; i < _height; i++)
    {
        for (int j = 0; j < _width; j++)
        {
            bool validLabel = _data[i * _width + j].label == -1;
            bool validRemission = _data[i * _width + j].remission == -1;
            if (validLabel && validRemission)
            {
                for (int p = 0; p < nbComponent; ++p)
                    componentSum[p] = 0;
                int pixelCount = 0;
                for (int y = i - halfsizeY; y <= i + halfsizeY; y++)
                {
                    for (int x = j - halfsizeX; x <= j + halfsizeX; x++)
                    {
                        if (x < _width && y < _height && x >= 0 && y >= 0) // inside the image
                        {
                            if (x != j || y != i) // not himself
                            {
                                if (_data[y * _width + x].remission != -1) // is a "dead" pixel
                                {
                                    for (int k = 0; k < nbComponent; k++)
                                    {
                                        componentSum[k] += data.at(y * _width * nbComponent + x * nbComponent + k);
                                    }
                                    pixelCount++;
                                }
                            }
                        }
                    }
                }

                if (pixelCount != 0)
                {
                    for (int k = 0; k < nbComponent; k++)
                    {
                        data.at(i * _width * nbComponent + j * nbComponent + k) = componentSum[k] / pixelCount;
                    }
                }
            }
        }
    }
    free(componentSum);
}

cv::Mat RangeImage::createColorMat(vector<int> idx, bool isGray, bool interpolate, bool closing, bool equalHist)
{
    vector<float> normalizedData = normalizedValue(idx);
    // convert to range between 0 and 255;
    size_t size = normalizedData.size();

    int nbComponent = 1;
    int cvType = CV_8UC1;
    if (idx.size() == 3)
    {
        cvType = CV_8UC3;
        nbComponent = 3;
    }

    if (interpolate)
        interpolation(normalizedData, RI_INTERPOLATE_HS_X, RI_INTERPOLATE_HS_Y, nbComponent);

    vector<uchar> dataColor;
    dataColor.reserve(size);
    for (size_t i = 0; i < size; i++)
    {
        dataColor.push_back((uchar)(normalizedData.at(i) * 255));
    }

    cv::Mat img = createCvMat(dataColor, cvType);

    if (equalHist)
    {
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        cv::Mat tmp;
        cv::equalizeHist(img, tmp);
        img = tmp;
        cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
    }

    if (isGray)
    {
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
    }
    else
    {
        cv::Mat img2;
        cv::applyColorMap(img, img2, cv::COLORMAP_JET);
        img = img2;
    }

    if (closing)
        img = morphClose(img);
    return img;
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
        cv::cvtColor(m, tmp, cv::COLOR_GRAY2BGR);
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

cv::Mat RangeImage::morphOpen(cv::Mat img)
{
    // Test : Apply dilation
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(1, 3));
    cv::Mat img_open;
    cv::morphologyEx(img, img_open, cv::MORPH_OPEN, element);
    return img_open;
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

cv::Mat RangeImage::morphErode(cv::Mat img)
{
    // Test : Apply dilation
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(1, 5));
    cv::Mat img_erode;
    cv::erode(img, img_erode, element);
    return img_erode;
}

const riVertex *RangeImage::getData()
{
    return _data;
}

const vector<float> *RangeImage::getNormalizedData()
{
    return &_normalizedData;
}

cv::Mat RangeImage::getRawDataFromIndex(int index)
{
    int size = _height * _width;
    vector<float> rawData;
    rawData.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        float val = *((float *)(_data) + i * DIM + index);
        rawData.push_back(val);
    }
    cv::Mat m = cv::Mat(_height, _width, CV_32FC1);
    memcpy(m.data, rawData.data(), size * sizeof(float));
    return m;
}

void RangeImage::setLabel(int index, int label)
{
    _data[index].label = label;
}

int RangeImage::getHeight()
{
    return _height;
}

int RangeImage::getWidth()
{
    return _width;
}

void RangeImage::save(string filename)
{
    int size = _height * _width;
    nc::NdArray<float> riNdArr(1, size * DIM);
    for (int i = 0; i < size; ++i)
    {
        int n_index = i * DIM;
        riNdArr[n_index + 0] = _data[i].x;
        riNdArr[n_index + 1] = _data[i].y;
        riNdArr[n_index + 2] = _data[i].z;
        riNdArr[n_index + 3] = _data[i].remission;
        riNdArr[n_index + 4] = _data[i].depth;
        riNdArr[n_index + 5] = _data[i].label;
    }
    riNdArr.tofile(filename);
}