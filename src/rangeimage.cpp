#include "rangeimage.h"
#include <assert.h>

using namespace std;

RangeImage::RangeImage(string fileName, int mode, int width, int height)
    : _data(nullptr), _width(width), _height(height)
{

    _data = (riVertex *)malloc(sizeof(riVertex) * width * height);
    assert(_data);

    if (mode == 0)
        loadRangeImage(fileName);
    if (mode == 1)
    {
        nc::NdArray<float> scan = nc::fromfile<float>(fileName, "");
        scan.reshape(-1, 4);
        nc::NdArray<float> points = scan(scan.rSlice(), nc::Slice(0, 3));
        nc::NdArray<float> remissions = scan(scan.rSlice(), 3);

        for (int i = 0; i < _height * _width; ++i)
        {
            _data[i].x = -1;
            _data[i].y = -1;
            _data[i].z = -1;
            _data[i].remission = -1;
            _data[i].depth = -1;
            _data[i].label = -1;
        }
        pointCloudProjection(points, remissions, FOV_UP, FOV_DOWN);
    }
}

RangeImage::RangeImage(string pc, string labelFile, int height, int width) : _data(nullptr), _height(height), _width(width)
{
    nc::NdArray<float> scan = nc::fromfile<float>(pc, "");
    scan.reshape(-1, 4);
    nc::NdArray<float> points = scan(scan.rSlice(), nc::Slice(0, 3));
    nc::NdArray<float> remissions = scan(scan.rSlice(), 3);

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
    pointCloudProjection(points, remissions, FOV_UP, FOV_DOWN);
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

void RangeImage::pointCloudProjection(const nc::NdArray<float> points, const nc::NdArray<float> remissions, float proj_fov_up, float proj_fov_down)

{
    std::cout << "pointCloudProjection start" << std::endl;
    // laser parameters
    float fov_up = proj_fov_up / 180.0 * M_PI;     // field of view up in rad
    float fov_down = proj_fov_down / 180.0 * M_PI; // field of view down in rad
    float fov = abs(fov_down) + abs(fov_up);       // get field of view total in rad

    // get depth of all points
    nc::NdArray<double> d_depth = nc::norm(points, nc::Axis::COL);
    nc::NdArray<float> depth = d_depth.astype<float>().transpose();

    // get scan components
    nc::NdArray<float> scan_x = points(points.rSlice(), 0);
    nc::NdArray<float> scan_y = points(points.rSlice(), 1);
    nc::NdArray<float> scan_z = points(points.rSlice(), 2);

    // get angles of all points
    nc::NdArray<float> yaw = -nc::arctan2(scan_y, scan_x);
    nc::NdArray<float> pitch = nc::arcsin(scan_z / depth);

    // get projections in image coords
    nc::NdArray<float> proj_x = (float)0.5 * (yaw / (float)(nc::constants::pi) + (float)1.0); // in [0.0, 1.0]
    nc::NdArray<float> proj_y = (float)1.0 - (pitch + abs(fov_down)) / (float)fov;            // in [0.0, 1.0]

    // scale to image size using angular resolution
    proj_x *= (float)_width;  // in [0.0, W]
    proj_y *= (float)_height; // in [0.0, H]

    // create zero array for comparison
    nc::NdArray<float> zeroes_x = nc::empty_like(proj_x);
    nc::NdArray<float> zeroes_y = nc::empty_like(proj_y);
    zeroes_x.zeros();
    zeroes_y.zeros();

    // round and clamp for use as index x
    proj_x = nc::floor(proj_x);
    nc::NdArray<float> width_1 = nc::empty_like(proj_x);
    width_1.fill((float)(_width - 1));
    proj_x = nc::minimum(width_1, proj_x);
    proj_x = nc::maximum(zeroes_x, proj_x);
    nc::NdArray<uint32_t> x = proj_x.astype<uint32_t>(); // in [0, W-1]

    // round and clamp for use as index y
    proj_y = nc::floor(proj_y);
    nc::NdArray<float> height_1 = nc::full(proj_y.shape().rows, proj_y.shape().cols, (float)((float)_height - 1));
    proj_y = nc::minimum(height_1, proj_y);
    proj_y = nc::maximum(zeroes_y, proj_y);
    nc::NdArray<uint32_t> y = proj_y.astype<uint32_t>(); // in [0,H-1]

    //order in decreasing depth
    nc::NdArray<uint32_t> indices = nc::arange<uint32_t>(depth.shape().rows);
    nc::NdArray<uint32_t> order = nc::argsort(depth, nc::Axis::ROW);
    order = nc::flip(order, nc::Axis::ROW);

    nc::NdArray<float> depth_sorted = nc::empty_like(depth);
    nc::NdArray<uint32_t> indices_sorted = nc::empty_like(indices);
    nc::NdArray<float> points_sorted = nc::empty_like(points);
    nc::NdArray<float> remissions_sorted = nc::empty_like(remissions);
    nc::NdArray<uint32_t> proj_x_sorted = nc::empty_like(x);
    nc::NdArray<uint32_t> proj_y_sorted = nc::empty_like(y);

    nc::NdArray<float> points_x = points(points.rSlice(), 0);
    nc::NdArray<float> points_y = points(points.rSlice(), 1);
    nc::NdArray<float> points_z = points(points.rSlice(), 2);

    nc::NdArray<float> points_x_sorted = nc::empty_like(points_x);
    nc::NdArray<float> points_y_sorted = nc::empty_like(points_y);
    nc::NdArray<float> points_z_sorted = nc::empty_like(points_z);
    if (_labels.size() == 0)
    {
        _labels.assign(points.size(), -1);
    }
    nc::NdArray<uint16_t> label_sorted = nc::empty<uint16_t>((int)_labels.size(), 1);

    for (uint i = 0; i < depth.size(); i++)
    {
        int idx = order[i];
        proj_x_sorted[i] = proj_x[idx];
        proj_y_sorted[i] = proj_y[idx];
        // indices_sorted[i] = indices[idx];
        points_x_sorted[i] = points_x[idx];
        points_y_sorted[i] = points_y[idx];
        points_z_sorted[i] = points_z[idx];
        label_sorted[i] = _labels[idx];
        // if (label_sorted[i] == 1){
        // std::cout << "i: " << i << " idx: " << idx << std::endl;
        // std::cout << "sorted: " << label_sorted[i] << " label: " << _labels[idx] << std::endl;
        // }
        remissions_sorted[i] = remissions[idx];
        depth_sorted[i] = depth[idx];
    }

    // assign to images
    for (uint i = 0; i < depth.size(); i++)
    {
        int idx = _width * proj_y_sorted[i] + proj_x_sorted[i];

        _data[idx].x = points_x_sorted[i];
        _data[idx].y = points_y_sorted[i];
        _data[idx].z = points_z_sorted[i];
        _data[idx].remission = remissions_sorted[i];
        _data[idx].depth = depth_sorted[i];
        // if(label_sorted[i] == 1)
        //     std::cout << "idx: " << idx << " label: " << label_sorted[i] << std::endl;
        _data[idx].label = label_sorted[i];
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