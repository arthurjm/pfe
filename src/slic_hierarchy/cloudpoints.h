#ifndef __CLOUDPOINTS_H__
#define __CLOUDPOINTS_H__

#include <stdlib.h>
#include <string>

#include "NumCpp.hpp"

using namespace std;

class CloudPoints
{
public:
    /**
     * Cloud points must be a raw binary format (.bin) with (Nb_points,4) as dimension 
     * A cloud points is defined by its coordinates (x,y,z) and its remission r
     * @param fileName location of cloud points
     **/
    CloudPoints(string fileName);
    nc::NdArray<float> getPoints() { return _points; };
    nc::NdArray<float> getRemissions() { return _remissions; };

private:
    nc::NdArray<float> _points;
    nc::NdArray<float> _remissions;
};

#endif // __CLOUDPOINTS_H__