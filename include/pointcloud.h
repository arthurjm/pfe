#ifndef __CLOUDPOINTS_H__
#define __CLOUDPOINTS_H__

#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/point_cloud_color_handlers.h>

#include <stdlib.h>
#include <stdint.h>
#include <string>

#include "rangeimage.h"

#define FOV_UP 3.0
#define FOV_DOWN -25.0

#define WIDTH 1024
#define HEIGHT 64

typedef pcl::PointXYZRGBA KittiPoint;
// typedef pcl::PointXYZI KittiPoint;
typedef pcl::PointCloud<KittiPoint> KittiPointCloud;
typedef pcl::visualization::PointCloudColorHandlerCustom<KittiPoint> KittiPointCloudColorHandlerCustom;

class PointCloud
{
public:
    /**
     * Cloud points must be a raw binary format (.bin) with (Nb_points,4) as dimension 
     * A cloud points is defined by its coordinates (x,y,z) and its remission r
     * @param fileName location of cloud points
     **/
    PointCloud(std::string fileName);
    PointCloud(std::string pcfileName, std::string labelfileName);
    void ChangeColor(int colorMode);
    const KittiPointCloud::Ptr getPointCloud();
    RangeImage generateRangeImage(int width = WIDTH, int height = HEIGHT);

private:
    void createPointCloud(std::string fileName);
    bool getLabels(std::string labelfileName);
    // void generateRangeImage(std::string fileName, int width = 1024, int height = 64);

    std::string _fileName;

    KittiPointCloud::Ptr _pointCloud;
    std::vector<float> _remissions;
    std::vector<uint32_t> _labels;

    std::map<int, std::vector<int>> _projectedPoints;
    // std::vector<bool> _projectedPoints;
    RangeImage _ri;
};

#endif // __CLOUDPOINTS_H__