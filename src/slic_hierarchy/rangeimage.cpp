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

RangeImage::RangeImage(PointCloud cp, int height, int width, float proj_fov_up, float proj_fov_down)
    : _data(nullptr), _height(height), _width(width)
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
    pointCloudProjection(cp,proj_fov_up,proj_fov_down);
}

void RangeImage::loadRangeImage(string fileName)
{
    // (height*width * DIM) => height & width & DIM data for each element (pixel)
    nc::NdArray<_Float32> ncArray = nc::fromfile<_Float32>(fileName);

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
void RangeImage::pointCloudProjection(PointCloud cp,float proj_fov_up, float proj_fov_down)
{

    nc::NdArray<_Float32> points = cp.getPoints();
    nc::NdArray<_Float32> remissions = cp.getRemissions();

    // laser parameters
    float fov_up = proj_fov_up / 180.0 * M_PI;     // field of view up in rad
    float fov_down = proj_fov_down / 180.0 * M_PI; // field of view down in rad
    float fov = abs(fov_down) + abs(fov_up);       // get field of view total in rad
    // std::cout << "points:" << cp.getPoints().shape() << std::endl;

    // get depth of all points
    nc::NdArray<double> d_depth = nc::norm(cp.getPoints(), nc::Axis::COL);
    nc::NdArray<_Float32> depth = d_depth.astype<_Float32>().transpose();

    // get scan components
    nc::NdArray<_Float32> scan_x = points(points.rSlice(), 0);
    nc::NdArray<_Float32> scan_y = points(points.rSlice(), 1);
    nc::NdArray<_Float32> scan_z = points(points.rSlice(), 2);

    // get angles of all points
    nc::NdArray<_Float32> yaw = -nc::arctan2(scan_y, scan_x);
    nc::NdArray<_Float32> pitch = nc::arcsin(scan_z / depth);

    // get projections in image coords
    nc::NdArray<_Float32> proj_x = (float)0.5 * (yaw / (float)(nc::constants::pi) + (float)1.0); // in [0.0, 1.0]
    nc::NdArray<_Float32> proj_y = (float)1.0 - (pitch + abs(fov_down)) / (float)fov;            // in [0.0, 1.0]

    // scale to image size using angular resolution
    proj_x *= (float)_width;  // in [0.0, W]
    proj_y *= (float)_height; // in [0.0, H]

    // create zero array for comparison
    nc::NdArray<_Float32> zeroes_x = nc::empty_like(proj_x);
    nc::NdArray<_Float32> zeroes_y = nc::empty_like(proj_y);
    zeroes_x.zeros();
    zeroes_y.zeros();

    // round and clamp for use as index x
    proj_x = nc::floor(proj_x);
    nc::NdArray<_Float32> width_1 = nc::empty_like(proj_x);
    width_1.fill((float)(_width - 1));
    proj_x = nc::minimum(width_1, proj_x);
    proj_x = nc::maximum(zeroes_x, proj_x);
    nc::NdArray<uint32_t> x = proj_x.astype<uint32_t>(); // in [0, W-1]

    // round and clamp for use as index y
    proj_y = nc::floor(proj_y);
    nc::NdArray<_Float32> height_1 = nc::full(proj_y.shape().rows, proj_y.shape().cols, (float)((float)_height - 1));
    proj_y = nc::minimum(height_1, proj_y);
    proj_y = nc::maximum(zeroes_y, proj_y);
    nc::NdArray<uint32_t> y = proj_y.astype<uint32_t>(); // in [0,H-1]

    //order in decreasing depth
    nc::NdArray<uint32_t> indices = nc::arange<uint32_t>(depth.shape().rows);
    nc::NdArray<uint32_t> order = nc::argsort(depth, nc::Axis::ROW);
    order = nc::flip(order, nc::Axis::ROW);

    nc::NdArray<_Float32> depth_sorted = nc::empty_like(depth);
    nc::NdArray<uint32_t> indices_sorted = nc::empty_like(indices);
    nc::NdArray<_Float32> points_sorted = nc::empty_like(points);
    nc::NdArray<_Float32> remissions_sorted = nc::empty_like(remissions);
    nc::NdArray<uint32_t> proj_x_sorted = nc::empty_like(x);
    nc::NdArray<uint32_t> proj_y_sorted = nc::empty_like(y);

    nc::NdArray<_Float32> points_x = points(points.rSlice(), 0);
    nc::NdArray<_Float32> points_y = points(points.rSlice(), 1);
    nc::NdArray<_Float32> points_z = points(points.rSlice(), 2);

    nc::NdArray<_Float32> points_x_sorted = nc::empty_like(points_x);
    nc::NdArray<_Float32> points_y_sorted = nc::empty_like(points_y);
    nc::NdArray<_Float32> points_z_sorted = nc::empty_like(points_z);

    for (uint i = 0; i < depth.size(); i++)
    {
        int idx = order[i];
        proj_x_sorted[i] = proj_x[idx];
        proj_y_sorted[i] = proj_y[idx];
        // indices_sorted[i] = indices[idx];
        points_x_sorted[i] = points_x[idx];
        points_y_sorted[i] = points_y[idx];
        points_z_sorted[i] = points_z[idx];

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
        _data[idx].label = -1;
    }

    // float max = -500000;
    // float min = 500000;
    // for (int i = 0; i < _height * _width; ++i)
    // {
    //     if (max < _data[i].z)
    //         max = _data[i].z;
    //     if (min > _data[i].z)
    //         min = _data[i].z;
    // }
    // std::cout << "max value: " << max << std::endl;
    // std::cout << "min value: " << min << std::endl;
}
