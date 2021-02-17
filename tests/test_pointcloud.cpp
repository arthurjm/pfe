#include "pointcloud.h"
#include <gtest/gtest.h>

#define CORRECT_FILE "../data/pointclouds_kitti/000242.bin"
#define WRONG_SHAPE_FILE "../data/test_files/shape.bin"
#define WRONG_REMISSIONS_FILE "../data/test_files/remission.bin"

TEST(PointCloud, creation)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE);
    ASSERT_NE(pc, nullptr);
}

TEST(PointCloud, points)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE);
    auto points = pc->getPoints();
    uint32_t rows = (uint32_t)points.shape().rows;
    uint32_t cols = (uint32_t)points.shape().cols;
    ASSERT_NE(rows, 0);
    ASSERT_EQ(cols, 3); //xyz
}

TEST(PointCloud, remission)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE);
    auto remissions = pc->getRemissions();
    uint32_t rows = (uint32_t)remissions.shape().rows;
    uint32_t cols = (uint32_t)remissions.shape().cols;

    EXPECT_NE(rows, 0);
    EXPECT_EQ(cols, 1);
    float min = remissions.min()[0];
    float max = remissions.max()[0];
    EXPECT_GT(max, (float)0.);
    EXPECT_LE(min, (float)1.);
}
