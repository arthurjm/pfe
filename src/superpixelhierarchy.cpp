#include "superpixelhierarchy.h"
#include "superpixelhierarchymex.hpp"
SuperpixelHierarchy::SuperpixelHierarchy()
{

}


int findset(int i, int *parent)
{
    int p = parent[i];
    if (i != p)
    {
        parent[i] = findset(p, parent);
    }
    return parent[i];
}

void SuperpixelHierarchy::getSuperpixels(int *parent, int *label, int *treeu, int *treev, int &nvertex, int &nregion, int &N)
{
    if (N < 1 || N > nvertex)
    {
        printf("error");
        exit(1);
    }

    _currentHierarchy = cv::Mat(1,_nbClusters, CV_32FC1);


    int end   = nvertex-N;
    int begin = nvertex-nregion;
    if (nregion < N)
    {
        for (int i=0; i<nvertex; ++i) parent[i] = i;
        begin = 0;
    }

    for (int i=begin; i<end; ++i)
    {
        int u  = treeu[i];
        int v  = treev[i];
        int pu = findset(u,parent);
        int pv = findset(v,parent);
        if (pu < pv)
            parent[pv] = pu;
        else
            parent[pu] = pv;
    }

    nregion = 0;
    for (int i=0; i<nvertex; ++i)
    {
        int p = findset(i,parent);
        if (i == p)
            label[i] = nregion++;
        else
            label[i] = label[p];
    }

    for (int i=0; i<nvertex; ++i){
        _currentHierarchy(0,_hierarchyPixels(0,i)) = label[i];
    }
}

void SuperpixelHierarchy::getLevels(int *parent, int *treeu, int *treev, int &nvertex, int &nregion){
    levels.clear();
    int n = _nbClusters;
    while(n>0){
        levels.push_back(n);
        n=n/2;
    }
    cv::Mat_<int> tree = cv::Mat(levels.size(), nvertex, CV_32FC1);
    _hierarchyClusters = cv::Mat(levels.size(), _nbClusters, CV_32FC1);


    int lvl;
    for (int n=0; n<levels.size(); n++) {
        lvl = levels[n];
        if (lvl < 1 || lvl > nvertex)
        {
            printf("error");
            exit(1);
        }

        int end   = nvertex-lvl;
        int begin = nvertex-nregion;
        if (nregion < lvl)
        {
            for (int i=0; i<nvertex; ++i) parent[i] = i;
            begin = 0;
        }

        for (int i=begin; i<end; ++i)
        {
            int u  = treeu[i];
            int v  = treev[i];
            int pu = findset(u,parent);
            int pv = findset(v,parent);
            if (pu < pv)
                parent[pv] = pu;
            else
                parent[pu] = pv;
        }

        nregion = 0;
        for (int i=0; i<nvertex; ++i)
        {
            int p = findset(i,parent);
            if (i == p)
                tree(n,i) = nregion++;
            else
                tree(n,i) = tree(n,p);
        }
    }
    _hierarchyPixels = tree;
    for (int n=0; n<levels.size(); n++) {
        for (int i=0; i<nvertex; ++i){
            _hierarchyClusters(n,tree(0,i)) = tree(n,i);
        }
    }
    initPic();
}

void SuperpixelHierarchy::initPic() {
    vector<vector<pair<int,int>>> pic (_nbClusters);
    vector<pair<int,int>> tmp;
    for (unsigned int i = 0; i < _nbClusters; i++){
        pic[i] = tmp;
    }
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            int label = _hierarchyPixels(0,j+i*h);
            if(label != -1){
                pic[label].push_back(make_pair(i,j));
            }
        }
    }
    _pixelsInCluster = pic;
}

cv::Mat SuperpixelHierarchy::buildHierarchy(const cv::Mat &pImage, const cv::Mat &pContours, int pNbSpx, int pWeight, bool buildScribbleLevels, RangeImage& rangeImage){
    connect = pWeight;
    w = pImage.cols;
    h = pImage.rows;
    currentLevel = pNbSpx;
    img = pImage;
    contours = pContours;

    cv::Mat_<float> imgLine = pImage.clone().reshape(1, h*w).t();
    cv::Mat_<float> edgeLine = pContours.clone().reshape(1, pContours.rows*pContours.cols).t();

    unsigned char * image_shift = (unsigned char *) calloc((h*w)*3,sizeof(unsigned char));
    unsigned char * edge_shift = (unsigned char *) calloc((h*w),sizeof(unsigned char));

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            image_shift[j+i*h] = imgLine(i+j*w);
            image_shift[j+i*h+h*w] = imgLine(i+j*w+h*w);
            image_shift[j+i*h+2*h*w] = imgLine(i+j*w+2*h*w);
            edge_shift[j+i*h] = edgeLine(i+j*w);
            //if(h!=pContours.rows) 
            edge_shift[j+i*h] = 0;
        }
    }

    SuperpixelHierarchyMex SH;
    SH.init(h,w,connect,iterSwitch);
    SH.buildTree(image_shift, rangeImage, edge_shift);
    //Extraction of parameters from the tree
    int * parent = (int *) malloc(h*w*sizeof(int));
    int * label = (int *) malloc(h*w*sizeof(int));
    int * treeu = (int *) malloc((h*w-1)*sizeof(int));
    int * treev = (int *) malloc((h*w-1)*sizeof(int));
    memcpy(parent, SH.getParent(), sizeof(int)*h*w);
    memcpy(label,  SH.getLabel(),  sizeof(int)*h*w);
    memcpy(treeu,  SH.getTreeU(),  sizeof(int)*(h*w-1));
    memcpy(treev,  SH.getTreeV(),  sizeof(int)*(h*w-1));
    int nvertex = h*w;
    int nregion = SH.getRegion();
    //Output the label map for a given sp scale
    if(buildScribbleLevels){
        getLevels(parent,treeu,treev,nvertex,nregion);
    }
    getSuperpixels(parent,label,treeu,treev,nvertex,nregion, pNbSpx);
    //Save output
    clustersImage = cv::Mat(pImage.rows, pImage.cols, CV_8UC3);
    for (int i=0; i<w; i++){
        for (int j=0; j<h; j++) {
            int c = label[j+i*h];
            clustersImage.at<cv::Vec3b>(j,i) = cv::Vec3b(c,c,c);
        }
    }
    cv::Mat output_border = displayContours(pImage.clone());

    free(parent);
    free(label);
    free(treeu);
    free(treev);
    free(image_shift);
    free(edge_shift);
    return output_border;
}

cv::Mat SuperpixelHierarchy::displayContours(cv::Mat pImage){
    cv::Mat output = pImage.clone();
    const int dc8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
    const int dr8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

    vector<cv::Point> contours;
    cv::Mat_<uchar> istaken(pImage.rows, pImage.cols, uchar(0));

    /* Go through all the pixels. */
    for (int col = 0; col < pImage.cols; col++) {
        for (int row = 0; row < pImage.rows; row++) {
            int nr_p = 0;

            /* Compare the pixel to its 8 neighbours. */
            for (int k = 0; k < 8; k++) {
                int c = col + dc8[k];
                int r = row + dr8[k];

                if (c >= 0 && c < pImage.cols && r >= 0 && r < pImage.rows) {
                    if (istaken(r,c) == false && clustersImage.at<cv::Vec3b>(row,col) != clustersImage.at<cv::Vec3b>(r,c)) {
                        nr_p += 1;
                    }
                }
            }

            /* Add the pixel to the contour list if desired. */
            if (nr_p >= 2) {
                contours.push_back(cv::Point(row,col));
                istaken(row,col) = true;
            }
        }
    }

    /* Draw the contour pixels. */
    for (int i = 0; i < (int)contours.size(); i++) {
       output.at<cv::Vec3b>(contours[i].x, contours[i].y) = colorContours;
    }
    return output;
}

void SuperpixelHierarchy::displaySaliency(cv::Mat &pImage, int n){
    const int dc8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
    const int dr8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

    vector<cv::Point> contours;
    cv::Mat_<uchar> istaken(pImage.rows, pImage.cols, uchar(0));

    /* Go through all the pixels. */
    for (int col = 0; col < pImage.cols; col++) {
        for (int row = 0; row < pImage.rows; row++) {
            int nr_p = 0;

            /* Compare the pixel to its 8 neighbours. */
            for (int k = 0; k < 8; k++) {
                int c = col + dc8[k];
                int r = row + dr8[k];

                if (c >= 0 && c < pImage.cols && r >= 0 && r < pImage.rows) {
                    if (istaken(r,c) == false && _hierarchyClusters(n, _hierarchyPixels(0,row+col*h)) != _hierarchyClusters(n, _hierarchyPixels(0,r+c*h))) {
                        nr_p += 1;
                    }
                }
            }

            /* Add the pixel to the contour list if desired. */
            if (nr_p >= 2) {
                contours.push_back(cv::Point(row,col));
                istaken(row,col) = true;
            }
        }
    }

    /* Draw the contour pixels. */
    for (int i = 0; i < (int)contours.size(); i++) {
        int greyScale[11] = { 250, 245, 240, 230, 220, 210, 200, 175, 150, 125, 0};
        int val = greyScale[n];
        pImage.at<cv::Vec3b>(contours[i].x, contours[i].y) = cv::Vec3b(val,val,val);
    }
}

cv::Mat SuperpixelHierarchy::displayAllSaliency(cv::Mat pImage){
    cv::Mat output = pImage.clone();
    for (int n = 0; n < levels.size() - 1; n++){
        displaySaliency(output, n);
    }
    return output;
}

void SuperpixelHierarchy::binaryLabelisationConnected(){
    if(obj.empty()) return;
    _selectedClusters.clear();
    int lvl, minFusion;

    for (int i = 0; i < obj.size(); i++){
        if(bg.empty()) minFusion = levels.size() -1;
        else{
            minFusion = INT_MAX;
            for (int j = 0; j < bg.size(); j++){
                lvl = levelOfFusion(obj[i], bg[j]);
                if(lvl < minFusion) minFusion = lvl;
            }
        }
        minFusion=max(1,minFusion); //TODO:CHECK IF NEEDED
        int indexObject = _hierarchyClusters(minFusion-1, obj[i]);
        for (int i=0; i<_nbClusters; i++){
            if(_hierarchyClusters(minFusion-1,i)== indexObject){
                if(find(_selectedClusters.begin(), _selectedClusters.end(), i) == _selectedClusters.end()){
                    _selectedClusters.push_back(i);
               }
           }
       }
    }

    //Connexity, not sure if 100% needed
    for (int i = 0; i < bg.size(); i++){
        minFusion = INT_MAX;
        for (int j = 0; j < obj.size(); j++){
            lvl = levelOfFusion(bg[i], obj[j]);
            if(lvl < minFusion) minFusion = lvl;
        }
        int indexBackground = _hierarchyClusters(minFusion-1, bg[i]);
        for (int i=0; i<_nbClusters; i++){
            if(_hierarchyClusters(minFusion-1,i)== indexBackground){
                auto spxIterator = find(_selectedClusters.begin(), _selectedClusters.end(), i);
                if (spxIterator!=_selectedClusters.end()){
                    _selectedClusters.erase(spxIterator);
                }
            }
        }
    }
}

cv::Mat SuperpixelHierarchy::displaySelection(cv::Mat backgroundImage, cv::Mat selectionImage){
    cv::Mat output = backgroundImage.clone();
    for(int index = 0; index < _selectedClusters.size(); index++){
        for (int cluster = 0; cluster < _nbClusters; cluster++){
            if(_hierarchyClusters(0,cluster)==_selectedClusters[index]){
                for (unsigned int p = 0; p < _pixelsInCluster[cluster].size(); ++p){
                    int i = _pixelsInCluster[cluster][p].first;
                    int j = _pixelsInCluster[cluster][p].second;
                    output.at<cv::Vec3b>(j,i) = selectionImage.at<cv::Vec3b>(j,i);
                }
            }
        }
    }
    return output;
}

cv::Mat SuperpixelHierarchy::displayGreySelection(cv::Mat pImage){
    cv::Mat output = pImage.clone();
    for(int index = 0; index < _selectedClusters.size(); index++){
        for (int cluster = 0; cluster < _nbClusters; cluster++){
            if(_hierarchyClusters(0,cluster)==_selectedClusters[index]){
                for (unsigned int p = 0; p < _pixelsInCluster[cluster].size(); ++p){
                    int i = _pixelsInCluster[cluster][p].first;
                    int j = _pixelsInCluster[cluster][p].second;
                    if (output.at<cv::Vec3b>(j, i) != colorContours)
                        output.at<cv::Vec3b>(j, i) = output.at<cv::Vec3b>(j, i) / 2.0f;
                    else{
                        output.at<cv::Vec3b>(j, i) = colorSelection;                }
                    }
            }
        }
    }
    return output;
}

void SuperpixelHierarchy::selectCluster(cv::Point2i pPos) {
    int pixel = pPos.y + pPos.x*h;
    int indexCluster = _hierarchyPixels(0, pixel);
    indexCluster = _currentHierarchy(0,indexCluster);
    if (indexCluster == -1) return;
    for (int i = 0; i < _nbClusters; i++){
        if (_currentHierarchy(0, i) == indexCluster){
            if(find(_selectedClusters.begin(), _selectedClusters.end(), i) == _selectedClusters.end()){
                _selectedClusters.push_back(i);
            }
        }
    }
}

void SuperpixelHierarchy::deselectCluster(cv::Point2i pPos) {
    int pixel = pPos.y + pPos.x*h;
    int indexCluster = _hierarchyPixels(0, pixel);
    indexCluster = _currentHierarchy(0,indexCluster);
    if (indexCluster == -1) return;
    for (int i = 0; i < _nbClusters; i++){
        if (_currentHierarchy(0, i) == indexCluster){
            auto index = find(_selectedClusters.begin(), _selectedClusters.end(), i);
            if(index!=_selectedClusters.end()){
                _selectedClusters.erase(index);
            }
        }
    }
}

int SuperpixelHierarchy::levelOfFusion(int label1, int label2){
    for(int lvl = 0; lvl < levels.size(); lvl++){
        if(_hierarchyClusters(lvl,label1) == _hierarchyClusters(lvl,label2)) return lvl;
    }
    return -1;
}

void SuperpixelHierarchy::addObjectCluster(cv::Point2i pPos) {
    int pixel = pPos.y + pPos.x*h;
    int cluster = _hierarchyPixels(0,pixel);
    if(find(obj.begin(), obj.end(), cluster) == obj.end()){
        obj.push_back(cluster);
    }
    auto index = find(bg.begin(), bg.end(), cluster);
    if(index!=bg.end()) bg.erase(index);
}

void SuperpixelHierarchy::addBackgroundCluster(cv::Point2i pPos) {
    int pixel = pPos.y + pPos.x*h;
    int cluster = _hierarchyPixels(0,pixel);
    if(find(bg.begin(), bg.end(), cluster) == bg.end()){
        bg.push_back(cluster);
    }
    auto index = find(obj.begin(), obj.end(), cluster);
    if(index!=obj.end()) obj.erase(index);
}

int SuperpixelHierarchy::zoomInHierarchy(int pMaxLevel){
    if(currentLevel < pMaxLevel /2){
        return currentLevel *2;
    }
    return pMaxLevel;
}

int SuperpixelHierarchy::zoomOutHierarchy(){
    return max(currentLevel/2,1);
}

int SuperpixelHierarchy::labelOfPixel(cv::Point2i pPos){
    int cluster = _hierarchyPixels(0,pPos.y + pPos.x*h);
    return _currentHierarchy(0, cluster);
}
