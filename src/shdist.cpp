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
                _vecData.push_back(ri.getRawDataFromIndex(idx.at(i)).t());
            else
                _vecData.push_back(convertRawDataToType(idx.at(i)).t());
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
        m = _rangeimage.createColorMat({idx}, true, true);
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

void SHDist::mergeData(int u, int pu, int sizeU, int sizePu)
{
    int sizeVec = _vecData.size();
    int size = sizeU + sizePu;
    for (int i = 0; i < sizeVec; ++i)
    {
        cv::Mat m = _vecData.at(i);
        int nbChannel = m.channels();
        for (int j = 0; j < nbChannel; j++)
        {
            m.data[pu * nbChannel + j] = (m.data[pu * nbChannel + j] * sizePu + m.data[u * nbChannel + j] * sizeU) / size;
        }
    }
}

int SHDist::computeDist(int u, int v, int mod, bool spatial2D)
{
    int dist = 0;
    for (int i = 0; i < _vecData.size(); ++i)
    {
        int type = _vecData.at(i).type();
        vector<float> vecU, vecV;
        int nbChannel = _vecData.at(i).channels();
        for (int j = 0; j < nbChannel; j++)
        {
            if (_vecData.at(i).type() == CV_32FC1 || _vecData.at(i).type() == CV_32FC3)
            {
                vecU.push_back(_vecData.at(i).at<float>(u * nbChannel + j));
                vecV.push_back(_vecData.at(i).at<float>(v * nbChannel + j));
            }
            else
            {
                vecU.push_back(_vecData.at(i).at<uchar>(u * nbChannel + j));
                vecV.push_back(_vecData.at(i).at<uchar>(v * nbChannel + j));
            }
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

int SHDist::euclidianDistance(vector<float> vecU, vector<float> vecV)
{
    if (vecU.size() != vecV.size())
    {
        cerr << "vecU and vecV have differents size in euclidianDistance in SHDist" << endl;
        exit(EXIT_FAILURE);
    }
    float sum = 0;
    for (int i = 0; i < vecU.size(); i++)
    {
        sum += (vecU.at(i) - vecV.at(i)) * (vecU.at(i) - vecV.at(i));
    }
    int res = sqrtf(sum);
    return res;
}

int SHDist::meanDistance(vector<float> vecU, vector<float> vecV)
{
    if (vecU.size() != vecV.size())
    {
        cerr << "vecU and vecV have differents size in meanDistance in SHDist" << endl;
        exit(EXIT_FAILURE);
    }
    int size = vecU.size();
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += abs(vecU[i] - vecV[i]);
    }
    int res = sum / (float)size;
    return res;
}
