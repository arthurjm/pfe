#include "pointcloud.h"

using namespace std;

PointCloud::PointCloud(string fileName)
{
    createPointCloud(fileName);
    // _pointCloud = getPointCloud(fileName);
}

PointCloud::PointCloud(string pcfileName, string labelfileName)
{
    _fileName = pcfileName;
    createPointCloud(pcfileName);
    getLabels(labelfileName);
    // generateRangeImage(pcfileName);
}

PointCloud::PointCloud(string pcfileName, string labelfileName, ClickableLabel *cl)
{
    _fileName = pcfileName;
    createPointCloud(pcfileName);
    getLabels(labelfileName);
    _cl = cl;
    // generateRangeImage(pcfileName);
}

void PointCloud::createPointCloud(string fileName)
{
    _pointCloud.reset(new KittiPointCloud);

    fstream file(fileName.c_str(), ios::in | ios::binary);

    if (file.good())
    {
        file.seekg(0, std::ios::beg);
        int i;
        float intensity;
        for (i = 0; file.good() && !file.eof(); i++)
        {
            KittiPoint point;
            file.read((char *)&point.x, 3 * sizeof(float));
            point.r = 255;
            point.g = 255;
            point.b = 255;
            point.a = 255;

            _pointCloud->push_back(point);

            file.read((char *)&intensity, sizeof(float));
            _remissions.push_back(intensity);
        }
        cout << "Nombre de points : " << _pointCloud->size() << endl;
    }
    file.close();
}

RangeImage PointCloud::generateRangeImage(int width, int height)
{
    riVertex *data = (riVertex *)malloc(sizeof(riVertex) * width * height);
    assert(data);

    for (int i = 0; i < height * width; ++i)
    {
        data[i].x = -1;
        data[i].y = -1;
        data[i].z = -1;
        data[i].remission = -1;
        data[i].depth = -1;
        data[i].label = -1;
    }

    float fov_up = FOV_UP / 180.0 * M_PI;     // field of view up in rad
    float fov_down = FOV_DOWN / 180.0 * M_PI; // field of view down in rad
    float fov = abs(fov_down) + abs(fov_up);  // get field of view total in rad

    int i = 0;
    for (KittiPointCloud::iterator it = _pointCloud->begin(); it != _pointCloud->end(); ++it)
    {
        float x = it->x;
        float y = it->y;
        float z = it->z;

        float depth = sqrt(x * x + y * y + z * z);

        float yaw = -atan2(y, x);
        float pitch = asin(z / depth);

        float proj_x = 0.5 * (yaw / M_PI + 1.0);            // in [0.0, 1.0]
        float proj_y = 1.0 - (pitch + abs(fov_down)) / fov; // in [0.0, 1.0]

        proj_x *= width;  // in [0.0, W]
        proj_y *= height; // in [0.0, H]

        uint32_t proj_x_uint = (uint32_t)floor(proj_x);
        proj_x_uint = min((uint32_t)width - 1, proj_x_uint);
        proj_x_uint = max((uint32_t)0, proj_x_uint);

        uint32_t proj_y_uint = (uint32_t)floor(proj_y);
        proj_y_uint = min((uint32_t)height - 1, proj_y_uint);
        proj_y_uint = max((uint32_t)0, proj_y_uint);

        int idx = width * proj_y_uint + proj_x_uint;

        if (data[idx].depth >= depth || data[idx].depth < 0)
        {
            _projectedPoints[idx].insert(_projectedPoints[idx].begin(), i);

            data[idx].x = x;
            data[idx].y = y;
            data[idx].z = z;
            data[idx].remission = _remissions.at(i);
            data[idx].depth = depth;
            data[idx].label = -1;
        }
        i++;
    }

    RangeImage ri(data, width, height);
    _rangeImage = &ri;
    return ri;
}

bool PointCloud::getLabels(string fileName)
{
    fstream file(fileName.c_str(), ios::in | ios::binary);
    vector<uint32_t> labels;

    if (file.good())
    {
        file.seekg(0, std::ios::beg);
        int i;
        for (i = 0; file.good() && !file.eof(); i++)
        {
            uint32_t label;
            file.read((char *)&label, sizeof(uint32_t));

            _labels.push_back(label);
        }
        cout << "Nombre de labels : " << i << endl;
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

void PointCloud::ChangeColor(Color colorMode)
{
    std::cout << "Couleur changée" << std::endl;
    int i = 0;

    switch (colorMode)
    {
    case Color::Projection:
    {
        for (KittiPointCloud::iterator it = _pointCloud->begin(); it != _pointCloud->end(); ++it)
        {
            it->r = 255;
            it->g = 255;
            it->b = 255;
            it->a = 50;
        }
        for (map<int, std::vector<int>>::iterator it = _projectedPoints.begin(); it != _projectedPoints.end(); ++it)
        {
            i = it->second.at(0);
            _pointCloud->at(i).r = 255;
            _pointCloud->at(i).g = 255;
            _pointCloud->at(i).b = 255;
            _pointCloud->at(i).a = 255;
        }
        break;
    }

    case Color::GroundTruth:
    {
        if (_labels.size() == 0)
            break;
        for (KittiPointCloud::iterator it = _pointCloud->begin(); it != _pointCloud->end(); ++it)
        {
            uint16_t l = (uint16_t)_labels.at(i);
            it->r = _labelMap[(Label)l].at(2);
            it->g = _labelMap[(Label)l].at(1);
            it->b = _labelMap[(Label)l].at(0);
            it->a = 255;
            ++i;
        }
        break;
    }

    case Color::Segmentation:
    {
        for (KittiPointCloud::iterator it = _pointCloud->begin(); it != _pointCloud->end(); ++it)
        {
            it->r = 0;
            it->g = 0;
            it->b = 0;
            it->a = 0;
        }

        RangeImage *ri = _cl->getRangeImage();
        const riVertex *riData = ri->getData();
        Slic *slic = _cl->getSlic();

        unsigned int nbSuperPixel = slic->nbLabels() - 1;
        vector<int> labels = slic->getLabelVec();
        for (int i = 0; i < nbSuperPixel; i++)
        {
            int label = labels.at(i);

            vector<pair<int, int>> pixels = slic->pixelsOfSuperpixel(i);
            unsigned int size = pixels.size();
            for (unsigned int j = 0; j < size; j++)
            {
                int mapIdx = pixels.at(j).first * WIDTH + pixels.at(j).second;
                if (_projectedPoints.count(mapIdx) > 0)
                {
                    int pcIdx = _projectedPoints.at(mapIdx).at(0);

                    if (label == SLIC_LABEL_GROUND)
                    {
                        _pointCloud->at(pcIdx).b = 168;
                        _pointCloud->at(pcIdx).g = 127;
                        _pointCloud->at(pcIdx).r = 173;
                        _pointCloud->at(pcIdx).a = 255;
                    }
                    if (label == SLIC_LABEL_STUCTURE)
                    {
                        _pointCloud->at(pcIdx).b = 78;
                        _pointCloud->at(pcIdx).g = 178;
                        _pointCloud->at(pcIdx).r = 185;
                        _pointCloud->at(pcIdx).a = 255;
                    }
                    if (label == SLIC_LABEL_VEHICLE)
                    {
                        _pointCloud->at(pcIdx).b = 205;
                        _pointCloud->at(pcIdx).g = 178;
                        _pointCloud->at(pcIdx).r = 98;
                        _pointCloud->at(pcIdx).a = 255;
                    }
                    if (label == SLIC_LABEL_NATURE)
                    {
                        _pointCloud->at(pcIdx).b = 109;
                        _pointCloud->at(pcIdx).g = 167;
                        _pointCloud->at(pcIdx).r = 96;
                        _pointCloud->at(pcIdx).a = 255;
                    }
                    if (label == SLIC_LABEL_HUMAN)
                    {
                        _pointCloud->at(pcIdx).b = 0;
                        _pointCloud->at(pcIdx).g = 0;
                        _pointCloud->at(pcIdx).r = 255;
                        _pointCloud->at(pcIdx).a = 255;
                    }
                    if (label == SLIC_LABEL_OBJECT)
                    {
                        _pointCloud->at(pcIdx).b = 50;
                        _pointCloud->at(pcIdx).g = 88;
                        _pointCloud->at(pcIdx).r = 140;
                        _pointCloud->at(pcIdx).a = 255;
                    }
                    if (label == SLIC_LABEL_OUTLIER)
                    {
                        _pointCloud->at(pcIdx).b = 128;
                        _pointCloud->at(pcIdx).g = 128;
                        _pointCloud->at(pcIdx).r = 128;
                        _pointCloud->at(pcIdx).a = 255;
                    }
                }
            }
        }
        break;
    }

    default: // White
    {
        for (KittiPointCloud::iterator it = _pointCloud->begin(); it != _pointCloud->end(); ++it)
        {
            it->r = 255;
            it->g = 255;
            it->b = 255;
            it->a = 255;
        }
    }
    }
}

const KittiPointCloud::Ptr PointCloud::getPointCloud()
{
    return _pointCloud;
}