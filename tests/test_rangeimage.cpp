#include "rangeimage.h"
#include "pointcloud.h"
#include <gtest/gtest.h>

#define CORRECT_FILE "../data/pointclouds_kitti/000242.bin"
#define WRONG_SHAPE_FILE "../data/test_files/shape.bin"
#define WRONG_REMISSIONS_FILE "../data/test_files/remissions.bin"

#define EPSILON 0.0001
TEST(RangeImage, creation)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE);
    ASSERT_NE(pc, nullptr);

    RangeImage *ri_pc = new RangeImage(*pc);
    ASSERT_NE(ri_pc, nullptr);
    ASSERT_GE(ri_pc->getWidth(), 0);
    ASSERT_GE(ri_pc->getHeight(), 0);

    RangeImage *ri_file = new RangeImage(CORRECT_FILE);
    EXPECT_NE(ri_file, nullptr);
    ASSERT_GE(ri_pc->getWidth(), 0);
    ASSERT_GE(ri_pc->getHeight(), 0);
}

TEST(RangeImage, points)
{
    PointCloud pc(CORRECT_FILE);
    RangeImage ri(pc);
    auto points = pc.getPoints();
    const riVertex *data = ri.getData();
    int pc_size = points.shape().rows;
    int ri_size = ri.getWidth() * ri.getHeight();

    // Checks that all projected xyz coordinates exists in the point cloud and projected only once in range image
    int c = 0;
    for (int i = 0; i < ri_size; i++)
    {
        auto x = data[i].x;
        auto y = data[i].y;
        auto z = data[i].z;

        c = 0;
        for (int j = 0; j < pc_size; j++)
        {
            if (x - points(j, 0) < EPSILON && y - points(j, 1) < EPSILON && z - points(j, 2) < EPSILON)
            {
                c += 1;
                break;
            }
        }
        if (c != 1)
            break;
    }
    EXPECT_EQ(c, 1);
}

TEST(RangeImage, remssions)
{
    PointCloud pc(CORRECT_FILE);
    RangeImage ri(pc);
    const riVertex *data = ri.getData();

    int size = ri.getWidth() * ri.getHeight();
    float min = 2;
    float max = -1;

    // Remission is a percentage [0,1]
    for (int i = 0; i < size; i++)
    {
        if (min > data[i].remission && data[i].remission != -1.)
            min = data[i].remission;
        if (max < data[i].remission)
            max = data[i].remission;
    }

    EXPECT_LE(min, 1.0);
    EXPECT_GT(max, 0.0);
}

TEST(RangeImage, depth)
{
    PointCloud pc(CORRECT_FILE);
    RangeImage ri(pc);
    const riVertex *data = ri.getData();

    // Depth is always >0
    int size = ri.getWidth() * ri.getHeight();
    for (int i = 0; i < size; i++)
    {
        if (data[i].depth != -1)
            EXPECT_GE(data[i].depth, 0.);
    }
}

TEST(RangeImage, both_methods)
{
}