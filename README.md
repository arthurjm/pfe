# Interactive segmentation software for 3D LiDAR pointclouds


![](https://raw.githubusercontent.com/arthurjm/pfe/main/docs/ri-segmentation-app.png "Software screenshot example")

## Minimum required dependencies :

These are the minimum version of libraries that has been tested. More recent version works except for VTK 9.
- CMake 2.8 (3-clause BSD License)
- Boost  1.54 (Boost Software License)
- QT 5.11 (GNU Lesser General Public License)
- OpenCV 3.2  ([3-clause BSD License](https://github.com/opencv/opencv/blob/4.4.0/LICENSE))
- PCL 1.9 (Berkeley Software Distribution License)
- VTK 7.1 (Berkeley Software Distribution License)


## Main software 

### Compilation and execution 

```
cd pfe
mkdir build 
cd build
cmake ..
make
./ri_segmentation
```

### Data organisation for pointclouds 

```
/data/
    └── sequences/
            ├── 00/
            │   ├── labels/
            │   │     ├ 000000.label
            │   │     └ 000001.label
            │   └── velodyne/
            │         ├ 000000.bin
            │         └ 000001.bin
            ├── 01/
            ├── 02/
            .
            .
            .
            └── 21/
```
This is just a recommended format inspired by the semantic kitti API, the mandatory condition is to have `velodyne` and `labels` in the same folder and to put the same file name on a pointcloud and its corresponding label.

## Window interaction 
### Files
- CTRL + O : Open pointcloud file
- CTRL + L : Open label file
- CTRL + S : Save labels
### PointCloud
- Clic and drag : Rotation
- Scroll : Zoom
- Scroll wheel click : Translation

### Range Image

- Left click : Place marker (on scribble mode)
- Right click : Place unwanted label marker (on scribble mode)
- Scroll : Zoom 

## Superpixel Benchmark compilation 

### Benchmark compilation and excecution
```
cd pfe/benchmark
mkdir build
cd build
cmake ..
make
./benchmark
```

### Benchmark Options

```
Mode : default using mode hierarchy
   -h : using mode hierarchy
   -s : using mode segmentation
Metrics : default don't use the metrics
   -x : using metric X
   -y : using metric Y
   -z : using metric Z
   -r : using metric Remission
Superpixels : default using 1200 and 800 superpixels
   -p <nbSpx1> <nbSpx2> ... : number of superpixels used for the benchmark
Default :
   -d : using default parameters
   -df: using default files

```

Clients :

- Michaël Clément
- Rémi Giraud

Authors :

- Zhong Yi Chen
- Julien Legere
- Arthur Mondon
- Tsiory Rakotoarisoa
