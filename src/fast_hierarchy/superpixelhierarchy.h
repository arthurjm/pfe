#ifndef SUPERPIXELHIERARCHY_H
#define SUPERPIXELHIERARCHY_H

#include <stdexcept>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <float.h>
#include <QColor>
using namespace std;

#include <opencv2/opencv.hpp>
#include "rangeimage.h"

class SuperpixelHierarchy {
public:
    /* Class constructors and deconstructors. */
    SuperpixelHierarchy();

    cv::Mat buildHierarchy(const cv::Mat &pImage, const cv::Mat &pContours, int pNbSpx, int pWeight, bool buildScribbleLevels, const riVertex* riData);

    void getSuperpixels(int *parent, int *label, int *treeu, int *treev, int &nvertex, int &nregion, int &N);

    void getLevels(int* parent, int* treeu, int* treev, int &nvertex, int &nregion);
    void binaryLabelisationConnected();
    int levelOfFusion(int label1, int label2);
    void addObjectCluster(cv::Point2i pPos);
    void addBackgroundCluster(cv::Point2i pPos);
    void initPic();
    cv::Mat displaySelection(cv::Mat backgroundImage, cv::Mat selectionImage);
    cv::Mat displayGreySelection(cv::Mat pImage);

    cv::Mat displayContours(cv::Mat pImage);
    void displaySaliency(cv::Mat &pImage, int n);
    cv::Mat displayAllSaliency(cv::Mat pImage);


    void selectCluster(cv::Point2i pPos);
    void deselectCluster(cv::Point2i pPos);

    void clearSelectedClusters() {_selectedClusters.clear(); }
    void clearScribbleClusters(){ obj.clear(); bg.clear(); }
    int nbClusters(){ return _nbClusters; }
    int getCurrentLevel(){ return currentLevel; }
    int getCurrentWeight(){return connect; }
    int labelOfPixel(cv::Point2i pPos);

    int zoomInHierarchy(int pMaxLevel);
    int zoomOutHierarchy();

private:
    cv::Mat_<int> _hierarchyClusters, _hierarchyPixels, _currentHierarchy;

    int h, w, currentLevel;
    vector<int> levels;
    vector<int> _selectedClusters;

    vector<int> obj;
    vector<int> bg;
    cv::Mat clustersImage, img, contours;

    int connect;//Edge_weight: balance between edge and color feature (default: 4);
    int iterSwitch = 4; //Compactness value (default 4);
    int _nbClusters = 4000;

    cv::Vec3b colorContours = cv::Vec3b(255,0,255); // default ; pink 255,0,255
    cv::Vec3b colorSelection = cv::Vec3b(0,0,255); // default ; red 0,0,255

    vector<vector<pair<int,int>>> _pixelsInCluster; //pic
};

#endif // SUPERPIXELHIERARCHY_H
