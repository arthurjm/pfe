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
#include "rangeimage.h"
using namespace std;

#include <opencv2/opencv.hpp>

/* The number of iterations run by the clustering algorithm. */
#define NR_ITERATIONS 10

#define SLIC_LABEL_OUTLIER 1
#define SLIC_LABEL_VEHICLE 10
#define SLIC_LABEL_HUMAN 30
#define SLIC_LABEL_GROUND 40
#define SLIC_LABEL_STUCTURE 50
#define SLIC_LABEL_NATURE 70
#define SLIC_LABEL_OBJECT 80

#define SLIC_METRIC_X 0
#define SLIC_METRIC_Y 1
#define SLIC_METRIC_Z 2
#define SLIC_METRIC_REMISSION 3

typedef cv::Vec<float, 2> Vec2f;
typedef cv::Vec<float, 4> Vec4f;

/*
 * class Slic.
 *
 * In this class, an over-segmentation is created of an image, provided by the
 * step-size (distance between initial cluster locations) and the colour
 * distance parameter.
 */
class Slic
{
public:
    /* Class constructors and deconstructors. */
    /**
     * Default constructor, nothing is made here 
     * */
    Slic() {}

    /**
     * Copy constructor
     * copy the reference ofselected_clusters and clusters parameters. 
     * The other parameters are neither copied nor initialized.
     * @param pSlic reference to Slic object 
     * */
    Slic(Slic &pSlic) : _clusters(pSlic.getClusters()), _selectedClusters(pSlic.selectedClusters()) {}

    /**
     * Destructor
     * */
    ~Slic() {}

    /**
     * Generate an over-segmentation for an image.
     * Compute the over-segmentation based on the step-size and relative weighting of the pixel and colour values.
     * @param nbSpx the number of superpixels
     * @param ri the range image
     * @param metrics a boolean array indicate which metric should be applied
     **/
    void generateSuperpixels(int nbSpx, RangeImage &ri, bool *metrics);

    /**
     * Enforce connectivity of the superpixels. This part is not actively discussed
     * in the paper, but forms an active part of the implementation of the authors
     * of the paper.
     * The initialisation of _cls is made at the end of the function.
     * */
    void createConnectivity();

    /**
     * Computes a hierarchical segmentation matrix of size (nb of labels x nb of labels)
     * where each row of the matrix stores the label of the generated superpixels.
     * @param metrics array contains metrics to be used 
     * */
    void createHierarchy(bool *metrics);

    /**
     * Create saliency map of the hierarchy, which weights each edge of the graph by the ultrametric distance between its extremities
     * */
    void createSaliencyMap();

    /* Draw functions. Resp. display of the centers, the contours and the selected clusters. */

    /**
     * Display the cluster centers.
     * @param pImage the image to display upon (cv::Mat)  
     * @param pColour the colour (cv::Vec3b)
     * */
    void displayCenterGrid(cv::Mat &pImage, cv::Scalar pColour);

    /**
     * Display a single pixel wide contour around the clusters.
     * @param pImage  the target image (cv::Mat) 
     * @param pColour the contour colour (cv::Vec3b)
     * */
    void displayContours(cv::Mat &pImage, cv::Vec3b pColour);

    /**
     * Give the pixels of each cluster the same colour values. The specified colour
     * is the mean RGB colour per cluster.
     * @param pImage The target image (cv::Mat).
     * */
    void colourWithClusterMeans(cv::Mat &pImage);

    /* Draw functions. Displayal of the selected clusters. */
    /**
     * Find the minium tree level according obj and bg vector,
     * go to the minimum tree level and update the selected cluster according to obj and bg vector
     * label vector is also updated at the same time.
     * @param label indicate the label to be applied on a cluster
     * */
    void multiLabelisationConnected(int label);

    /**
     * Find the treelevel where label1 and label2 are equal
     * @param label1 first label to be compared with
     * @param label2 second label to be compared with
     * @return the treelevel where both labels are equal
     * */
    int levelOfFusion(int label1, int label2);

    /**
     * @param pImage
     * */
    void displaySaliency(cv::Mat &pImage);
    /**
     * 
     * @param pImage
     * */
    void displayMultipleSaliency(cv::Mat &pImage);
    /**
     * Display the selected clusters by reducing clusters intensity and changing their contours in red.
     * @param pImage The target image (cv::Mat).
     * */
    void displayClusters(cv::Mat &pImage);

    /**
     * 
     * @param backgroundImage
     * @param selectionImage
     * */
    cv::Mat displaySelection(cv::Mat backgroundImage, cv::Mat selectionImage);

    /**
     * 
     * @param pImage
     * */
    cv::Mat displayGraySelection(cv::Mat pImage);

    /**
     * Display the selected clusters on pImage by filling them with pixels values of pImageRef.
     * @param pImage the target image (cv::Mat)  
     * @param pImageRef the source image (cv::Mat)
     * */
    void getAndDisplaySelection(cv::Mat &pImage, const cv::Mat pImageRef);

    /* Functions relative to the selection/deselection of superpixels */
    /**
     * Add the superpixel containing the pixel pPos to the list of selected clusters if not yet done.
     * @param pPos clicked position 
     * @param label current label
     * */
    void selectCluster(cv::Point2i pPos, int label);
    /**
     * Erase the superpixel containing the pixel pPos from the list of selected clusters if not yet done.
     * @param pPos The pixel clicked.
     * */
    void deselectCluster(cv::Point2i pPos);

    /**
     * Add selected SuperPixel (cluster) at the position pPos in the obj (object) vector and remove it from the bg (background) vector
     * @param pPos position of pixel selected
     * */
    void addObjectCluster(cv::Point2i pPos);
    /**
     * Add selected SuperPixel (cluster) at the position pPos in the bg (background) vector and remove it from the obj (object) vector
     * @param pPos position of pixel selected
     * */
    void addBackgroundCluster(cv::Point2i pPos);

    /**
     * clear the object and background vectors
     * */
    void clearScribbleClusters();

    /**
     * 
     * @param indexCluster
     * */
    void splitSuperpixel(int indexCluster);
    /**
     * 
     * */
    void clearSelectedClusters();

    /* Return the selected region */
    /**
     *  Fill in an image with white for pixels out of the selected region and with the pixel values in pImage for others.
     * @param pImage The reference image (cv::Mat).
     * @return The image (cv::Mat) representing the selected region of pImage.
     * */
    cv::Mat getRoiSelection(const cv::Mat pImage);
    /* Adapt selection of new segmentation from the current one */

    /**
     * Adapt selection of new segmentation from the current one (ie pOldSlic selection).
     * */
    void spreadSelection();
    /* Return the list of superpixels labels neighbours of a superpixel given by its label */
    /**
     * Search for the neighbours of a superpixel given its label.
     * @param pLabelSpx  The label (int) of the superpixel to process.
     * @return The neighbours superpixels labels (vector<int>) of the superpixel to process.
     * */
    vector<int> findLabelNeighbours(size_t pLabelSpx);

    /* Data access and attribution functions */
    /**
     * 
     * */
    unsigned int nbLabels() { return _nbLabels; }
    /**
     * 
     * */
    const cv::Mat_<int> getClusters() const;
    /**
     * 
     * */
    const vector<int> selectedClusters() const;
    /**
     * 
     * */
    const vector<vector<pair<int, int>>> getCls() const;
    /**
     * 
     * @param pLabel
     * */
    const vector<pair<int, int>> pixelsOfSuperpixel(int pLabel) const;
    /**
     * 
     * @param pRow
     * @param pCol
     * */
    int labelOfPixel(int pRow, int pCol);
    /**
     * 
     * @param pClusters
     * */
    void setClusters(const cv::Mat_<int> pClusters);
    /**
     * 
     * @param pCls
     * */
    void setCls(vector<vector<pair<int, int>>> pCls);

    /**
     * 
     * */
    int getTreeLevel();
    /**
     * 
     * @param pZoom
     * */
    void setTreeLevel(unsigned int pZoom);
    /**
     * 
     * */
    void zoomInTree();
    /**
     * 
     * */
    void zoomOutTree();
    /**
     * 
     * @param label
     * */
    cv::Vec3b getColorFromLabel(int label);

    /**
     * 
     * */
    const vector<int> getLabelVec() { return _labelVec; };

private:
    /** 
    * Compute the distance between a center and an individual pixel.
    * @param clusterIndex index of superpixel
    * @param pixel a point to compute distance with
    * @param metrics a boolean array indicate which data should be taking into account for compute distance
    * */
    float computeDist(int clusterIndex, cv::Point pixel, bool *metrics);

    /**
     * Find a local 3D gradient minimum of a pixel in a 3x3 neighbourhood. This
     * method is called upon initialization of the cluster centers.
     * @param pCenter the center pixel
     * */
    cv::Point findLocalMinimum(cv::Point pCenter);

    /**
     * Clear attributs vectors and release cv::Mat
     * */
    void clearData();

    /**
    * Initialize the cluster centers and initial values of the pixel-wise cluster
    * assignment and distance values.
    * Initialize normailzed and interpolated data from range image,
    * they will be used for distance computation.
     * @param pImage the image (cv::Mat).
     * */
    void initData();
    /**
     * Initialisation of _cls from _nbLabels and _clusters.
     * @param pCols image width
     * @param pRows image height
     * */
    void initCls(int pCols, int pRows);

    /* The step size per cluster, and the colour (nc) and distance (ns)
     * parameters. */
    int _step, _nc;
    unsigned int _nbLabels = 0;

    /* The cluster assignments and distance values for each pixel. */
    cv::Mat_<int> _clusters;
    cv::Mat_<float> _distances;

    /* The xy values of the centers. */
    cv::Mat_<Vec2f> _centers;
    /* the normalized data from range image (x,y,z,remission)*/
    cv::Mat_<Vec4f> _centersNormalized4D;
    /* The normalized and interpolated data from range image (x,y,z,remission) */
    cv::Mat_<Vec4f> _centersInterpolated4D;

    /* The number of occurences of each center. */
    vector<int> _centerCounts;

    /* The label of each selected cluster - Added */
    vector<int> _selectedClusters;
    /* The pixels contained in a superpixel for each superpixel. - Added */
    vector<vector<pair<int, int>>> _cls;

    cv::Mat_<int> tree;
    cv::Mat_<int> saliency;

    vector<int> obj;
    vector<int> bg;

    int treeLevel = 0;

    cv::Vec3b colorContours = cv::Vec3b(0, 0, 0);    // default ; black
    cv::Vec3b colorSelection = cv::Vec3b(0, 0, 255); // default ; red 0,0,255

    vector<int> _zoomHist;
    vector<int> _labelVec; // Classes

    RangeImage _rangeImage;
};

#endif
