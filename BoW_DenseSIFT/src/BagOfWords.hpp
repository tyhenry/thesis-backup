//
//  BagOfWords.hpp
//  BoWClustersTSNE
//
//  Created by Tyler on 4/4/16.
//
//

#pragma once
#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxCv.h"

// include non-free OpenCV modules
#include "opencv2/nonfree/nonfree.hpp"

//#include "Histogrid.cpp"

using namespace cv;
using namespace ofxCv;

class BagOfWords {
    
public:
    
    BagOfWords();
    BagOfWords(vector<string>& _imgPaths,
               int _nWords=400, // # of words
               string _dictionaryFile="dictionary.yml",
               string _descriptorsFile="descriptors.yml");
    
    
    /* TRAIN */
    // creates bag of words from image files, saves to dictionary (.yml) file
    // uses SIFT descriptors + K-means to find word centroids
    // pctToBoW = % of imgs to randomly extract descriptors from to build Bag of Visual Words
    
    void train(float pctToBoW = 1.0, int nIters = 3, bool bSave = true, bool bDense = false);
    
    // alt: write dictionary to specific file
    void train(string _dictionaryFile, float pctToBoW = 1.0, int nIters = 3, bool bDense = false){
        dictionaryFile = _dictionaryFile;
        train(pctToBoW, nIters, true, bDense);
    }
    
    
    /* DESCRIBE */
    // uses BoW to describe a set of images, or a single image
    // saves to descriptors (.yml) file
    
    void describe(int nImgs, bool bDense = false); // describe a bunch of images
    void describe(bool bDense = false){ // describe all images
        describe(-1, bDense);
    }
    void describe(string _descriptorsFile, int nImgs = -1, bool bDense = false){ // write to specific file
        descriptorsFile = _descriptorsFile;
    }
    void describe(ofImage& img, bool bDense); // describe a single image
    
    
    
    void describeSpatialPyramid(int nImgs, int nDivsX, int nDivsY, bool bAddFull = false, bool bDense=false); // divide grid evenly
    // bMerge = merge with overall description
    
    void describeSpatialPyramid(bool bAddFull = false, bool bDense = false){ // describe all images
        describeSpatialPyramid(-1, 3, 3, bAddFull, bDense); // use 3 x 3 grid
    }
    
    
    // load from file
    
    void loadDictionary(string _dictionaryFile);
    void loadImages(vector<string>& _imgPaths);
    void loadDescriptors(string _descriptorsFile);
    
    
    // return descriptors as map of vectors
    
    map<string,vector<float>>& getFeatureMap(){
        return features;
    }
    map<string,vector<vector<float>>>& getFeaturesByRoi(){
        return featuresByRoi;
    }
    
    void deriveFeaturesByRoi(int nRoi); // turns descriptor mat into featuresByRoi map
    
    
    int nWords;
    string dictionaryFile, descriptorsFile; //filenames
    
    Mat dictionary, descriptors; // vocabulary + saved histograms of images
    
    
private:
    
//    Ptr<xfeatures2d::SIFT> sift = xfeatures2d::SIFT::create();
    
    
    vector<string>* imgPaths; // stores img paths
    
    string type2str(int type); // returns Mat type
    
    map<string,vector<float>> features; // stores image names + descriptors
    
    map<string,vector<vector<float>>> featuresByRoi; // stores image names with vector of Roi descriptors
    
};
