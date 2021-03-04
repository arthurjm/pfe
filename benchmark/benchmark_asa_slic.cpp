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

void benchmark(string pathPointCloud, string pathLabel)
{
    // The range image contains the ground truth data on label.
    RangeImage ri(pathPointCloud, pathLabel);
    Slic slic;

    // create a range image with canal Y of the range image with an equalization of histogram and an interpolation on the "dead" pixels (remission == -1)
    cv::Mat img = ri.createColorMat({RI_Y}, false, true, false, true);
    const riVertex *riData = ri.getData();

    cv::Mat imgTmp = img.clone();
    cv::Mat labImage;
    cvtColor(imgTmp, labImage, cv::COLOR_BGR2Lab);

    int width = img.cols;

    vector<int> nbSpxVec({800});
    // Make sure that first element is the largest
    std::sort(nbSpxVec.begin(), nbSpxVec.end(), std::greater<int>());
    vector<int> weightVec({20, 10, 5});
    cout << "Benchmark on : " << pathPointCloud << endl;
    for (int i = 0; i < weightVec.size(); i++)
    {
        // Generation of SLIC Super Pixel
        slic.generateSuperpixels(labImage, nbSpxVec[0], weightVec[i]);
        slic.createConnectivity(labImage);
        slic.createHierarchy(labImage);

        for (int j = 0; j < nbSpxVec.size(); j++)
        {
            pair<int, int> key(nbSpxVec.at(j), weightVec.at(i));
            slic.setTreeLevel(nbSpxVec[j]);
            float accuracy = computeAccuracy(slic, riData, nbSpxVec[j], width);
            _result[key].push_back(accuracy);
            // cout << "NbSpx : " << nbSpxVec[j] << ", Weight : " << weightVec[i] << ", Accuracy : " << accuracy << endl;
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

int main(int argc, char **argv)
{
    // verif param

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
                benchmark(pointClouds.at(i), labels.at(j));
            }
        }
    }
    // Compute the benchmark average accuracy 
    for(map<pair<int,int>,vector<float>>::iterator it = _result.begin(); it != _result.end(); it++){
        int vecSize = it->second.size();
        float sum = 0;
        for (int i = 0; i < vecSize; i++)
        {
            sum += it->second.at(i);
        }
        sum /= vecSize;
        cout << "nbspx = " << it->first.first << " | weight = " << it->first.second << " | accuracy = " << sum*100 << "%" << endl;
    }
    
    return 0;
}
