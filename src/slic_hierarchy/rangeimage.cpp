#include <assert.h>

#include "rangeimage.h"

using namespace std;

RangeImage::RangeImage(string fileName, int height, int width)
    : _data(nullptr), _height(height), _width(width)
{
    _data = (riVertex *)malloc(sizeof(riVertex) * width * height);
    assert(_data);
    loadRangeImage(fileName);
}

RangeImage::RangeImage(CloudPoints cp, int height, int width, float proj_fov_up, float proj_fov_down)
    : _data(nullptr), _height(height), _width(width)
{
    _data = (riVertex *)malloc(sizeof(riVertex) * width * height);
    assert(_data);

    // laser parameters
    float fov_up = proj_fov_up / 180.0 * M_PI;     // field of view up in rad
    float fov_down = proj_fov_down / 180.0 * M_PI; // field of view down in rad
    float fov = abs(fov_down) + abs(fov_up);       // get field of view total in rad
    std::cout << "points:" << cp.getPoints().shape() << std::endl;
    auto points = cp.getPoints();

    // get depth of all points
    nc::NdArray<double> d_depth = nc::norm(cp.getPoints(), nc::Axis::COL);
    nc::NdArray<float> depth = d_depth.astype<float>().transpose();
    std::cout << "depth:" << depth.shape() << std::endl;

    // get scan components
    nc::NdArray<float> scan_x = points(points.rSlice(), 0);
    nc::NdArray<float> scan_y = points(points.rSlice(), 1);
    nc::NdArray<float> scan_z = points(points.rSlice(), 2);
    std::cout << "scan x:" << scan_x.shape() << "scan y:" << scan_y.shape() << "scan z:" << scan_z.shape() << std::endl;

    // get angles of all points
    nc::NdArray<float> yaw = -nc::arctan2(scan_y, scan_x);
    nc::NdArray<float> pitch = nc::arcsin(scan_z / depth);
    std::cout << "yaw:" << yaw.shape() << "pitch:" << pitch.shape() << std::endl;

    // get projections in image coords
    auto proj_x = (float)0.5 * (yaw / (float)(M_PI + 1.0));   // in [0.0, 1.0]
    auto proj_y = (float)1.0 - (pitch + abs(fov_down)) / fov; // in [0.0, 1.0]

    // scale to image size using angular resolution
    proj_x *= (float)width;  // in [0.0, W]
    proj_y *= (float)height; // in [0.0, H]

    // round and clamp for use as index
    proj_x = nc::floor(proj_x);
    nc::NdArray<float> width_1 = proj_x.copy();
    width_1.fill((float)(width - 1));
    nc::NdArray<float> zeroes = proj_x.copy();
    zeroes.zeros();
    proj_x = nc::minimum(width_1, proj_x);
    proj_x = nc::maximum(zeroes, proj_x);
    auto x = proj_x.astype<int>(); // in [0, W-1]

    std::cout << "width_1:" << width_1.shape() << "proj_x:" << proj_x.shape() << std::endl;

    proj_y = nc::floor(proj_y);
    nc::NdArray<float> height_1 = nc::full(proj_y.shape().rows, proj_y.shape().cols, (float)(height - 1));
    proj_y = nc::minimum(height_1, proj_y);
    proj_y = nc::maximum(zeroes, proj_y);
    auto y = proj_y.astype<int>(); // in [0,H-1]

    auto indices = nc::arange<int>(depth.shape().rows);
    auto order = nc::argsort(depth, nc::Axis::ROW); // validé
    order = nc::flip(order, nc::Axis::ROW);         // validé
    std::cout << "order:" << order.shape() << std::endl;

    auto depth2 = depth.copy();
    for (int i = 0; i < depth.size(); i++)
    {
        depth2[i] = depth[order[i]];
        // ... à faire pour les autres //
    }
    // array = array2;
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
// void RangeImage::projectionPC(){

// }