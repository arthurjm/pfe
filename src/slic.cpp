#include "slic.h"

void Slic::clearData()
{
    _clusters.release();
    _distances.release();
    _centers.release();
    _centersNormalized4D.release();
    _centersInterpolated4D.release();
    _centerCounts.clear();
    _selectedClusters.clear();
    _cls.clear();
}

void Slic::initData()
{
    int width = _rangeImage.getWidth();
    int height = _rangeImage.getHeight();
    /* Initialize the cluster and distance matrices. */
    if (width <= 0 || height <= 0)
    {
        std::cerr << "The image size has to be strictly positive" << std::endl;
        return;
    }
    if (_step < 1)
    {
        std::cerr << "The number of requested superpixels is too huge" << std::endl;
        return;
    }
    _clusters = cv::Mat_<int>(height, width, -1);
    _distances = cv::Mat_<float>(height, width, FLT_MAX);

    /* Initialize the centers and counters. */
    const vector<float> *interpolatedData = _rangeImage.getNormalizedAndInterpolatedData();
    for (int col = _step / 2; col <= width - _step / 2; col += _step)
    {
        for (int row = _step / 2; row <= height - _step / 2; row += _step)
        {
            /* Find the local minimum (gradient-wise). */
            cv::Point nc = findLocalMinimum(cv::Point(row, col));
            int idx = nc.y + nc.x * width;
            /* Generate the centers vector. */
            riVertex coord = _rangeImage.getNormalizedValue(idx);

            Vec2f center(nc.x, nc.y);
            Vec4f centerNormalized4D = Vec4f(coord.x, coord.y, coord.z, coord.remission);

            Vec4f centerInterpolated4D = Vec4f(interpolatedData->at(idx * 4 + SLIC_METRIC_X), interpolatedData->at(idx * 4 + SLIC_METRIC_Y),
                                               interpolatedData->at(idx * 4 + SLIC_METRIC_Z), interpolatedData->at(idx * 4 + SLIC_METRIC_REMISSION));

            _centers.push_back(center);
            _centersNormalized4D.push_back(centerNormalized4D);
            _centersInterpolated4D.push_back(centerInterpolated4D);
            _centerCounts.push_back(0);
        }
    }
}

float Slic::computeDist(int clusterIndex, cv::Point pixel, bool metrics[4])
{
    int idx = pixel.y + pixel.x * _rangeImage.getWidth();
    riVertex normalizedData = _rangeImage.getNormalizedValue(idx);

    // metrics, normalized raw data with interpolation
    const vector<float> *interpolatedData = _rangeImage.getNormalizedAndInterpolatedData();
    Vec4f cenInterpolated4d(_centersInterpolated4D(clusterIndex));
    float distMetrics = 0;
    if (metrics[SLIC_METRIC_X])
    {
        float dif = cenInterpolated4d[SLIC_METRIC_X] - interpolatedData->at(idx * 4 + SLIC_METRIC_X);
        distMetrics += dif * dif;
    }
    if (metrics[SLIC_METRIC_Y])
    {
        float dif = cenInterpolated4d[SLIC_METRIC_Y] - interpolatedData->at(idx * 4 + SLIC_METRIC_Y);
        distMetrics += dif * dif;
    }
    if (metrics[SLIC_METRIC_Z])
    {
        float dif = cenInterpolated4d[SLIC_METRIC_Z] - interpolatedData->at(idx * 4 + SLIC_METRIC_Z);
        distMetrics += dif * dif;
    }
    if (metrics[SLIC_METRIC_REMISSION])
    {
        float dif = cenInterpolated4d[SLIC_METRIC_REMISSION] - interpolatedData->at(idx * 4 + SLIC_METRIC_REMISSION);
        distMetrics += dif * dif;
    }
    if (distMetrics != 0)
    {
        distMetrics = sqrt(distMetrics);
    }
    // normalized raw data

    // spatial 3D distance
    Vec4f cenNormalized4d(_centersNormalized4D(clusterIndex));
    float dr0 = cenNormalized4d[SLIC_METRIC_X] - normalizedData.x;
    float dr1 = cenNormalized4d[SLIC_METRIC_Y] - normalizedData.y;
    float dr2 = cenNormalized4d[SLIC_METRIC_Z] - normalizedData.z;
    float dr = sqrt(dr0 * dr0 + dr1 * dr1 + dr2 * dr2);

    // intensity distance
    float di = cenNormalized4d[SLIC_METRIC_REMISSION] - normalizedData.remission;
    di = sqrt(di * di);

    // spatial 2D distance
    // float dc3 = cen[3] - pPixel.x;
    // float dc4 = cen[4] - pPixel.y;
    // float ds = sqrt(dc3 * dc3 + dc4 * dc4);

    return dr + di + distMetrics;

    // default slic distance
    // return dc + (_nc / _step) * ds;

    //float w = 1.0 / (pow(ns / nc, 2));
    //return sqrt(dc) + sqrt(ds * w);
}

cv::Point Slic::findLocalMinimum(cv::Point pCenter)
{
    float min_grad = FLT_MAX;
    cv::Point loc_min(pCenter.x, pCenter.y);
    for (int i = pCenter.x - 1; i < pCenter.x + 2; i++)
    {
        for (int j = pCenter.y - 1; j < pCenter.y + 2; j++)
        {
            riVertex riv1 = _rangeImage.getNormalizedValue(i * _rangeImage.getWidth() + (j + 1));
            riVertex riv2 = _rangeImage.getNormalizedValue((i + 1) * _rangeImage.getWidth() + j);
            riVertex riv3 = _rangeImage.getNormalizedValue(i * _rangeImage.getWidth() + j);
            // distance between riVertex 1 and riVertex 3
            float dx = riv1.x - riv3.x;
            float dy = riv1.y - riv3.y;
            float dz = riv1.z - riv3.z;
            float distRiv1Riv3 = sqrt(dx * dx + dy * dy + dz * dz);

            // distance between riVertex 2 and riVertex 3
            dx = riv2.x - riv3.x;
            dy = riv2.y - riv3.y;
            dz = riv2.z - riv3.z;
            float distRiv2Riv3 = sqrt(dx * dx + dy * dy + dz * dz);

            float sum = distRiv1Riv3 + distRiv2Riv3;
            if (sum < min_grad)
            {
                min_grad = sum;
                loc_min.x = i;
                loc_min.y = j;
            }
        }
    }
    return loc_min;
}

// TODO : delete pNc (weight) or rename
void Slic::generateSuperpixels(int nbSpx, int pNc, RangeImage &ri, bool metrics[4])
{
    int width = ri.getWidth();
    int height = ri.getHeight();
    int size = height * width;
    if (nbSpx <= 0 || nbSpx > size)
    {
        std::cerr << "The number of superpixels should be between 1 and number of pixels of range image" << std::endl;
        return;
    }

    if (width == 0 || height == 0)
    {
        std::cerr << "The size of the image shouldn't be 0" << std::endl;
        return;
    }
    int step = sqrt(size / (double)nbSpx);
    if (step == 0)
    {
        std::cerr << "The number of superpixels should not exceed the amount of pixels in the image" << std::endl;
        step = 1;
    }
    _selectedClusters.clear();
    _step = step;
    // TODO : delete (weight) pNc or rename
    _nc = pNc;

    /* Clear previous data (if any), and re-initialize it. */
    clearData();
    _rangeImage = ri;
    initData();

    /* Run segmentation for 10 iterations (as prescribed by the algorithm). */
    for (int i = 0; i < NR_ITERATIONS; i++)
    {
        /* Reset distance values. */
        _distances = FLT_MAX;
        for (int j = 0; j < _centers.rows; j++)
        {
            Vec2f cen(_centers(j));
            /* Only compare to pixels in a 2 x step by 2 x step region. */
            for (int row = cen[0] - step; row < cen[0] + step; row++)
            {
                for (int col = cen[1] - step; col < cen[1] + step; col++)
                {

                    if (row >= 0 && row < height && col >= 0 && col < width)
                    {
                        float d = computeDist(j, cv::Point(row, col), metrics);

                        /* Update cluster allocation if the cluster minimizes the
                           distance. */
                        if (d < _distances(row, col))
                        {
                            _distances(row, col) = d;
                            _clusters(row, col) = j;
                        }
                    }
                }
            }
        }

        /* Clear the center values. */
        for (int j = 0; j < _centers.rows; j++)
        {
            _centers(j) = 0;
            _centersNormalized4D(j) = 0;
            _centersInterpolated4D(j) = 0;
            _centerCounts[j] = 0;
        }

        /* Compute the new cluster centers. */
        const vector<float> *interpolatedData = _rangeImage.getNormalizedAndInterpolatedData();
        for (int col = 0; col < width; col++)
        {
            for (int row = 0; row < height; row++)
            {
                int c_id = _clusters(row, col);

                if (c_id != -1)
                {
                    _centers(c_id) += Vec2f(row, col);

                    int idx = col + row * width;
                    riVertex coord = _rangeImage.getNormalizedValue(idx);
                    _centersNormalized4D(c_id) += Vec4f(coord.x, coord.y, coord.z, coord.remission);

                    _centersInterpolated4D(c_id) += Vec4f(interpolatedData->at(idx * 4 + SLIC_METRIC_X), interpolatedData->at(idx * 4 + SLIC_METRIC_Y),
                                                          interpolatedData->at(idx * 4 + SLIC_METRIC_Z), interpolatedData->at(idx * 4 + SLIC_METRIC_REMISSION));
                    _centerCounts[c_id] += 1;
                }
            }
        }

        /* Normalize the clusters. */
        for (int j = 0; j < _centers.rows; j++)
        {
            _centers(j) /= _centerCounts[j];
            _centersNormalized4D(j) /= _centerCounts[j];
            _centersInterpolated4D(j) /= _centerCounts[j];
        }
    }
}

void Slic::createConnectivity()
{
    int width = _rangeImage.getWidth();
    int height = _rangeImage.getHeight();
    /* Initialize the cluster and distance matrices. */
    if (width <= 0 || height <= 0)
    {
        std::cerr << "Failed to create connectivity in Slic::createConnectivity, the range image is empty" << std::endl;
        exit(EXIT_FAILURE);
    }

    int label = 0, adjlabel = 0;
    int lims;
    if (_centers.rows == 0)
    {
        lims = 0;
    }
    else
    {
        lims = (width * height) / (_centers.rows);
    }

    const int dr4[4] = {0, -1, 0, 1};
    const int dc4[4] = {-1, 0, 1, 0};

    /* Initialize the new cluster matrix. */
    cv::Mat_<int> new_clusters(height, width, -1);

    for (int col = 0; col < width; col++)
    {
        for (int row = 0; row < height; row++)
        {
            if (new_clusters(row, col) == -1)
            {
                vector<cv::Point> elements;
                elements.push_back(cv::Point(row, col));

                /* Find an adjacent label, for possible use later. */
                for (int k = 0; k < 4; k++)
                {
                    int r = elements[0].x + dr4[k];
                    int c = elements[0].y + dc4[k];

                    if (r >= 0 && r < height && c >= 0 && c < width)
                    {
                        if (new_clusters(r, c) >= 0)
                        {
                            adjlabel = new_clusters(r, c);
                        }
                    }
                }

                int count = 1;
                for (int cpt = 0; cpt < count; cpt++)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int r = elements[cpt].x + dr4[k];
                        int c = elements[cpt].y + dc4[k];

                        if (r >= 0 && r < height && c >= 0 && c < width)
                        {
                            if (new_clusters(r, c) == -1 && _clusters(row, col) == _clusters(r, c))
                            {
                                elements.push_back(cv::Point(r, c));
                                new_clusters(r, c) = label;
                                ++count;
                            }
                        }
                    }
                }

                /* Use the earlier found adjacent label if a segment size is
                   smaller than a limit. */
                if (count <= lims >> 2)
                {
                    for (int cpt = 0; cpt < count; cpt++)
                    {
                        new_clusters(elements[cpt].x, elements[cpt].y) = adjlabel;
                    }
                    --label;
                }
                ++label;
            }
        }
    }
    _nbLabels = label;
    _clusters = new_clusters;
    initCls(height, width);
}

/**
 * Computes the distance between two superpixels.
 * @param msi metrics vector
 * @param ms spatial distance 3D 
 * @param mr remission distance
 * @param mt number of pixels in a cluster
 * @param i cluster i 
 * @param j cluster j
 * @param spn number of labels (cluster)
 * @param compactness constant variable define in createHierarchy
 * @return distance between two superpixels.
 * */
float distance(std::vector<float> msi, std::vector<float> ms, std::vector<float> mr, std::vector<float> mt, int i, int j, int spn, float compactness)
{
    float dist_si = (msi[j] - msi[i]) * (msi[j] - msi[i]) +
                    (msi[j + spn] - msi[i + spn]) * (msi[j + spn] - msi[i + spn]) +
                    (msi[j + spn * 2] - msi[i + spn * 2]) * (msi[j + spn * 2] - msi[i + spn * 2]) +
                    (msi[j + spn * 3] - msi[i + spn * 3]) * (msi[j + spn * 3] - msi[i + spn * 3]);
    float dist_s = (ms[j] - ms[i]) * (ms[j] - ms[i]) +
                   (ms[j + spn] - ms[i + spn]) * (ms[j + spn] - ms[i + spn]) +
                   (ms[j + spn * 2] - ms[i + spn * 2]) * (ms[j + spn * 2] - ms[i + spn * 2]);
    float dist_r = (mr[j] - mr[i]) * (mr[j] - mr[i]);
    float d_mt = (mt[j] * mt[i]) / (mt[j] + mt[i]);
    return d_mt * (dist_si + dist_r + dist_s * compactness);
}

/**
 * Fill an adjacency matrix
 * @param L matrix of labels
 * @param h height 
 * @param w width
 * @param spn number of labels (cluster)
 * @param adj matrix of adjacent labels to fill
 * */
void adjacency(cv::Mat_<int> L, int h, int w, int spn, cv::Mat_<int> adj)
{
    for (int i = 0; i < h - 1; i++)
    {
        for (int j = 0; j < w - 1; j++)
        {
            int lab = L(i * w + j);
            if (lab != L(i * w + j + 1))
            {
                adj(lab + L(i * w + j + 1) * spn) = 1;
                adj(lab * spn + L(i * w + j + 1)) = 1;
            }
            if (lab != L((i + 1) * w + j))
            {
                adj(lab + L((i + 1) * w + j) * spn) = 1;
                adj(lab * spn + L((i + 1) * w + j)) = 1;
            }
        }
    }
}

void Slic::createHierarchy(bool metrics[4])
{
    assert(metrics);
    int h = _rangeImage.getHeight();
    int w = _rangeImage.getWidth();
    if (w <= 0 || h <= 0)
    {
        std::cerr << "Failed to create hierarchy in Slic::createHierarchy, the range image is empty" << std::endl;
        exit(EXIT_FAILURE);
    }

    cv::Mat_<int> L = _clusters.clone().reshape(1, h * w);

    //_nbLabels = _nbLabels +1 ; // TODO : try to remove +1

    _labelVec.assign(_nbLabels, -1);
    float comp_factor = 100.0 / (h * w);
    float compactness = comp_factor * _nbLabels;

    cv::Mat_<int> living_sp = cv::Mat(1, _nbLabels, CV_32FC1, 1);

    std::vector<float> ms(_nbLabels * 3, 0); // spatial normalized
    std::vector<float> mt(_nbLabels * 1, 0); // count of pixel inside cluster
    std::vector<float> mr(_nbLabels * 1, 0); // remission

    std::vector<float> msi(_nbLabels * 4, 0); // spatial normalized and interpolated

    // initialized vectors
    const vector<float> *interpolatedData = _rangeImage.getNormalizedAndInterpolatedData();
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int pos = i * w + j;
            int lab = L(pos);

            riVertex normalizedData = _rangeImage.getNormalizedValue(pos);
            if (metrics[SLIC_METRIC_X])
            {
                msi[lab] += interpolatedData->at(pos * 4 + SLIC_METRIC_X);
            }
            if (metrics[SLIC_METRIC_Y])
            {
                msi[lab + _nbLabels] += interpolatedData->at(pos * 4 + SLIC_METRIC_Y);
            }
            if (metrics[SLIC_METRIC_Z])
            {
                msi[lab + _nbLabels * 2] += interpolatedData->at(pos * 4 + SLIC_METRIC_Z);
            }
            if (metrics[SLIC_METRIC_REMISSION])
            {
                msi[lab + _nbLabels * 3] += interpolatedData->at(pos * 4 + SLIC_METRIC_REMISSION);
            }

            ms[lab] += normalizedData.x;
            ms[lab + _nbLabels] += normalizedData.y;
            ms[lab + _nbLabels * 2] += normalizedData.z;
            mr[lab] += normalizedData.remission;
            mt[lab] += 1;
        }
    }
    // center of cluster (mean of each cluster)
    for (unsigned int lab = 0; lab < _nbLabels; lab++)
    {
        if (mt[lab] > 0)
        {
            ms[lab] /= mt[lab];
            ms[lab + _nbLabels] /= mt[lab];
            ms[lab + _nbLabels * 2] /= mt[lab];
            mr[lab] /= mt[lab];
            msi[lab] /= mt[lab];
            msi[lab + _nbLabels] /= mt[lab];
            msi[lab + _nbLabels * 2] /= mt[lab];
            msi[lab + _nbLabels * 3] /= mt[lab];
        }
        else
        {
            mt[lab] = 1;
        }
    }

    // matrix of distance between two clusters
    cv::Mat_<float> dist = cv::Mat(1, _nbLabels * _nbLabels, CV_32FC1, FLT_MAX);
    // matrix contain the adjacent labels of clusters
    cv::Mat_<int> adj = cv::Mat(1, _nbLabels * _nbLabels, CV_32FC1, cv::Scalar(0));

    adjacency(L, h, w, _nbLabels, adj);

    cv::Mat_<int> plhs_0 = cv::Mat(_nbLabels, _nbLabels, CV_32FC1);
    cv::Mat_<int> plhs_1 = cv::Mat(_nbLabels, _nbLabels, CV_32FC1);
    cv::Mat_<int> classif = plhs_0.reshape(1, _nbLabels * _nbLabels);
    for (unsigned int i = 0; i < _nbLabels; i++)
        classif(i * _nbLabels) = i;

    cv::Mat_<int> iter_classif = plhs_1.reshape(1, _nbLabels * _nbLabels);
    float dist_tmp, dmin;
    int l1 = 0, l2 = 0;
    dmin = FLT_MAX;
    // fill the distance matrix and save the minimum distance
    for (unsigned int i = 0; i < _nbLabels; i++)
    {
        for (unsigned int j = i + 1; j < _nbLabels; j++)
        {
            if (adj(i + _nbLabels * j))
            {

                dist_tmp = distance(msi, ms, mr, mt, i, j, _nbLabels, compactness);
                dist(j + i * _nbLabels) = dist_tmp;
                dist(j * _nbLabels + i) = dist_tmp;

                if (dist_tmp < dmin)
                {
                    dmin = dist_tmp;
                    l1 = j;
                    l2 = i;
                }
            }
        }
    }
    for (unsigned int i = 1; i < _nbLabels; i++)
    {
        //Classif merge and recopy l1 <- l2
        for (unsigned int j = 0; j < _nbLabels; j++)
        {
            if (classif((i - 1) + _nbLabels * j) == l1)
                classif(i + _nbLabels * j) = l2;
            else
                classif(i + _nbLabels * j) = classif((i - 1) + _nbLabels * j);
        }
        living_sp(l1) = 0;

        iter_classif(l1 * _nbLabels + l1) = i;
        iter_classif(l2 * _nbLabels + l1) = i;

        // Update adjacency matrix (can be optimized)
        for (unsigned int j = 0; j < _nbLabels; j++)
        {
            adj(l2 + j * _nbLabels) += adj(l1 + j * _nbLabels);
            adj(l2 * _nbLabels + j) += adj(l1 * _nbLabels + j);
            adj(l1 + j * _nbLabels) = 0;
            adj(l1 * _nbLabels + j) = 0;
            adj(l2 * _nbLabels + l2) = 0;
        }

        //Update mean metrics, remission, and spatial 3D barycenters
        ms[l2] = ms[l2] * mt[l2] + ms[l1] * mt[l1];
        ms[l2 + _nbLabels] = ms[l2 + _nbLabels] * mt[l2] + ms[l1 + _nbLabels] * mt[l1];
        ms[l2 + _nbLabels * 2] = ms[l2 + _nbLabels * 2] * mt[l2] + ms[l1 + _nbLabels * 2] * mt[l1];
        mr[l2] = mr[l2] * mt[l2] + mr[l1] * mt[l1];
        msi[l2] = msi[l2] * mt[l2] + msi[l1] * mt[l1];
        msi[l2 + _nbLabels] = msi[l2 + _nbLabels] * mt[l2] + msi[l1 + _nbLabels] * mt[l1];
        msi[l2 + _nbLabels * 2] = msi[l2 + _nbLabels * 2] * mt[l2] + msi[l1 + _nbLabels * 2] * mt[l1];
        msi[l2 + _nbLabels * 3] = msi[l2 + _nbLabels * 3] * mt[l2] + msi[l1 + _nbLabels * 3] * mt[l1];
        mt[l2] += mt[l1];
        //Normalization
        mr[l2] /= mt[l2];
        ms[l2] /= mt[l2];
        ms[l2 + _nbLabels] /= mt[l2];
        ms[l2 + _nbLabels * 2] /= mt[l2];
        msi[l2] /= mt[l2];
        msi[l2 + _nbLabels] /= mt[l2];
        msi[l2 + _nbLabels * 2] /= mt[l2];
        msi[l2 + _nbLabels * 3] /= mt[l2];

        //Recompute dist
        for (unsigned int j = 0; j < _nbLabels; j++)
        {
            if (adj(l2 + _nbLabels * j))
            {
                if (living_sp(j))
                {
                    dist_tmp = distance(msi, ms, mr, mt, l2, j, _nbLabels, comp_factor * (_nbLabels - i));
                    dist(j + l2 * _nbLabels) = dist_tmp;
                    dist(j * _nbLabels + l2) = dist_tmp;
                }
            }
            dist(l1 + j * _nbLabels) = FLT_MAX;
            dist(l1 * _nbLabels + j) = FLT_MAX;
        }

        dmin = FLT_MAX;
        for (unsigned int j = 0; j < _nbLabels; j++)
        {
            if (living_sp(j))
            {
                for (unsigned int p = j + 1; p < _nbLabels; p++)
                {
                    if (living_sp(p))
                    {
                        if (dist(j + p * _nbLabels) < dmin)
                        {
                            dmin = dist(j + p * _nbLabels);
                            l1 = j;
                            l2 = p;
                        }
                    }
                }
            }
        }
    }
    tree = plhs_0.clone().t();
    createSaliencyMap();
}

void Slic::createSaliencyMap()
{
    saliency = cv::Mat::zeros(_nbLabels, _nbLabels, CV_32FC1);
    for (unsigned int lvl = 0; lvl < _nbLabels - 10; lvl++)
    {
        for (unsigned int spx = 0; spx < _nbLabels; spx++)
        {
            saliency(lvl, tree(lvl, spx)) += 1;
        }
    }
    double min, max;
    cv::minMaxLoc(saliency, &min, &max);

    for (unsigned int lvl = 0; lvl < _nbLabels - 10; lvl++)
    {
        for (unsigned int spx = 0; spx < _nbLabels; spx++)
        {
            if (saliency(lvl, spx) != 0)
                saliency(lvl, spx) = (((saliency(lvl, spx) / -max) + 1) * 220);
        }
    }
}

void Slic::addObjectCluster(cv::Point2i pPos)
{
    int indexObject = labelOfPixel(pPos.y, pPos.x);
    if (find(obj.begin(), obj.end(), indexObject) == obj.end())
    {
        obj.push_back(indexObject);
    }
    auto index = find(bg.begin(), bg.end(), indexObject);
    if (index != bg.end())
        bg.erase(index);
}

void Slic::addBackgroundCluster(cv::Point2i pPos)
{
    int indexBackground = labelOfPixel(pPos.y, pPos.x);
    if (find(bg.begin(), bg.end(), indexBackground) == bg.end())
    {
        bg.push_back(indexBackground);
    }
    auto index = find(obj.begin(), obj.end(), indexBackground);
    if (index != obj.end())
        obj.erase(index);
}

void Slic::clearScribbleClusters()
{
    obj.clear();
    bg.clear();
}

void Slic::multiLabelisationConnected(int label)
{
    if (obj.empty() || bg.empty())
        return;
    int lvl, min;
    const riVertex *riData = _rangeImage.getData();
    int width = _rangeImage.getWidth();
    // loop through object and background vector and find the minimum tree level where both labels are equal
    for (size_t i = 0; i < obj.size(); i++)
    {
        min = INT_MAX;
        for (size_t j = 0; j < bg.size(); j++)
        {
            lvl = levelOfFusion(obj[i], bg[j]);
            if (lvl < min)
                min = lvl;
        }

        // go to the tree level according to elements in obj vector and add pixels which are not in selected clusters
        // update label vector with indicate label in parameter
        int indexObject = tree(min - 1, obj[i]);
        for (unsigned int spx = 0; spx < _nbLabels; spx++)
        {
            if (tree(min - 1, spx) == indexObject)
            {
                if (find(_selectedClusters.begin(), _selectedClusters.end(), spx) == _selectedClusters.end())
                {
                    _labelVec.at(spx) = label;
                    vector<pair<int, int>> pixels = pixelsOfSuperpixel(spx);
                    unsigned int size = pixels.size();
                    for (unsigned int j = 0; j < size; j++)
                    {
                        int index = pixels.at(j).first * width + pixels.at(j).second;
                        if (riData[index].remission != -1)
                            _rangeImage.setLabel(index, label);
                    }
                    _selectedClusters.push_back(spx);
                }
            }
        }
    }

    // loop through background and object vector and find the minimum tree level where both labels are equal
    for (size_t i = 0; i < bg.size(); i++)
    {
        min = INT_MAX;
        for (size_t j = 0; j < obj.size(); j++)
        {
            lvl = levelOfFusion(bg[i], obj[j]);
            if (lvl < min)
                min = lvl;
        }
        // go to the tree level according to elements in bg vector and remove pixels which are in selected clusters
        // update label vector with default label (-1)
        int indexBackground = tree(min - 1, bg[i]);
        for (unsigned int spx = 0; spx < _nbLabels; spx++)
        {
            if (tree(min - 1, spx) == indexBackground)
            {
                auto spxIterator = find(_selectedClusters.begin(), _selectedClusters.end(), spx);
                if (spxIterator != _selectedClusters.end())
                {
                    _labelVec.at(spx) = -1;
                    vector<pair<int, int>> pixels = pixelsOfSuperpixel(spx);
                    unsigned int size = pixels.size();
                    for (unsigned int j = 0; j < size; j++)
                    {
                        int index = pixels.at(j).first * width + pixels.at(j).second;
                        if (riData[index].remission != -1)
                            _rangeImage.setLabel(index, -1);
                    }
                    _selectedClusters.erase(spxIterator);
                }
            }
        }
    }
}

int Slic::levelOfFusion(int label1, int label2)
{
    for (unsigned int lvl = 0; lvl < _nbLabels; lvl++)
    {
        if (tree(lvl, label1) == tree(lvl, label2))
            return lvl;
    }
    return -1;
}
/*
 * Display the cluster centers.
 *
 * Input : The image to display upon (cv::Mat) and the colour (cv::Vec3b).
 * Output: -
 */
void Slic::displayCenterGrid(cv::Mat &pImage, cv::Scalar pColour)
{
    for (int i = 0; i < _centers.rows; i++)
    {
        cv::circle(pImage, cv::Point2d(_centers(i)[0], _centers(i)[1]), 2, pColour, 2);
    }
}

/*
 * Display a single pixel wide contour around the clusters.
 *
 * Input : The target image (cv::Mat) and contour colour (cv::Vec3b).
 * Output: -
 */
void Slic::displayContours(cv::Mat &pImage, cv::Vec3b pColour)
{
    const int dc8[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    const int dr8[8] = {0, -1, -1, -1, 0, 1, 1, 1};

    /* Initialize the contour vector and the matrix detailing whether a pixel
      * is already taken to be a contour. */
    vector<cv::Point> contours;
    cv::Mat_<uchar> istaken(pImage.rows, pImage.cols, uchar(0));

    cv::Mat_<int> clusters(pImage.cols, pImage.rows, -1);

    for (int col = 0; col < pImage.cols; col++)
    {
        for (int row = 0; row < pImage.rows; row++)
        {
            int lab = _clusters(row, col);
            clusters(col, row) = tree(treeLevel, lab);
        }
    }

    /* Go through all the pixels. */
    for (int col = 0; col < pImage.cols; col++)
    {
        for (int row = 0; row < pImage.rows; row++)
        {
            int nr_p = 0;

            /* Compare the pixel to its 8 neighbours. */
            for (int k = 0; k < 8; k++)
            {
                int c = col + dc8[k];
                int r = row + dr8[k];

                if (c >= 0 && c < pImage.cols && r >= 0 && r < pImage.rows)
                {
                    if (istaken(r, c) == false && tree(treeLevel, _clusters(row, col)) != tree(treeLevel, _clusters(r, c)))
                    {
                        nr_p += 1;
                    }
                }
            }

            /* Add the pixel to the contour list if desired. */
            if (nr_p >= 2)
            {
                contours.push_back(cv::Point(row, col));
                istaken(row, col) = true;
            }
        }
    }

    /* Draw the contour pixels. */
    for (int i = 0; i < (int)contours.size(); i++)
    {
        pImage.at<cv::Vec3b>(contours[i].x, contours[i].y) = pColour;
    }
}

void Slic::displaySaliency(cv::Mat &pImage)
{
    const int dc8[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    const int dr8[8] = {0, -1, -1, -1, 0, 1, 1, 1};

    /* Initialize the contour vector and the matrix detailing whether a pixel
       * is already taken to be a contour. */
    vector<cv::Point> contours;
    cv::Mat_<uchar> istaken(pImage.rows, pImage.cols, uchar(0));

    cv::Mat_<int> clusters(pImage.cols, pImage.rows, -1);

    for (int col = 0; col < pImage.cols; col++)
    {
        for (int row = 0; row < pImage.rows; row++)
        {
            int lab = _clusters(row, col);
            clusters(col, row) = tree(treeLevel, lab);
            // int a = tree(treeLevel, lab);
        }
    }

    /* Go through all the pixels. */
    for (int col = 0; col < pImage.cols; col++)
    {
        for (int row = 0; row < pImage.rows; row++)
        {
            int nr_p = 0;

            /* Compare the pixel to its 8 neighbours. */
            for (int k = 0; k < 8; k++)
            {
                int c = col + dc8[k];
                int r = row + dr8[k];

                if (c >= 0 && c < pImage.cols && r >= 0 && r < pImage.rows)
                {
                    if (istaken(r, c) == false && tree(treeLevel, _clusters(row, col)) != tree(treeLevel, _clusters(r, c)))
                    {
                        nr_p += 1;
                    }
                }
            }

            /* Add the pixel to the contour list if desired. */
            if (nr_p >= 2)
            {
                contours.push_back(cv::Point(row, col));
                istaken(row, col) = true;
            }
        }
    }

    /* Draw the contour pixels. */
    for (int i = 0; i < (int)contours.size(); i++)
    {
        int val = saliency(treeLevel, labelOfPixel(contours[i].x, contours[i].y));
        pImage.at<cv::Vec3b>(contours[i].x, contours[i].y) = cv::Vec3b(val, val, val);
    }
}

void Slic::displayMultipleSaliency(cv::Mat &pImage)
{
    int save = treeLevel;
    for (unsigned int lvl = 0; lvl < _nbLabels - 5; lvl += 5)
    {
        treeLevel = lvl;
        displaySaliency(pImage);
    }
    treeLevel = save;
}

/*
 * Give the pixels of each cluster the same colour values. The specified colour
 * is the mean RGB colour per cluster.
 *
 * Input : The target image (cv::Mat).
 * Output: -
 */
void Slic::colourWithClusterMeans(cv::Mat &pImage)
{
    vector<cv::Vec3i> colours(_centers.rows);
    /* fill */
    for (size_t i = 0; i < colours.size(); i++)
    {
        colours[i] = cv::Vec3i(0, 0, 0);
    }
    /* Gather the colour values per cluster. */
    for (int col = 0; col < pImage.cols; col++)
    {
        for (int row = 0; row < pImage.rows; row++)
        {
            int index = _clusters(row, col);
            cv::Vec3b c = pImage.at<cv::Vec3b>(row, col);
            colours[index][0] += (c[0]);
            colours[index][1] += (c[1]);
            colours[index][2] += (c[2]);
        }
    }

    /* Divide by the number of pixels per cluster to get the mean colour. */
    for (int i = 0; i < (int)colours.size(); i++)
    {
        colours[i] /= _centerCounts[i];
    }

    /* Fill in. */
    for (int col = 0; col < pImage.cols; col++)
    {
        for (int row = 0; row < pImage.rows; row++)
        {
            cv::Vec3i c = colours[_clusters(row, col)];
            pImage.at<cv::Vec3b>(row, col) = cv::Vec3b(c[0], c[1], c[2]);
        }
    }
}

/*
 * Display the selected clusters by reducing clusters intensity and changing their contours in red.
 *
 * Input : The target image (cv::Mat).
 * Output: -
 */
void Slic::displayClusters(cv::Mat &pImage)
{
    for (unsigned int index = 0; index < _selectedClusters.size(); ++index)
    {
        int label = _selectedClusters[index];

        for (unsigned int i = 0; i < _nbLabels; i++)
        {
            if (tree(treeLevel, i) == label)
            {
                for (unsigned int p = 0; p < _cls[i].size(); ++p)
                {
                    int row = _cls[i][p].first;
                    int col = _cls[i][p].second;
                    if (pImage.at<cv::Vec3b>(row, col) != cv::Vec3b(255, 0, 255))
                        pImage.at<cv::Vec3b>(row, col) = pImage.at<cv::Vec3b>(row, col) / 2.0f;
                    else
                        pImage.at<cv::Vec3b>(row, col) = cv::Vec3b(0, 0, 255);
                }
            }
        }
    }
}

/*
 * Display the selected clusters on pImage by filling them with pixels values of pImageRef.
 *
 * Input : The target image (cv::Mat) and the source image (cv::Mat).
 * Output: -
 */
void Slic::getAndDisplaySelection(cv::Mat &pImage, const cv::Mat pImageRef)
{
    for (unsigned int index = 0; index < _selectedClusters.size(); ++index)
    {
        int label = _selectedClusters[index];
        for (unsigned int i = 0; i < _nbLabels; i++)
        {
            if (tree(treeLevel, i) == label)
            {
                for (unsigned int p = 0; p < _cls[i].size(); ++p)
                {
                    int row = _cls[i][p].first;
                    int col = _cls[i][p].second;
                    pImage.at<cv::Vec3b>(row, col) = pImageRef.at<cv::Vec3b>(row, col);
                }
            }
        }
    }
}

cv::Mat Slic::displaySelection(cv::Mat backgroundImage, cv::Mat selectionImage)
{
    cv::Mat output = backgroundImage.clone();
    for (unsigned int index = 0; index < _selectedClusters.size(); ++index)
    {
        int label = _selectedClusters[index];
        for (unsigned int i = 0; i < _nbLabels; i++)
        {
            if (tree(treeLevel, i) == label)
            {
                cv::Vec3b color = getColorFromLabel(i);
                for (unsigned int p = 0; p < _cls[i].size(); ++p)
                {
                    int row = _cls[i][p].first;
                    int col = _cls[i][p].second;
                    // output.at<cv::Vec3b>(row, col) = selectionImage.at<cv::Vec3b>(row, col);
                    output.at<cv::Vec3b>(row, col) = color;
                }
            }
        }
    }
    return output;
}

cv::Mat Slic::displayGraySelection(cv::Mat pImage)
{
    cv::Mat output = pImage.clone();
    for (unsigned int index = 0; index < _selectedClusters.size(); ++index)
    {
        int label = _selectedClusters[index];
        for (unsigned int i = 0; i < _nbLabels; i++)
        {
            if (tree(treeLevel, i) == label)
            {
                for (unsigned int p = 0; p < _cls[i].size(); ++p)
                {
                    int row = _cls[i][p].first;
                    int col = _cls[i][p].second;
                    if (output.at<cv::Vec3b>(row, col) != colorContours)
                        output.at<cv::Vec3b>(row, col) = output.at<cv::Vec3b>(row, col) / 2.0f;
                    else
                    {
                        output.at<cv::Vec3b>(row, col) = colorSelection;
                    }
                }
            }
        }
    }
    return output;
}

/*
 * Add the superpixel containing the pixel pPos to the list of selected clusters if not yet done.
 *
 * Input : The pixel (cv::Point2i) clicked.
 * Output: 0 if the superpixel has been already selected, 1 if not.
 */
void Slic::selectCluster(cv::Point2i pPos, int label)
{
    int indexCluster = labelOfPixel(pPos.y, pPos.x);
    if (indexCluster == -1)
        return;

    const riVertex *riData = _rangeImage.getData();
    int width = _rangeImage.getWidth();
    for (unsigned int i = 0; i < _nbLabels; i++)
    {
        if (tree(treeLevel, i) == indexCluster)
        {
            _labelVec.at(i) = label;
            vector<pair<int, int>> pixels = pixelsOfSuperpixel(i);
            unsigned int size = pixels.size();
            for (unsigned int j = 0; j < size; j++)
            {
                int index = pixels.at(j).first * width + pixels.at(j).second;
                if (riData[index].remission != -1)
                    _rangeImage.setLabel(index, label);
            }

            if (find(_selectedClusters.begin(), _selectedClusters.end(), i) == _selectedClusters.end())
            {
                _selectedClusters.push_back(i);
            }
        }
    }
}

/*
 * Erase the superpixel containing the pixel pPos from the list of selected clusters if not yet done.
 *
 * Input : The pixel (cv::Point2i) clicked.
 * Output: 1 if the superpixel has been already deselected, 0 if not.
 */
void Slic::deselectCluster(cv::Point2i pPos)
{
    int indexCluster = labelOfPixel(pPos.y, pPos.x);
    if (indexCluster == -1)
        return;

    const riVertex *riData = _rangeImage.getData();
    int width = _rangeImage.getWidth();
    for (unsigned int i = 0; i < _nbLabels; i++)
    {
        if (tree(treeLevel, i) == indexCluster)
        {
            // prevent all spx in indexCluster to be fusionned
            for (int lvl = 1; lvl < getTreeLevel(); lvl++)
            {
                tree((_nbLabels - lvl), i) = indexCluster;
            }
            auto index = find(_selectedClusters.begin(), _selectedClusters.end(), i);
            if (index != _selectedClusters.end())
            {
                _labelVec.at(i) = -1;
                vector<pair<int, int>> pixels = pixelsOfSuperpixel(i);
                unsigned int size = pixels.size();
                for (unsigned int j = 0; j < size; j++)
                {
                    int index = pixels.at(j).first * width + pixels.at(j).second;
                    if (riData[index].remission != -1)
                        _rangeImage.setLabel(index, -1);
                }
                _selectedClusters.erase(index);
            }
        }
        //prevent all spx from being fusionned by current spx
        for (int lvl = getTreeLevel() - 1; lvl > 0; lvl--)
        {
            if (tree((_nbLabels - lvl), i) == indexCluster && tree((_nbLabels - lvl - 1), i) != indexCluster)
            {
                tree((_nbLabels - lvl), i) = tree((_nbLabels - lvl - 1), i);
            }
        }
    }
    // prevent only current spx to be fusionned
    // for (int lvl = 1; lvl < getTreeLevel(); lvl++){
    //     tree((_nbLabels - lvl),indexCluster) = indexCluster;
    // }
}

void Slic::clearSelectedClusters()
{
    int width = _rangeImage.getWidth();
    const riVertex *riData = _rangeImage.getData();
    for (uint i = 0; i < _labelVec.size(); i++)
    {
        _labelVec.at(i) = -1;
        vector<pair<int, int>> pixels = pixelsOfSuperpixel(i);
        unsigned int size = pixels.size();
        for (unsigned int j = 0; j < size; j++)
        {
            int index = pixels.at(j).first * width + pixels.at(j).second;
            if (riData[index].remission != -1)
                _rangeImage.setLabel(index, -1);
        }
    }
    _selectedClusters.clear();
}

/*
 * Fill in an image with white for pixels out of the selected region and with the pixel values in pImage for others.
 *
 * Input : The reference image (cv::Mat).
 * Output: The image (cv::Mat) representing the selected region of pImage.
 */
cv::Mat Slic::getRoiSelection(const cv::Mat pImage)
{
    cv::Mat imgToSave(pImage.rows, pImage.cols, CV_8UC3, cv::Scalar(255, 255, 255));
    getAndDisplaySelection(imgToSave, pImage);
    return imgToSave;
}

/*
 * Initialisation of _cls from _nbLabels and _clusters.
 *
 * Input : The size (int,int) of the image.
 * Output: -
 */
void Slic::initCls(int pRows, int pCols)
{
    vector<vector<pair<int, int>>> new_cls(_nbLabels);
    _cls = new_cls;
    vector<pair<int, int>> tmp;
    for (unsigned int i = 0; i < _nbLabels; ++i)
    {
        _cls[i] = tmp;
    }
    for (int col = 0; col < pCols; ++col)
    {
        for (int row = 0; row < pRows; ++row)
        {
            int label = _clusters(row, col);
            if (label != -1)
            {
                _cls[label].push_back(make_pair(row, col));
            }
        }
    }
}

/*
 * Adapt selection of new segmentation from the current one (ie pOldSlic selection).
 *
 * Input : The slic (Slic) which contains the selected clusters to spread.
 * Output: -
 */
void Slic::spreadSelection()
{
    for (size_t spx_label = 0; spx_label < _cls.size(); ++spx_label)
    {
        size_t count_pixel = 0;
        /* Count the number of pixels previously selected for one superpixel */
        for (size_t px = 0; px < _cls[spx_label].size(); ++px)
        {
            pair<int, int> coords = _cls[spx_label][px];
            int label = _clusters(coords.first, coords.second);
            vector<int> osc = _selectedClusters;
            if (find(osc.begin(), osc.end(), label) != osc.end())
            {
                ++count_pixel;
            }
        }
        if (count_pixel >= _cls[spx_label].size() / 2)
        {
            if (find(_selectedClusters.begin(), _selectedClusters.end(), spx_label) == _selectedClusters.end())
                _selectedClusters.push_back(spx_label);
        }
    }
}

/*
 * Search for the neighbours of a superpixel given its label.
 *
 * Input : The label (int) of the superpixel to process.
 * Output: The neighbours superpixels labels (vector<int>) of the superpixel to process.
 */
vector<int> Slic::findLabelNeighbours(size_t pLabelSpx)
{
    vector<int> neighLabels;
    const int dc4[4] = {-1, 0, 1, 0};
    const int dr4[4] = {0, -1, 0, 1};
    if (pLabelSpx < 0)
    {
        std::cerr << "There is no superpixel with negative label." << std::endl;
        return neighLabels;
    }
    else if (pLabelSpx >= _cls.size())
    {
        std::cerr << _cls.size() << std::endl;
        std::cerr << "The label " << pLabelSpx << " does not belong to the list." << std::endl;
        return neighLabels;
    }
    for (size_t px = 0; px < _cls[pLabelSpx].size(); ++px)
    {
        pair<int, int> coords = _cls[pLabelSpx][px];

        /* loop over neighbours */
        for (int k = 0; k < 4; k++)
        {
            int r = coords.first + dr4[k];
            int c = coords.second + dc4[k];
            if (r >= 0 && r < _clusters.rows && c >= 0 && c < _clusters.cols)
            {
                if ((size_t)_clusters(r, c) != pLabelSpx && find(neighLabels.begin(), neighLabels.end(), _clusters(r, c)) == neighLabels.end())
                {
                    neighLabels.push_back(_clusters(r, c));
                }
            }
        }
    }
    return neighLabels;
}

int Slic::labelOfPixel(int pRow, int pCol)
{
    pRow = min(_clusters.rows - 1, pRow);
    pCol = min(_clusters.cols - 1, pCol);

    if (pRow < 0)
    {
        throw out_of_range("x coordinate should not be negative");
    }
    if (pCol < 0)
    {
        throw out_of_range("y coordinate should not be negative");
    }
    if (pRow >= _clusters.rows)
    {
        throw out_of_range("x coordinate should not exceed the width of the image");
    }
    if (pCol >= _clusters.cols)
    {
        throw out_of_range("y coordinate should not exceed the height of the image");
    }
    return tree(treeLevel, _clusters(pRow, pCol));
}

const cv::Mat_<int> Slic::getClusters() const
{
    return _clusters;
}

void Slic::setClusters(const cv::Mat_<int> pClusters)
{
    _clusters = pClusters;
}

const vector<int> Slic::selectedClusters() const
{
    return _selectedClusters;
}

const vector<pair<int, int>> Slic::pixelsOfSuperpixel(int pLabel) const
{
    if (pLabel < 0)
    {
        throw out_of_range("No superpixel has negative label in Slic::pixelsOfSuperpixel");
    }
    if (pLabel >= (int)_cls.size())
    {
        throw out_of_range("There is not that much superpixels in Slic::pixelsOfSuperpixel");
    }
    vector<pair<int, int>> pixels;
    for (unsigned int i = 0; i < _nbLabels; i++)
    {
        if (tree(treeLevel, i) == pLabel)
        {
            for (unsigned int p = 0; p < _cls[i].size(); ++p)
            {
                pixels.push_back(_cls[i][p]);
            }
        }
    }
    return pixels;
}

const vector<vector<pair<int, int>>> Slic::getCls() const
{
    return _cls;
}

void Slic::setCls(vector<vector<pair<int, int>>> pCls)
{
    if (pCls.size() == 0)
    {
        throw runtime_error("The new structure cls should not be empty");
    }
    _cls = pCls;
    _nbLabels = pCls.size();
}

int Slic::getTreeLevel()
{
    return _nbLabels - treeLevel;
}

void Slic::setTreeLevel(unsigned int pLevel)
{
    if (_nbLabels >= pLevel)
        treeLevel = _nbLabels - pLevel;
    else
        treeLevel = 0;
}

void Slic::zoomInTree()
{
    _zoomHist.push_back(treeLevel);
    treeLevel = max(0, treeLevel / 2);
}

void Slic::zoomOutTree()
{
    treeLevel = _zoomHist.back();
    _zoomHist.pop_back();
}

cv::Vec3b Slic::getColorFromLabel(int index)
{
    cv::Vec3b color;
    int label = _labelVec.at(index);
    switch (label)
    {
    case SLIC_LABEL_GROUND:
        color = cv::Vec3b(168, 127, 173);
        break;
    case SLIC_LABEL_STUCTURE:
        color = cv::Vec3b(78, 178, 185);
        break;
    case SLIC_LABEL_VEHICLE:
        color = cv::Vec3b(205, 178, 98);
        break;
    case SLIC_LABEL_NATURE:
        color = cv::Vec3b(109, 167, 96);
        break;
    case SLIC_LABEL_HUMAN:
        color = cv::Vec3b(0, 0, 255);
        break;
    case SLIC_LABEL_OBJECT:
        color = cv::Vec3b(50, 88, 140);
        break;
    case SLIC_LABEL_OUTLIER:
        color = cv::Vec3b(128, 128, 128);
        break;
    default:
        color = cv::Vec3b(255, 255, 255);
        break;
    }
    return color;
}
