#ifndef __CLOUDPOINTS_H__
#define __CLOUDPOINTS_H__

#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/point_cloud_color_handlers.h>

#include <stdlib.h>
#include <stdint.h>
#include <string>

#include "rangeimage.h"
#include "clickablelabel.h"

#define FOV_UP 3.0
#define FOV_DOWN -25.0

#define WIDTH 1024
#define HEIGHT 64

typedef pcl::PointXYZRGBA KittiPoint;
typedef pcl::PointCloud<KittiPoint> KittiPointCloud;
typedef pcl::visualization::PointCloudColorHandlerCustom<KittiPoint> KittiPointCloudColorHandlerCustom;

enum class Color
{
    White,
    Projection,
    GroundTruth,
    Segmentation
};

enum class Label
{
    unlabeled = 0,
    outlier = 1,
    car = 10,
    bicycle = 11,
    bus = 13,
    motorcycle = 15,
    on_rails = 16,
    truck = 18,
    other_vehicle = 20,
    person = 30,
    bicyclist = 31,
    motorcyclist = 32,
    road = 40,
    parking = 44,
    sidewalk = 48,
    other_ground = 49,
    building = 50,
    fence = 51,
    other_structure = 52,
    lane_marking = 60,
    vegetation = 70,
    trunk = 71,
    terrain = 72,
    pole = 80,
    traffic_sign = 81,
    other_object = 99,
    moving_car = 252,
    moving_bicyclist = 253,
    moving_person = 254,
    moving_motorcyclist = 255,
    moving_on_rails = 256,
    moving_bus = 257,
    moving_truck = 258,
    moving_other_vehicle = 259
};

class PointCloud
{
public:
    /**
     * Cloud points must be a raw binary format (.bin) with (Nb_points,4) as dimension 
     * A cloud points is defined by its coordinates (x,y,z) and its remission r
     * @param fileName location of cloud points
     **/
    PointCloud(std::string pcFileName);
    PointCloud(std::string pcFileName, std::string labelFileName);

    bool openLabels(std::string fileName);
    bool saveLabels(std::string fileName);

    void ChangeColor(Color colorMode = Color::White);
    const KittiPointCloud::Ptr getPointCloud();
    RangeImage generateRangeImage(bool groundTruth = false, int width = WIDTH, int height = HEIGHT);

private:
    void createPointCloud(std::string fileName);
    void getSelectedLabels();

    KittiPointCloud::Ptr _pointCloud;
    std::vector<float> _remissions;
    std::vector<uint16_t> _labels;
    std::vector<uint16_t> _selectedLabels;

    std::map<int, std::vector<int>> _projectedPoints;
    RangeImage _rangeImage;

    // bgr
    std::map<Label, std::vector<uint8_t>> _labelMap = {
        {Label::unlabeled, {0, 0, 0}},
        {Label::outlier, {75, 75, 75}},
        {Label::car, {245, 150, 100}},
        {Label::bicycle, {245, 230, 100}},
        {Label::bus, {250, 80, 100}},
        {Label::motorcycle, {150, 60, 30}},
        {Label::on_rails, {255, 0, 0}},
        {Label::truck, {180, 30, 80}},
        {Label::other_vehicle, {255, 0, 0}},
        {Label::person, {30, 30, 255}},
        {Label::bicyclist, {200, 40, 255}},
        {Label::motorcyclist, {90, 30, 150}},
        {Label::road, {255, 0, 255}},
        {Label::parking, {255, 150, 255}},
        {Label::sidewalk, {75, 0, 75}},
        {Label::other_ground, {75, 0, 175}},
        {Label::building, {0, 200, 255}},
        {Label::fence, {50, 120, 255}},
        {Label::other_structure, {0, 150, 255}},
        {Label::lane_marking, {170, 255, 150}},
        {Label::vegetation, {0, 175, 0}},
        {Label::trunk, {0, 60, 135}},
        {Label::terrain, {80, 240, 150}},
        {Label::pole, {150, 240, 255}},
        {Label::traffic_sign, {0, 0, 255}},
        {Label::other_object, {255, 255, 50}},
        {Label::moving_car, {245, 150, 100}},
        {Label::moving_bicyclist, {255, 0, 0}},
        {Label::moving_person, {200, 40, 255}},
        {Label::moving_motorcyclist, {30, 30, 255}},
        {Label::moving_on_rails, {90, 30, 255}},
        {Label::moving_bus, {250, 80, 100}},
        {Label::moving_truck, {180, 30, 80}},
        {Label::moving_other_vehicle, {255, 0, 0}}};
};

#endif // __CLOUDPOINTS_H__