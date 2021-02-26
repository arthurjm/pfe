#include "shdist.h"

SHDist::SHDist(RangeImage &ri, int type, vector<int> idx)
    : _rangeimage(ri), _type(type), _index(idx)
{
    if (type < 0 || type > 3)
    {
        cerr << "invalide type, please check SHDIST macro SHDIST_{RAW,GRAY,RGB,LAB}" << endl;
        exit(EXIT_FAILURE);
    }
    _vecData.reserve(idx.size());
    for (int i = 0; i < idx.size(); i++)
    {
        if (idx.at(i) >= 0 && idx.at(i) <= 4)
        {
            if (type == SHDIST_RAW)
                _vecData.push_back(ri.getRawDataFromIndex(idx.at(i)));
            else
                _vecData.push_back(convertRawDataToType(idx.at(i)));
        }
        else
        {
            cerr << "invalide type in SHDist constructor, please use RangeImage macro RI_{X,Y,Z,Depth,Remission}" << endl;
            exit(EXIT_FAILURE);
        }
    }
}

SHDist::~SHDist()
{
    _vecData.clear();
}

cv::Mat SHDist::convertRawDataToType(int idx)
{
    cv::Mat m;
    switch (_type)
    {
    case SHDIST_GRAY:
        m = _rangeimage.createColorMat({idx}, true);
        break;
    case SHDIST_BGR:
        m = _rangeimage.createColorMat({idx}, false);
        break;
    case SHDIST_LAB:
        m = _rangeimage.createColorMat({idx}, false);
        break;
    }
    return m;
}

int SHDist::computeDist(int u, int v, int mod, bool spatial2D)
{
    int dist = 0;

    for (int i = 0; i < _vecData.size(); ++i)
    {
        vector<float> vecU, vecV;
        int nbChannel = _vecData.at(i).channels();
        for (int j = 0; j < nbChannel; j++)
        {
            vecU.push_back(_vecData.at(i).data[u * nbChannel + j]);
            vecV.push_back(_vecData.at(i).data[v * nbChannel + j]);
        }
        switch (mod)
        {
        case SHDIST_EUCLIDIAN:
            dist += euclidianDistance(vecU, vecV);
            break;
        case SHDIST_MEAN:
            dist += meanDistance(vecU, vecV);
            break;
        }
    }
    return dist;
}

// euclidian dist

int SHDist::euclidianDistance(vector<float> vecU, vector<float> vecV)
{
    float sum = 0;
    for (int i = 0; i < vecU.size(); i++)
    {
        sum += (vecU.at(i) - vecV.at(i)) * (vecU.at(i) - vecV.at(i));
    }
    int res = sqrtf(sum);
    return res;
}

// mean dist
int SHDist::meanDistance(vector<float> vecU, vector<float> vecV)
{
    float sum = 0;
    for (int i = 0; i < vecU.size(); i++)
    {
        sum += (vecU.at(i) - vecV.at(i));
    }
    int res = sum / (float)vecU.size();
    return res;
}
