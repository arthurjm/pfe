#include "rangeimage.h"
#include <assert.h>

using namespace std;

RangeImage::RangeImage(string fileName, int width, int height)
    : _data(nullptr), _width(width), _height(height)
{
    _data = (riVertex *)malloc(sizeof(riVertex) * width * height);
    assert(_data);

    for (int i = 0; i < _height * _width; ++i)
    {
        _data[i].x = -1;
        _data[i].y = -1;
        _data[i].z = -1;
        _data[i].remission = -1;
        _data[i].depth = -1;
        _data[i].label = -1;
    }

    vector<float> x;
    vector<float> y;
    vector<float> z;
    vector<float> remission;
    fstream file(fileName.c_str(), ios::in | ios::binary);

    if (file.good())
    {
        file.seekg(0, std::ios::beg);
        int i;
        float tmp;
        for (i = 0; file.good() && !file.eof(); i++)
        {
            if (i != 0)
            {
                file.read((char *)&tmp, sizeof(float));
                x.push_back(tmp);

                file.read((char *)&tmp, sizeof(float));
                y.push_back(tmp);

                file.read((char *)&tmp, sizeof(float));
                z.push_back(tmp);

                file.read((char *)&tmp, sizeof(float));
                remission.push_back(tmp);
            }
        }
        file.close();
    }

    for (int i = 0; i < 4; i++)
    {
        _minValue[i] = FLT_MAX;
        _maxValue[i] = FLT_MIN;
    }

    pointCloudProjection(x, y, z, remission, FOV_UP, FOV_DOWN);
    separateInvalideComposant();
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

void RangeImage::pointCloudProjection(vector<float> scan_x, vector<float> scan_y,
                                      vector<float> scan_z, vector<float> scan_remission,
                                      float proj_fov_up, float proj_fov_down)
{
    float fov_up = proj_fov_up / 180.0 * M_PI;     // field of view up in rad
    float fov_down = proj_fov_down / 180.0 * M_PI; // field of view down in rad
    float fov = abs(fov_down) + abs(fov_up);       // get field of view total in rad

    vector<float> depth_v;
    vector<uint32_t> indices_v;
    vector<uint32_t> proj_x_v;
    vector<uint32_t> proj_y_v;

    depth_v.reserve(scan_x.size());
    indices_v.reserve(scan_x.size());
    proj_x_v.reserve(scan_x.size());
    proj_y_v.reserve(scan_x.size());

    for (int i = 0; i < scan_remission.size(); ++i)
    {
        indices_v.push_back(i);
        float x = scan_x.at(i);
        float y = scan_y.at(i);
        float z = scan_z.at(i);

        float depth = sqrt(x * x + y * y + z * z);
        depth_v.push_back(depth);

        float yaw = -atan2(y, x);
        float pitch = asin(z / depth);

        float proj_x = 0.5 * (yaw / M_PI + 1.0);            // in [0.0, 1.0]
        float proj_y = 1.0 - (pitch + abs(fov_down)) / fov; // in [0.0, 1.0]

        proj_x *= _width;  // in [0.0, W]
        proj_y *= _height; // in [0.0, H]

        uint32_t proj_x_uint = (uint32_t)floor(proj_x);
        proj_x_uint = min((uint32_t)_width - 1, proj_x_uint);
        proj_x_uint = max((uint32_t)0, proj_x_uint);

        uint32_t proj_y_uint = (uint32_t)floor(proj_y);
        proj_y_uint = min((uint32_t)_height - 1, proj_y_uint);
        proj_y_uint = max((uint32_t)0, proj_y_uint);

        proj_x_v.push_back(proj_x_uint);
        proj_y_v.push_back(proj_y_uint);
    }

    for (int i = 0; i < indices_v.size(); ++i)
    {
        int idx = _width * proj_y_v.at(i) + proj_x_v.at(i);

        if (_data[idx].depth >= depth_v.at(i) || _data[idx].depth < 0)
        {
            _data[idx].x = scan_x.at(i);
            _data[idx].y = scan_y.at(i);
            _data[idx].z = scan_z.at(i);
            _data[idx].remission = scan_remission.at(i);
            _data[idx].depth = depth_v.at(i);
            _data[idx].label = -1;

            _minValue[0] = min(_minValue[0], _data[idx].x);
            _minValue[1] = min(_minValue[1], _data[idx].y);
            _minValue[2] = min(_minValue[2], _data[idx].z);
            _minValue[3] = min(_minValue[3], _data[idx].depth);

            _maxValue[0] = max(_maxValue[0], _data[idx].x);
            _maxValue[1] = max(_maxValue[1], _data[idx].y);
            _maxValue[2] = max(_maxValue[2], _data[idx].z);
            _maxValue[3] = max(_maxValue[3], _data[idx].depth);
        }
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
                normVal.push_back((uchar)val);
            }
            else
                normVal.push_back((uchar)0);
        }
    }
    return normVal;
}

void RangeImage::interpolation(vector<uchar> &dataColor, int halfsizeX, int halfsizeY, bool BGR)
{
    int nbChannel = 1;
    if (BGR)
        nbChannel = 3;
    for (int i = 0; i < _height; i++)
    {
        for (int j = 0; j < _width; j++)
        {
            bool validLabel = _data[i * _width + j].label == -1;
            bool validRemission = _data[i * _width + j].remission == -1;
            if (validLabel && validRemission)
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
                                if (_data[y * _width + x].remission != -1)
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

cv::Mat RangeImage::createColorMat(vector<int> idx, bool isGray, bool interpolate, bool closing, bool equalHist)
{
    vector<uchar> dataColor = normalizedValue(idx);

    bool isBGR = false;
    int cvType = CV_8UC1;
    if (idx.size() == 3)
    {
        cvType = CV_8UC3;
        isBGR = true;
    }

    if (interpolate)
        interpolation(dataColor, 0, 2, isBGR);

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

cv::Mat RangeImage::createImageFromXYZ()
{
    vector<uchar> dataX = normalizedValue({RI_X});
    vector<uchar> dataY = normalizedValue({RI_Y});
    vector<uchar> dataZ = normalizedValue({RI_Z});

    cv::Mat imgX = createCvMat(dataX, CV_8UC1);
    cv::Mat imgY = createCvMat(dataY, CV_8UC1);
    cv::Mat imgZ = createCvMat(dataZ, CV_8UC1);

    cv::cvtColor(imgX, imgX, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imgY, imgY, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imgZ, imgZ, cv::COLOR_BGR2GRAY);

    cv::Mat tmp;
    cv::equalizeHist(imgX, tmp);
    imgX = tmp;
    cv::equalizeHist(imgY, tmp);
    imgY = tmp;
    cv::equalizeHist(imgZ, tmp);
    imgZ = tmp;

    vector<uchar> dataColor;
    int size = _width * _height;
    dataColor.reserve(size * 3);
    for (int i = 0; i < size; ++i)
    {
        float re = _data[i].remission != -1 ? _data[i].remission : 1;
        dataColor.push_back(imgX.data[i] * re);
        dataColor.push_back(imgY.data[i] * re);
        dataColor.push_back(imgZ.data[i] * re);
    }
    interpolation(dataColor, 0, 2, false);
    cv::Mat img = createCvMat(dataColor);
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