#ifndef SLIC_H
#define SLIC_H

/* slic.h.
 *
 * Written by: Pascal Mettes.
 *
 * This file contains the class elements of the class Slic. This class is an
 * implementation of the SLIC Superpixel algorithm by Achanta et al. [PAMI'12,
 * vol. 34, num. 11, pp. 2274-2282].
 *
 * This implementation is created for the specific purpose of creating
 * over-segmentations in an OpenCV-based environment.
 */

#include <stdexcept>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <float.h>
#include <QColor>
using namespace std;

#include <opencv2/opencv.hpp>

/* The number of iterations run by the clustering algorithm. */
#define NR_ITERATIONS 10

typedef cv::Vec<float, 5> Vec5f;

/*
 * class Slic.
 *
 * In this class, an over-segmentation is created of an image, provided by the
 * step-size (distance between initial cluster locations) and the colour
 * distance parameter.
 */
class Slic {
public:
    /* Class constructors and deconstructors. */
    Slic();
    Slic(Slic* pSlic);
    ~Slic();

    /* Generate an over-segmentation for an image. */
    void generateSuperpixels(const cv::Mat &pImage, int pNbSpx, int pNc);
    /* Enforce connectivity for an image. */
    void createConnectivity(const cv::Mat &pImage);
    void createHierarchy(const cv::Mat &pImage);

    /* Draw functions. Resp. displayal of the centers, the contours and the selected clusters. */
    void displayCenterGrid(cv::Mat &pImage, cv::Scalar pColour);
    void displayContours(cv::Mat &pImage, cv::Vec3b pColour);
    void colourWithClusterMeans(cv::Mat &pImage);

    /*
     *
     *  Following functions are the one we added to the basic code
     *
     */

    /* Draw functions. Displayal of the selected clusters. */
    void binaryLabelisation(int pMode);
    void binaryLabelisationTree();
    void binaryLabelisationConnected();
    int levelOfFusion(int label1, int label2);

    void createSaliencyMap();
    void displaySaliency(cv::Mat &pImage);
    void displayMultipleSaliency(cv::Mat &pImage);
    void displayClusters(cv::Mat &pImage);
    void getAndDisplaySelection(cv::Mat &pImage, const cv::Mat pImageRef);

    /* Functions relative to the selection/deselection of superpixels */
    void selectCluster(cv::Point2i pPos);
    void deselectCluster(cv::Point2i pPos);

    void addObjectCluster(cv::Point2i pPos);
    void addBackgroundCluster(cv::Point2i pPos);
    void clearScribbleClusters();

    void splitSuperpixel(int indexCluster);
    void clearSelectedClusters();

    /* Return the selected region */
    cv::Mat getRoiSelection(const cv::Mat pImage);
    /* Adapt selection of new segmentation from the current one */
    void spreadSelection();
    /* Return the list of superpixels labels neighbours of a superpixel given by its label */
    vector<int> findLabelNeighbours(int pLabelSpx);

    /* Data access and attribution functions */
    unsigned int nbLabels(){return _nbLabels;}
    const cv::Mat_<int> getClusters() const;
    const vector<int> selectedClusters() const;
    const vector<vector<pair<int,int>>> getCls() const;
    const vector<pair<int,int>> pixelsOfSuperpixel(int pLabel) const;
    int labelOfPixel(int pRow, int pCol);
    void setClusters(const cv::Mat_<int> pClusters);
    void setCls(vector<vector<pair<int,int>>> pCls);

    int getTreeLevel();
    void setTreeLevel(int pZoom);
    void zoomInTree();
    void zoomOutTree();


private:
    /* Compute the distance between a center and an individual pixel. */
    float computeDist(int pCi, cv::Point pPixel, cv::Vec3b pColour);
    /* Find the pixel with the lowest gradient in a 3x3 surrounding. */
    cv::Point findLocalMinimum(const cv::Mat_<cv::Vec3b> &pImage, cv::Point pCenter);

    /* Remove and initialize the 2d vectors. */
    void clearData();
    void initData(const cv::Mat &pImage);
    /* Added */
    void initCls(int pCols, int pRows);


    /* The step size per cluster, and the colour (nc) and distance (ns)
     * parameters. */
    int _step, _nc, _ns = 500;
    unsigned int _nbLabels = 0;

    /* The cluster assignments and distance values for each pixel. */
    cv::Mat_<int> _clusters;
    cv::Mat_<float> _distances;

    /* The LAB and xy values of the centers. */
    cv::Mat_<Vec5f> _centers;

    /* The number of occurences of each center. */
    vector<int> _centerCounts;

    /* The label of each selected cluster - Added */
    vector<int> _selectedClusters;
    /* The pixels contained in a superpixel for each superpixel. - Added */
    vector<vector<pair<int,int>>> _cls;

    cv::Mat_<int> tree;
    cv::Mat_<int> saliency;

    vector<int> obj;
    vector<int> bg;

    int treeLevel;

};

#endif
