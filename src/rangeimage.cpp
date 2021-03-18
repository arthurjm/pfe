#include "rangeimage.h"
#include <assert.h>

using namespace std;

RangeImage::~RangeImage()
{
}

void RangeImage::freeData()
{
    if (_data)
    {
        free(_data);
        _data = nullptr;
    }
}

RangeImage::RangeImage(RangeImage &ri) : _data(ri._data), _width(ri._width), _height(ri._height)
{
    //     _data = (riVertex *)malloc(sizeof(riVertex) * _width * _height);
    //     for (int i = 0; i < _width * _height; i++)
    //     {
    //         _data[i] = ri._data[i];
    //     }
    for (int i = 0; i < 4; i++)
    {
        _minValue[i] = ri._minValue[i];
        _maxValue[i] = ri._maxValue[i];
    }
    _normalizedAndInterpolatedData = ri._normalizedAndInterpolatedData;
}

RangeImage::RangeImage(RangeImage &&ri) : _width(ri._width), _height(ri._height)
{
    _data = move(ri._data);
    swap(_minValue, ri._minValue);
    swap(_maxValue, ri._maxValue);
    swap(_normalizedAndInterpolatedData, ri._normalizedAndInterpolatedData);
}

RangeImage::RangeImage(riVertex *data, int width, int height)
    : _data(data), _width(width), _height(height)
{
    assert(_data);
    if (_width <= 0 || _height <= 0)
    {
        cerr << "invalid width or height in RangeImage constructor" << endl;
        exit(EXIT_FAILURE);
    }
    // set the minimum and the maximum attributes
    for (int i = 0; i < 4; i++)
    {
        _minValue[i] = FLT_MAX;
        _maxValue[i] = FLT_MIN;
    }

    for (int i = 0; i < _width * _height; i++)
    {
        if (_data[i].remission != -1)
        {
            _minValue[0] = min(_minValue[RI_X], _data[i].x);
            _minValue[1] = min(_minValue[RI_Y], _data[i].y);
            _minValue[2] = min(_minValue[RI_Z], _data[i].z);
            _minValue[3] = min(_minValue[RI_DEPTH], _data[i].depth);

            _maxValue[0] = max(_maxValue[RI_X], _data[i].x);
            _maxValue[1] = max(_maxValue[RI_Y], _data[i].y);
            _maxValue[2] = max(_maxValue[RI_Z], _data[i].z);
            _maxValue[3] = max(_maxValue[RI_DEPTH], _data[i].depth);
        }
    }

    separateInvalidComposant();
    // set the normalized and interpolated raw data attribute
    vector<int> component = {RI_X, RI_Y, RI_Z, RI_REMISSION};
    _normalizedAndInterpolatedData = normalizedValue(component);
    interpolation(_normalizedAndInterpolatedData, RI_INTERPOLATE_HS_X, RI_INTERPOLATE_HS_Y, component.size());
}

void RangeImage::separateInvalidComposant()
{
    assert(_data);
    int size = _height * _width;
    if (size <= 0)
    {
        cerr << "invalid size in RangeImage::separateInvalidComposant" << endl;
        exit(EXIT_FAILURE);
    }
    // convert to bichrome image
    vector<uchar> tmpImg;
    tmpImg.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        if (_data[i].remission == -1)
            tmpImg.push_back(255);
        else
            tmpImg.push_back(0);
    }
    // apply morphologie to valid component
    cv::Mat m = morphOpen(createCvMat(tmpImg, CV_8UC1));
    cv::cvtColor(m, m, cv::COLOR_BGR2GRAY);
    uchar *mask = m.data;
    // assigne -2 to invalid component's label
    for (int i = 0; i < size; ++i)
    {
        if (mask[i] == 255)
            _data[i].label = -2;
    }
}

RangeImage &RangeImage::operator=(const RangeImage &ri)
{
    _width = ri._width;
    _height = ri._height;
    // _data = (riVertex *)malloc(sizeof(riVertex) * _width * _height);
    // for (int i = 0; i < _width * _height; i++)
    // {
    //     _data[i] = ri._data[i];
    // }
    _data = ri._data;
    for (int i = 0; i < 4; i++)
    {
        _minValue[i] = ri._minValue[i];
        _maxValue[i] = ri._maxValue[i];
    }
    _normalizedAndInterpolatedData = ri._normalizedAndInterpolatedData;
    return *this;
}

RangeImage &RangeImage::operator=(RangeImage &&ri)
{
    _width = ri._width;
    _height = ri._height;
    _data = move(ri._data);
    swap(_minValue, ri._minValue);
    swap(_maxValue, ri._maxValue);
    swap(_normalizedAndInterpolatedData, ri._normalizedAndInterpolatedData);
    return *this;
}

vector<float> RangeImage::normalizedValue(vector<int> idx)
{
    if (_data == nullptr)
    {
        cerr << "invalid _data in RangeImage::normalizedValue" << endl;
        exit(EXIT_FAILURE);
    }
    int size = _width * _height;
    if (size <= 0)
    {
        cerr << "invalid size in RangeImage::normalizedValue" << endl;
        exit(EXIT_FAILURE);
    }
    if ((int)idx.size() == 0)
    {
        cerr << "invalid idx vector size in RangeImage::normalizedValue" << endl;
        exit(EXIT_FAILURE);
    }
    vector<float> normVal;
    normVal.reserve(size * idx.size());
    vector<float> min(idx.size()), max(idx.size());
    for (size_t i = 0; i < idx.size(); i++)
    {

        if (idx.at(i) < RI_REMISSION && idx.at(i) >= RI_X)
        {
            min.at(i) = _minValue[idx.at(i)];
            max.at(i) = _maxValue[idx.at(i)];
        }
        else if (idx.at(i) == RI_REMISSION)
        {
            min.at(i) = 0;
            max.at(i) = 1;
        }
        else
        {
            cerr << "invalide index in RangeImage::normalizedValue function" << endl;
            exit(EXIT_FAILURE);
        }
    }
    float val, remission;
    for (int i = 0; i < size; i++)
    {
        remission = *((float *)(_data) + i * DIM + RI_REMISSION);
        for (size_t j = 0; j < idx.size(); j++)
        {
            if (remission != -1.f) // verification of valid pixel
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
    assert(_data);
    if (pixelIndex < 0 || pixelIndex >= _width * _height)
    {
        cerr << "invalid pixelIndex in RangeImage::getNormalizedValue, it must be in range of 0 and the image size" << endl;
        exit(EXIT_FAILURE);
    }
    riVertex riv;

    riv.remission = *((float *)(_data) + pixelIndex * DIM + RI_REMISSION);
    if (riv.remission != -1) // verification of valid pixel
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
    if ((int)data.size() != _width * _height * nbComponent)
    {
        cerr << "invalid data in RangeImage::interpolation" << endl;
        exit(EXIT_FAILURE);
    }
    if (halfsizeX < 0 || halfsizeY < 0)
    {
        cerr << "invalid halfsize in RangeImage::interpolation, can't be negative" << endl;
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
                            if (x != j || y != i) // not itself
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
    size_t size = normalizedData.size();
    // check type for the cv::Mat
    int nbComponent = 1;
    int cvType = CV_8UC1;
    if (idx.size() == 3)
    {
        cvType = CV_8UC3;
        nbComponent = 3;
    }
    else if (idx.size() != 1)
    {
        cerr << "invalid idx vector size in RangeImage::createColorMat" << endl;
        exit(EXIT_FAILURE);
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

cv::Mat RangeImage::createCvMat(vector<uchar> data, int type)
{
    size_t size = _height * _width;
    if (size <= 0)
    {
        cerr << "invalid size in RangeImage::createCvMat" << endl;
        exit(EXIT_FAILURE);
    }
    if (!(type == CV_8UC3 || type == CV_8UC1))
    {
        cerr << "invalide CV_TYPE in createCvMat function" << endl;
        exit(EXIT_FAILURE);
    }

    cv::Mat m = cv::Mat(_height, _width, type);

    if (type == CV_8UC3)
        size *= 3;

    if (data.size() == size)
        memcpy(m.data, data.data(), data.size() * sizeof(uchar));
    else
    {
        cerr << "invalide data vector in createCvMat function" << endl;
        exit(EXIT_FAILURE);
    }

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
    if (img.rows == 0 || img.cols == 0)
    {
        cerr << "invalid cv:::Mat in RangeImage::morphClose" << endl;
        exit(EXIT_FAILURE);
    }
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(1, 3));
    cv::Mat img_close;
    cv::morphologyEx(img, img_close, cv::MORPH_CLOSE, element);
    return img_close;
}

cv::Mat RangeImage::morphOpen(cv::Mat img)
{
    if (img.rows == 0 || img.cols == 0)
    {
        cerr << "invalid cv:::Mat in RangeImage::morphOpen" << endl;
        exit(EXIT_FAILURE);
    }
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(1, 3));
    cv::Mat img_open;
    cv::morphologyEx(img, img_open, cv::MORPH_OPEN, element);
    return img_open;
}

cv::Mat RangeImage::morphDilate(cv::Mat img)
{
    if (img.rows == 0 || img.cols == 0)
    {
        cerr << "invalid cv:::Mat in RangeImage::morphDilate" << endl;
        exit(EXIT_FAILURE);
    }
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(1, 3));
    cv::Mat img_dilate;
    cv::dilate(img, img_dilate, element);
    return img_dilate;
}

cv::Mat RangeImage::morphErode(cv::Mat img)
{
    if (img.rows == 0 || img.cols == 0)
    {
        cerr << "invalid cv:::Mat in RangeImage::morphErode" << endl;
        exit(EXIT_FAILURE);
    }
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

const vector<float> *RangeImage::getNormalizedAndInterpolatedData()
{
    return &_normalizedAndInterpolatedData;
}

cv::Mat RangeImage::getRawDataFromIndex(int index)
{
    if (index < 0)
    {
        cerr << "invalid index in RangeImage::getRawDataFromIndex" << endl;
        exit(EXIT_FAILURE);
    }
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
    assert(_data);
    if (index < 0)
    {
        cerr << "invalid index in RangeImage::setLabel" << endl;
        exit(EXIT_FAILURE);
    }
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
