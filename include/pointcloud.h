#ifndef __CLOUDPOINTS_H__
#define __CLOUDPOINTS_H__

#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/point_cloud_color_handlers.h>

#include <stdlib.h>
#include <stdint.h>
#include <string>



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
private:
    void createPointCloud(std::string fileName);
    void getLabels(std::string labelfileName);
    KittiPointCloud::Ptr _pointCloud;
    std::vector<uint32_t> _labels;
};

#endif // __CLOUDPOINTS_H__