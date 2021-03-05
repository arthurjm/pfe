#include "slic.h"
#include "rangeimage.h"
#include <QFileDialog>
#include <QApplication>
#include <stdlib.h>
#include <string.h>

using namespace std;

map<pair<int, int>, vector<float>> _result;

float computeAccuracy(Slic slic, const riVertex *riData, int nbSpx, int width)
{
    // nbLabels == number of clusters in the SLIC Algortithm
    unsigned int nbLabel = slic.nbLabels() - 1;
    unsigned int nbCluster = slic.getTreeLevel() - 1;
    float sum = 0;
    for (unsigned int i = 0; i < nbLabel; i++)
    {
        // pixels from one superpixel
        vector<pair<int, int>> pixels = slic.pixelsOfSuperpixel(i);
        unsigned int size = pixels.size();
        // first indicate the label, second indicate the number it appears
        vector<pair<int, int>> labelCount;
        for (unsigned int j = 0; j < size; j++)
        {
            int index = pixels.at(j).first * width + pixels.at(j).second;
            // get the label from ground truth
            int label = riData[index].label;
            int k;
            // if label was present in label increment the count for the label
            for (k = 0; k < labelCount.size(); k++)
            {
                if (labelCount.at(k).first == label)
                {
                    labelCount.at(k).second++;
                    break;
                }
            }
            // else add new label to labelCount
            if (k == labelCount.size())
                labelCount.push_back(pair<int, int>(label, 1));
        }

        // research of the label with the maximum apparitions and save the number it appears
        int max = -1;
        for (int k = 0; k < labelCount.size(); k++)
        {
            if (labelCount.at(k).second > max)
                max = labelCount.at(k).second;
        }
        if (size != 0)
            sum += max / (float)size;
    }
    // average accuracy
    float accuracy = sum / nbCluster;
    return accuracy;
}

void benchmark(string pathPointCloud, string pathLabel, bool mode, vector<int> nbSpxVec)
{
    // The range image contains the ground truth data on label.
    RangeImage ri(pathPointCloud, pathLabel);
    Slic slic;

    // create a range image with canal Y of the range image with an equalization of histogram and an interpolation on the "dead" pixels (remission == -1)
    cv::Mat img = ri.createColorMat({RI_Y}, false, true, false, true);
    const riVertex *riData = ri.getData();

    cv::Mat imgTmp = img.clone();
    cv::Mat labImage = imgTmp;
    // cvtColor(imgTmp, labImage, cv::COLOR_BGR2Lab);

    int width = img.cols;

    // Make sure that first element is the largest
    std::sort(nbSpxVec.begin(), nbSpxVec.end(), std::greater<int>());
    vector<int> weightVec({20, 10, 5});
    cout << "Benchmark on : " << pathPointCloud << endl;
    for (int i = 0; i < weightVec.size(); i++)
    {
        // Generation of SLIC Super Pixel
        if (mode)
        {
            slic.generateSuperpixels(labImage, nbSpxVec[0], weightVec[i], ri);
            slic.createConnectivity(labImage);
            slic.createHierarchy(labImage);
        }

        for (int j = 0; j < nbSpxVec.size(); j++)
        {
            if (!mode)
            {
                slic.generateSuperpixels(labImage, nbSpxVec[j], weightVec[i], ri);
                slic.createConnectivity(labImage);
                slic.createHierarchy(labImage);
            }
            pair<int, int> key(nbSpxVec.at(j), weightVec.at(i));
            slic.setTreeLevel(nbSpxVec[j]);
            float accuracy = computeAccuracy(slic, riData, nbSpxVec[j], width);
            _result[key].push_back(accuracy);
        }
    }
}

string getFileName(string path)
{
    int found = path.find_last_of("/");
    string fileName = path.substr(found + 1);
    found = fileName.find_last_of(".");
    fileName = fileName.substr(0, found);

    return fileName;
}

/**
 * First input parameter indicates which mode to use :
 * - 0 : mode hierarchy where number of superpixels is reduced by SLIC Hierarchy Tree
 * - 1 or other integer : mode re-segmentation where number of superpixels is regenerate by SLIC 
 * 
 * Rest parameters are the number of superpixels to be tested.
 * 
 * Ex : ./benchmark 0 1200 800
 * */
int main(int argc, char **argv)
{
    // Verification on input parameters
    bool modeHierarchy = true; // true for reducing by hierarchy, false for reducing by re-segmentation
    vector<int> nbSpxVec;
    if (argc < 2)
    {
        cout << "default settings : Mode hierarchy : reduce the nbSpx using SLIC hierarchy, number of super pixel = 800 400 200" << endl;
    }
    else if (argc < 3)
    {
        cout << "default settings : number of super pixel = 800 400 200" << endl;
        modeHierarchy = atoi(argv[1]) == 0;
        if (modeHierarchy)
            cout << "using mode hierarchy" << endl;
        else
            cout << "using mode re-segmentation" << endl;
    }
    else
    {
        modeHierarchy = atoi(argv[1]) == 0;
        if (modeHierarchy)
            cout << "using mode hierarchy" << endl;
        else
            cout << "using mode re-segmentation" << endl;

        cout << "number of super pixel =";
        for (int i = 2; i < argc; i++)
        {
            int spx = atoi(argv[i]);
            if (spx <= 0)
            {
                cerr << "number of super pixel invalid, need to be strict positive (nbsp>0)" << endl;
                exit(EXIT_FAILURE);
            }
            nbSpxVec.push_back(spx);
            cout << " " << spx;
        }
        cout << endl;
    }
    if (nbSpxVec.size() == 0)
    {
        nbSpxVec.push_back(1200);
        nbSpxVec.push_back(800);
    }

    QApplication a(argc, argv);
    cout << "Please select a PointCloud (.bin) file you want to benchmark with." << endl;
    QStringList listOfFileNamePointCloud = QFileDialog::getOpenFileNames(nullptr, "Open a point cloud file", QString("../data/pointclouds_kitti"), "Binary file (*.bin)");
    cout << "Please select the Label (.label) file that match the previous PointCloud." << endl;
    QStringList listOfFileNameLabel = QFileDialog::getOpenFileNames(nullptr, "Open a label file", QString("../data/label"), "Binary file (*.label)");

    vector<string> pointClouds;
    pointClouds.reserve(listOfFileNamePointCloud.size());

    vector<string> labels;
    labels.reserve(listOfFileNameLabel.size());

    for (const auto &i : listOfFileNamePointCloud)
    {
        pointClouds.push_back(i.toStdString());
    }
    for (const auto &i : listOfFileNameLabel)
    {
        labels.push_back(i.toStdString());
    }

    int pointCloudsSize = pointClouds.size();
    int labelsSize = labels.size();
    for (int i = 0; i < pointCloudsSize; i++)
    {
        string pointCloud = getFileName(pointClouds.at(i));
        for (int j = 0; j < labelsSize; j++)
        {
            string label = getFileName(labels.at(j));
            if (pointCloud == label)
            {
                benchmark(pointClouds.at(i), labels.at(j), modeHierarchy, nbSpxVec);
            }
        }
    }
    // Compute the benchmark average accuracy
    for (map<pair<int, int>, vector<float>>::iterator it = _result.begin(); it != _result.end(); it++)
    {
        int vecSize = it->second.size();
        float sum = 0;
        for (int i = 0; i < vecSize; i++)
        {
            sum += it->second.at(i);
        }
        sum /= vecSize;
        cout << "nbspx = " << it->first.first << " | weight = " << it->first.second << " | accuracy = " << sum * 100 << "%" << endl;
    }

    return 0;
}
