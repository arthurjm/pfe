#ifndef __RANGE_IMAGE__
#define __RANGE_IMAGE__

#include <stdlib.h>
#include <string>

#include "NumCpp.hpp"
#include "cloudpoints.h"

#define DIM 6
#define HEIGHT 64
#define WIDTH 1024
#define FOV_UP 3.0
#define FOV_DOWN -25.0

using namespace std;

typedef struct riVertex
{
    float x;
    float y;
    float z;
    float remission;
    float depth;
    float label;
} riVertex;

class RangeImage
{

public:
    /**
     * Range image must be a raw binary format (.bin) with (h * w * 6) as dimension
     * @param fileName location of range image
     * @param width width of the range image
     * @param height height of the range image
     **/
    RangeImage(string fileName, int height = HEIGHT, int width = WIDTH);
    RangeImage(CloudPoints cp, int height = HEIGHT, int width = WIDTH,
               float proj_fov_up = FOV_UP, float proj_fov_down = FOV_DOWN);

private:
    void loadRangeImage(string fileName);

    riVertex *_data;
    int _height;
    int _width;
};

#endif