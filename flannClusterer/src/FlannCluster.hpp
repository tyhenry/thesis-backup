//
//  FlannCluster.hpp
//  flannClusterer
//
//  Created by Tyler on 4/28/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxCsv.h"
#include "opencv2/nonfree/nonfree.hpp"

using namespace cv;
using namespace ofxCv;
using namespace wng;

class FlannCluster{
    
public:
    
    FlannCluster(){}
    FlannCluster(vector<vector<float>>& _features);
    void loadFeatures(vector<vector<float>>& _features);
    void loadFeaturesFile(string path);
    
    void readoutFeatures(bool bPrintFull = false);
    
    void splitFeaturesToRois(int _rois);
    
    void makeIndex(string _indexFile = "");
    void loadIndex(string _indexFile);
    
    void makeRoiIndexes(string _indexFileBase = "");
    void loadRoiIndexes(string _indexFileBase);
    
    bool knnSearch(Mat& queries, int k, Mat& _indices, Mat& _distances);
    // returns true/false for success/failure
    bool knnSearchRoi(int roi, Mat& queries, int k, Mat& _indices, Mat& _distances);
    
    Mat getFeaturesAtIndex(int i);
    Mat getRoiFeaturesAtIndex(int i, int roi);
    
    string getImgAtIndex(int i);
    vector<string> getImgsAtIndices(Mat& indices);
    vector<string> getImgsAtIndices(vector<int>& indices);
    
    void setNumRoi(int _rois){ rois = _rois; }
    
    
private:
    
    vector<vector<float>>* features;
    Mat featuresMat;
    int nElements = 0;
    int nDims = 0;
    int rois = 1;
    int roiNDims = 0;
    
    vector<vector<vector<float>>> roiFeatures;
    // rois < imgs < features
    vector<Mat> roiFeaturesMats;
    
    flann::Index* index;
    bool bHasIndex = false;
    string indexFile;
    
    vector<flann::Index*> roiIndexes;
    bool bHasRoiIndexes = false;
    string roiIndexFile;
    
    vector<string> imgNames; // stores imgNames
    
    // concatenate multivector into one
    void concatVectors(vector<vector<float>>& vectors, vector<float>& concat){
        concat.clear();
        for (auto vec=vectors.begin(); vec!=vectors.end(); vec++){
            for (auto it=vec->begin(); it!=vec->end(); it++){
                concat.push_back(*it);
        }}
    }
    
    // transfer multivector into mat, return nElements + nDims as pair<int,int>
    pair<int,int> loadFeaturesVecToMat(vector<vector<float>>& _features, Mat& mat){
        pair<int,int> nElsnDims(0,0);
        nElsnDims.first = _features.size();
        if (nElsnDims.first > 0){
            nElsnDims.second = _features[0].size();
        } else {
            ofLogError("FlannCluster::loadFeaturesVecToMat") << "empty vector!\n\t"
                << "nElements,nDims: " << nElsnDims.first << "," << nElsnDims.second;
            return nElsnDims;
        }
        if (nElsnDims.second <= 0){
            ofLogError("FlannCluster::loadFeaturesVecToMat") << "empty features!\n\t"
                << "nElements,nDims: " << nElsnDims.first << "," << nElsnDims.second;
            return nElsnDims;
        }
        
        // convert features vector to Mat
        mat = Mat (nElsnDims.first, nElsnDims.second, CV_32F);
        vector<float> concat;
        concatVectors(_features, concat);
        memcpy(featuresMat.data,concat.data(),concat.size()*sizeof(float));
        return nElsnDims;
    }
    
    string type2str(int type) {
        string r;
        
        uchar depth = type & CV_MAT_DEPTH_MASK;
        uchar chans = 1 + (type >> CV_CN_SHIFT);
        
        switch ( depth ) {
            case CV_8U:  r = "8U"; break;
            case CV_8S:  r = "8S"; break;
            case CV_16U: r = "16U"; break;
            case CV_16S: r = "16S"; break;
            case CV_32S: r = "32S"; break;
            case CV_32F: r = "32F"; break;
            case CV_64F: r = "64F"; break;
            default:     r = "User"; break;
        }
        
        r += "C";
        r += (chans+'0');
        
        return r;
    }
    
};