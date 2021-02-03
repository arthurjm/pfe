
# Sous-représentation irrégulière duale pour le traitement d'image

#### Stagiaire : Arthur LONGUEFOSSE
###### Encadrants : R. Giraud, M. Clément
### SLIC Superpixel Implementation (C++), originally from : https://github.com/YutoUchimi/SLIC-Superpixels
### Superpixel Hierarchy (C++ / Matlab), originally from : https://github.com/xingxjtu/SuperpixelHierarchy
### Folders description :

* slic_hierarchy : Contains header and source files of the slic software, and the project file `slic_hierarchy.pro`
* fast_hierarchy : Contains header and source files of the fast hierarchy software, and the project file `fast_hierarchy.pro`
* images : Contains the image database

### Required packages :

* QMake verion 3.0 minimum  
* Qt version 4.8 minimum
* OpenCV 3.2.0 minimum

### Software execution :
`cd slic_hierarchy/` or `cd fast_hierarchy/`

`mkdir build && cd build && qmake .. && make`

`./slic_hierarchy` or `./fast_hierarchy`
