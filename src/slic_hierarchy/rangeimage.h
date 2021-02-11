#ifndef __RANGE_IMAGE__
#define __RANGE_IMAGE__

#include <stdlib.h>
#include <string>

#include "NumCpp.hpp"

#define DIM 6

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
    RangeImage(string fileName, int width = 1024, int height = 64);

private:
    void loadRangeImage(string fileName);

    riVertex *_data;
    int _width;
    int _height;
};

#endif