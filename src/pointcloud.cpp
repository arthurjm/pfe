#include "pointcloud.h"

using namespace std;

PointCloud::PointCloud(string fileName)
{
    createPointCloud(fileName);
    // _pointCloud = getPointCloud(fileName);
}

PointCloud::PointCloud(string pcfileName, string labelfileName)
{
    createPointCloud(pcfileName);
    getLabels(labelfileName);
}

void PointCloud::createPointCloud(string fileName)
{
    _pointCloud.reset(new KittiPointCloud);

    fstream file(fileName.c_str(), ios::in | ios::binary);

    if (file.good())
    {
        file.seekg(0, std::ios::beg);
        int i;
        for (i = 0; file.good() && !file.eof(); i++)
        {
            KittiPoint point;
            file.read((char *)&point.x, 3 * sizeof(float));
            // file.read((char *)&it->intensity, sizeof(float));
            float poubelle;
            file.read((char *)&poubelle, sizeof(float)); // Arthur
            point.r = 255;
            point.g = 255;
            point.b = 255;
            point.a = 255;

            _pointCloud->push_back(point);
        }
        cout << "Nombre de points : " << i << endl;
    }
    file.close();
}

void PointCloud::getLabels(string fileName)
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
    }
    else
    {
        cout << "Pas de fichier label" << endl;
    }
}

void PointCloud::ChangeColor(int colorMode)
{
    std::cout << "Couleur changée" << std::endl;
    int i = 0;
    for (KittiPointCloud::iterator it = _pointCloud->begin(); it != _pointCloud->end(); ++it)
    {
        switch (colorMode)
        {
        case 0: // projection
        {
            it->r = 255;
            it->g = 0;
            it->b = 0;
            it->a = 255;
            break;
        }

        case 1: // VT
        {
            uint16_t l = (uint16_t)_labels.at(i);
            it->a = 255;
            if (l == 0) // truc au loin, observation aberrante ???
            {
                it->r = 100;
                it->g = 100;
                it->b = 100;
            }

            else if (l == 40) // route
            {
                it->r = 128;
                it->g = 64;
                it->b = 128;
            }
            else if (l == 44) // parking ???
            {
                it->r = 115;
                it->g = 52;
                it->b = 99;
            }
            else if (l == 48) // trottoire ???
            {
                it->r = 203;
                it->g = 126;
                it->b = 184;
            }

            else if (l == 50 || l == 51 || l == 52) // batiments
            {
                it->r = 205;
                it->g = 133;
                it->b = 63;
            }

            else if (l == 60) // écriture sol
            {
                it->r = 255;
                it->g = 255;
                it->b = 255;
            }

            // Plantes
            else if (l == 70) // feuillage
            {
                it->r = 10;
                it->g = 76;
                it->b = 21;
            }
            else if (l == 71) // tronc
            {
                it->r = 118;
                it->g = 55;
                it->b = 15;
            }
            else if (l == 72) // herbes
            {
                it->r = 0;
                it->g = 230;
                it->b = 0;
                it->a = 1;
            }

            else if (l == 80) // poteaux
            {
                it->r = 192;
                it->g = 192;
                it->b = 128;
            }
            else if (l == 81) // signalisation route
            {
                it->r = 192;
                it->g = 128;
                it->b = 128;
            }

            else if (l == 99) //
            {
                it->r = 0;
                it->g = 0;
                it->b = 192;
            }

            else if (l == 255 || l == 10) // 255 voiture roule - 10 à l'arrêt
            {
                it->r = 251;
                it->g = 71;
                it->b = 205;
            }

            else
            {
                it->r = 255;
                it->g = 255;
                it->b = 255;
            }
            break;
        }

        case 2: // green
        {
            it->r = 0;
            it->g = 255;
            it->b = 0;
            it->a = 255;
            break;
        }

        default: // white
        {
            it->r = 255;
            it->g = 255;
            it->b = 255;
            it->a = 255;
        }
        }
        ++i;
    }
}

const KittiPointCloud::Ptr PointCloud::getPointCloud()
{
    return _pointCloud;
}
