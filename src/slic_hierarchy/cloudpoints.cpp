#include "cloudpoints.h"

CloudPoints::CloudPoints(string fileName) 
{
    // Need to check if filename exists 
    nc::NdArray<float> scan = nc::fromfile<float>(fileName, "");
    scan.reshape(-1, 4);
    
    _points = scan(scan.rSlice(),nc::Slice(0,3));
    _remissions = scan(scan.rSlice(),3);
}
