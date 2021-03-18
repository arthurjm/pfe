#include "pointcloud.h"
#include "rangeimage.h"
#include <gtest/gtest.h>

#define CORRECT_FILE "../data/velodyne/000000.bin"
#define CORRECT_LABEL "../data/labels/000000.label"
#define EPSILON 0.0001

TEST(PointCloud, creation)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE);
    ASSERT_NE(pc, nullptr);
}

TEST(PointCloud, creation_with_label)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE, CORRECT_LABEL);
    ASSERT_NE(pc, nullptr);
}

TEST(RangeImage, creation)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE);
    RangeImage ri_pc = pc->generateRangeImage();
    ASSERT_GE(ri_pc.getWidth(), 0);
    ASSERT_GE(ri_pc.getHeight(), 0);
}

TEST(RangeImage, remission)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE);
    RangeImage ri_pc = pc->generateRangeImage();
    uint size = ri_pc.getWidth() * ri_pc.getHeight();
    const riVertex *data = ri_pc.getData();
    for (uint i = 0; i < size; i++)
    {
        auto d = data[i];
        EXPECT_GE(d.remission, 0);
        EXPECT_LE(d.remission, 1);
    }
}

TEST(RangeImage, depth)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE);
    RangeImage ri_pc = pc->generateRangeImage();
    const riVertex *data = ri_pc.getData();
    uint size = ri_pc.getWidth() * ri_pc.getHeight();

    for (uint i = 0; i < size; i++)
    {
        auto d = data[i];
        EXPECT_GE(d.depth, 0);
    }
}

TEST(RangeImage, proj_labels)
{
    PointCloud *pc = new PointCloud(CORRECT_FILE, CORRECT_LABEL);
    RangeImage ri_pc = pc->generateRangeImage();
    const riVertex *data = ri_pc.getData();
    uint size = ri_pc.getWidth() * ri_pc.getHeight();

    bool check_loaded = false;
    for (uint i = 0; i < size; i++)
    {
        auto d = data[i];
        if (d.label != -1)
            check_loaded = true;
    }
    EXPECT_TRUE(check_loaded);

    PointCloud *pc_nolabel = new PointCloud(CORRECT_FILE);
    RangeImage ri_nolabel = pc->generateRangeImage();
    const riVertex *data_nolabel = ri_nolabel.getData();
    size = ri_nolabel.getWidth() * ri_nolabel.getHeight();

    check_loaded = false;
    for (uint i = 0; i < size; i++)
    {
        auto d = data_nolabel[i];
        if (d.label != -1)
            check_loaded = true;
    }
    EXPECT_FALSE(check_loaded);
}