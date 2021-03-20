#include "slic.h"
#include "rangeimage.h"
#include "pointcloud.h"
#include <QFileDialog>
#include <QApplication>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "gnuplot_i.hpp"

using namespace std;

#define METRIC_X 0
#define METRIC_Y 1
#define METRIC_Z 2
#define METRIC_REMISSION 3
#define MODE_HIERARCHY 4
#define MODE_SEGMENTATION 5
#define NUMBER_SUPERPIXEL 6
#define DEFAULT_PARAMETER 7
#define DEFAULT_FILE 8
#define INVALID -1

map<int, vector<float>> _result;

float computeAccuracy(Slic *slic, const riVertex *riData, int width)
{
    // nbLabels == number of clusters in the SLIC Algortithm
    unsigned int nbLabel = slic->nbLabels();
    unsigned int nbCluster = slic->getTreeLevel();
    float sum = 0;

    for (unsigned int i = 0; i < nbLabel; i++)
    {
        // pixels from one superpixel
        vector<pair<int, int>> pixels = slic->pixelsOfSuperpixel(i);
        unsigned int size = pixels.size();
        // first indicate the label, second indicate the number it appears
        vector<pair<int, int>> labelCount;
        for (unsigned int j = 0; j < size; j++)
        {
            int index = pixels.at(j).first * width + pixels.at(j).second;
            // get the label from ground truth
            int label = riData[index].label;
            long unsigned int k;
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
        for (long unsigned int k = 0; k < labelCount.size(); k++)
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

void benchmark(string pathPointCloud, string pathLabel, bool mode, vector<int> nbSpxVec, bool metrics[4])
{
    // The range image contains the ground truth data on label.
    PointCloud pc(pathPointCloud, pathLabel);

    RangeImage ri = pc.generateRangeImage(true);
    // RangeImage ri(pathPointCloud, pathLabel);
    Slic slic;

    // create a range image with canal Y of the range image with an equalization of histogram and an interpolation on the "dead" pixels (remission == -1)
    cv::Mat img = ri.createColorMat({RI_Y}, false, true, false, true);
    const riVertex *riData = ri.getData();
    int width = img.cols;

    // Make sure that first element is the largest
    std::sort(nbSpxVec.begin(), nbSpxVec.end(), std::greater<int>());
    cout << "Benchmark on : " << pathPointCloud << endl;

    for (long unsigned int j = 0; j < nbSpxVec.size(); j++)
    {
        if (!mode || j == 0) // re-segmentation or first segmentation
        {
            slic.generateSuperpixels(nbSpxVec[j], ri, metrics);
            slic.createConnectivity();
            slic.createHierarchy(metrics);
        }
        else // hierarchy
        {
            slic.setTreeLevel(nbSpxVec[j]);
        }
        float accuracy = computeAccuracy(&slic, riData, width);
        cout << "nbSpx : " << nbSpxVec[j] << " | accuracy : " << accuracy << endl;
        _result[nbSpxVec.at(j)].push_back(accuracy);
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

int getOption(char *param)
{
    string opt(param);
    int option = INVALID;
    if (opt.compare("-x") == 0)
        option = METRIC_X;
    else if (opt.compare("-y") == 0)
        option = METRIC_Y;
    else if (opt.compare("-z") == 0)
        option = METRIC_Z;
    else if (opt.compare("-r") == 0)
        option = METRIC_REMISSION;
    else if (opt.compare("-h") == 0)
        option = MODE_HIERARCHY;
    else if (opt.compare("-s") == 0)
        option = MODE_SEGMENTATION;
    else if (opt.compare("-p") == 0)
        option = NUMBER_SUPERPIXEL;
    else if (opt.compare("-d") == 0)
        option = DEFAULT_PARAMETER;
    else if (opt.compare("-df") == 0)
        option = DEFAULT_FILE;
    return option;
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
    if (argc == 1)
    {
        cout << "Mode : default using mode hierarchy" << endl;
        cout << "   -h : using mode hierarchy" << endl;
        cout << "   -s : using mode segmentation" << endl;
        cout << "Metrics : default don't use the metrics" << endl;
        cout << "   -x : using metric X" << endl;
        cout << "   -y : using metric Y" << endl;
        cout << "   -z : using metric Z" << endl;
        cout << "   -r : using metric Remission" << endl;
        cout << "Superpixels : default using 1200 and 800 superpixels" << endl;
        cout << "   -p <nbSpx1> <nbSpx2> ... : number of superpixels used for the benchmark" << endl;
        cout << "Default :" << endl;
        cout << "   -d : using default parameters" << endl;
        cout << "   -df: using default files" << endl;
        exit(EXIT_SUCCESS);
    }

    bool defaultFile = false;
    bool modeHierarchy = true; // true for reducing by hierarchy, false for reducing by re-segmentation
    vector<int> nbSpxVec;
    bool metrics[4];
    for (int i = 0; i < 4; i++)
    {
        metrics[i] = false;
    }

    // int minSpx = (_img.rows * _img.cols) / (min(_img.rows, _img.cols) * min(_img.rows, _img.cols));
    int minSpx = 16;

    // Verification on input parameters
    for (int i = 1; i < argc; i++)
    {
        int opt = getOption(argv[i]);
        switch (opt)
        {
        case MODE_HIERARCHY:
            cout << "using mode hierarchy" << endl;
            modeHierarchy = true;
            break;
        case MODE_SEGMENTATION:
            cout << "using mode re-segmentation" << endl;
            modeHierarchy = false;
            break;
        case METRIC_X:
            metrics[SLIC_METRIC_X] = true;
            cout << "using metric X" << endl;
            break;
        case METRIC_Y:
            metrics[SLIC_METRIC_Y] = true;
            cout << "using metric Y" << endl;
            break;
        case METRIC_Z:
            metrics[SLIC_METRIC_Z] = true;
            cout << "using metric Z" << endl;
            break;
        case METRIC_REMISSION:
            metrics[SLIC_METRIC_REMISSION] = true;
            cout << "using metric Remission" << endl;
            break;
        case NUMBER_SUPERPIXEL:
            cout << "number of super pixel =";
            while (++i < argc && getOption(argv[i]) == -1)
            {
                int spx = atoi(argv[i]);
                if (spx < minSpx)
                {
                    cerr << "\nnumber of super pixel invalid, must be greater than " << minSpx << endl;
                    exit(EXIT_FAILURE);
                }
                nbSpxVec.push_back(spx);
                cout << " " << spx;
            }
            i--;
            cout << endl;
            break;
        case DEFAULT_FILE:
            cout << "using default files" << endl;
            defaultFile = true;
            break;
        case DEFAULT_PARAMETER:
            cout << "using default parameters" << endl;
            break;
        case INVALID:
            cerr << "invalid option, execute without option for help" << endl;
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (nbSpxVec.size() == 0)
    {
        nbSpxVec.push_back(1200);
        nbSpxVec.push_back(800);
    }

    vector<string> pointClouds;
    vector<string> labels;
    // open the file to benchmark
    if (!defaultFile)
    {
        QApplication a(argc, argv);
        cout << "Please select a PointCloud (.bin) file you want to benchmark with." << endl;
        QStringList listOfFileNamePointCloud = QFileDialog::getOpenFileNames(nullptr, "Open a point cloud file", QString("../../data/velodyne"), "Binary file (*.bin)");
        cout << "Please select the Label (.label) file that match the previous PointCloud." << endl;
        QStringList listOfFileNameLabel = QFileDialog::getOpenFileNames(nullptr, "Open a label file", QString("../../data/labels"), "Binary file (*.label)");

        pointClouds.reserve(listOfFileNamePointCloud.size());
        labels.reserve(listOfFileNameLabel.size());

        for (const auto &i : listOfFileNamePointCloud)
        {
            pointClouds.push_back(i.toStdString());
        }
        for (const auto &i : listOfFileNameLabel)
        {
            labels.push_back(i.toStdString());
        }
    }
    // use default files
    else
    {
        //     pointClouds.push_back("../../data/velodyne/000000.bin");
        //     pointClouds.push_back("../../data/velodyne/000001.bin");
        //     pointClouds.push_back("../../data/velodyne/000002.bin");
        //     pointClouds.push_back("../../data/velodyne/000003.bin");
        //     pointClouds.push_back("../../data/velodyne/000004.bin");

        //     labels.push_back("../../data/labels/000000.label");
        //     labels.push_back("../../data/labels/000001.label");
        //     labels.push_back("../../data/labels/000002.label");
        //     labels.push_back("../../data/labels/000003.label");
        //     labels.push_back("../../data/labels/000004.label");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/00/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/00/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/00/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/00/velodyne/000800.bin");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/01/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/01/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/01/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/01/velodyne/000800.bin");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/02/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/02/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/02/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/02/velodyne/000800.bin");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/03/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/03/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/03/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/03/velodyne/000800.bin");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/05/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/05/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/05/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/05/velodyne/000800.bin");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/06/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/06/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/06/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/06/velodyne/000800.bin");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/07/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/07/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/07/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/07/velodyne/000800.bin");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/08/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/08/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/08/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/08/velodyne/000800.bin");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/09/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/09/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/09/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/09/velodyne/000800.bin");

        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/10/velodyne/000000.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/10/velodyne/000200.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/10/velodyne/000400.bin");
        pointClouds.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/pc_sequences/10/velodyne/000800.bin");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/00/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/00/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/00/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/00/labels/000800.label");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/01/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/01/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/01/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/01/labels/000800.label");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/02/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/02/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/02/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/02/labels/000800.label");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/03/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/03/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/03/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/03/labels/000800.label");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/05/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/05/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/05/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/05/labels/000800.label");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/06/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/06/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/06/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/06/labels/000800.label");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/07/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/07/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/07/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/07/labels/000800.label");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/08/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/08/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/08/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/08/labels/000800.label");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/09/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/09/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/09/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/09/labels/000800.label");

        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/10/labels/000000.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/10/labels/000200.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/10/labels/000400.label");
        labels.push_back("/home/julien/Desktop/benchmark_data/benchmark_data/label_sequences/10/labels/000800.label");
    }

    vector<string> files;

    // benchmark on files
    int pointCloudsSize = pointClouds.size();
    int labelsSize = labels.size();
    // for (int i = 0; i < pointCloudsSize; i++)
    // {
    //     string pointCloud = getFileName(pointClouds.at(i));
    //     for (int j = 0; j < labelsSize; j++)
    //     {
    //         string label = getFileName(labels.at(j));
    //         if (pointCloud == label)
    //         {
    //             files.push_back(getFileName(pointClouds.at(i)));
    //             benchmark(pointClouds.at(i), labels.at(j), modeHierarchy, nbSpxVec, metrics);
    //         }
    //     }
    // }

    for (int i = 0; i < pointCloudsSize; i++)
    {
        files.push_back(getFileName(pointClouds.at(i)));
        benchmark(pointClouds.at(i), labels.at(i), modeHierarchy, nbSpxVec, metrics);
    }

    Gnuplot plt;

    plt.set_style("lines");
    plt.set_title("accuracy on different files");
    plt.set_legend("right bottom");
    plt.set_xlabel("nbSpx");
    plt.set_ylabel("accuracy");
    vector<double> spx;
    vector<double> accVec;

    // plot the accuracy of the differents file
    for (int i = 0; i < _result.begin()->second.size(); i++)
    {
        for (map<int, vector<float>>::iterator it = _result.begin(); it != _result.end(); it++)
        {
            float acc = it->second.at(i);
            accVec.push_back(acc * 100);
            spx.push_back(it->first);
        }

        // save to png the plot with different accuracy for each file
        if (i == _result.begin()->second.size() - 1)
            plt.savetopng("filesAccuracy");

        plt.plot_xy(spx, accVec, files.at(i));
        spx.clear();
        accVec.clear();
    }

    fstream file("result.txt");
    ofstream file2("tmp.txt");

    // Compute the benchmark average accuracy
    cout << "Average accuracy : " << endl;
    for (map<int, vector<float>>::iterator it = _result.begin(); it != _result.end(); it++)
    {
        int vecSize = it->second.size();
        float sum = 0;
        for (int i = 0; i < vecSize; i++)
        {
            sum += it->second.at(i);
        }
        sum /= vecSize;
        cout << "nbspx = " << it->first << " | accuracy = " << sum * 100 << "%" << endl;
        spx.push_back(it->first);
        accVec.push_back(sum * 100);

        // save the accuracy in result file
        if (file && file2)
        {
            char line[256];
            file.getline(line, 256);
            file2 << line << " " << sum * 100 << endl;
        }
        else if (file2)
        {
            file2 << it->first << " " << sum * 100 << endl;
        }
        else
        {
            cerr << "error file " << endl;
            exit(EXIT_FAILURE);
        }
    }

    if (file)
        file.close();

    file2.flush();
    file2.close();
    system("mv tmp.txt result.txt");

    // plot and save the average accuracy
    Gnuplot plt2;

    plt2.set_style("lines");
    plt2.set_xlabel("nbSpx");
    plt2.set_ylabel("accuracy");
    plt2.set_legend("right bottom");
    plt2.set_title("average accuracy");
    plt2.plot_xy(spx, accVec, "average accuracy");
    //plt2.set_title("notitle");
    plt2.set_style("points");
    plt2.savetopng("accuracy");
    plt2.plot_xy(spx, accVec, "average accuracy");
    return 0;
}
