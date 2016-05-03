//
//  FlannCluster.cpp
//  flannClusterer
//
//  Created by Tyler on 4/28/16.
//
//

#include "FlannCluster.hpp"


FlannCluster::FlannCluster(vector<vector<float>>& _features){
    
    loadFeatures(_features);
}

//--------------------------------------------------------------
void FlannCluster::loadFeatures(vector<vector<float>>& _features){
    
    features = &_features;
    
    pair<int,int> nElsnDims = loadFeaturesVecToMat(*features,featuresMat);
    nElements = nElsnDims.first;
    nDims = nElsnDims.second;
    
    // print results
    readoutFeatures();
}

//--------------------------------------------------------------
void FlannCluster::loadFeaturesFile(string featuresPath){
    
    features = new vector<vector<float>>;
    
    FileStorage featuresFile(ofToDataPath(featuresPath), FileStorage::READ);
    
    FileNode featuresRoot = featuresFile.root();
    
    FileNodeIterator it = featuresRoot.begin();
    FileNodeIterator it_end = featuresRoot.end();
    
    ofLogNotice("FlannCluster") << "features file size: " << featuresRoot.size();

    int idx = 0;
    
    imgNames.clear();
    
    // load yml file into features vector and featuresMat
    
    for( ; it != it_end; ++it, idx++ ){
        
        string imgName = (*it).name(); // get image name
        
        Mat featuresData;
        featuresFile[imgName] >> featuresData; // get feature data as Mat
        vector<float> featureVec; // store features for features vector
        
        // if data is empty
        if (featuresData.cols == 0){
            ofLogError("FlannCluster")
                << "features [" << idx << "] - " << imgName << ": empty data!";
        }
        else if (featuresData.isContinuous()) {
            
            featureVec.assign((float*)featuresData.datastart, (float*)featuresData.dataend); // < converts 1d mat to vector

            imgNames.push_back(imgName); // save img filename
            features->push_back(featureVec); // save features to vector
            
        } else {
            ofLogError("FlannCluster")
                << "features [" << idx << "] - " << imgName << ": data not continuous!";
        }
    }
    
    readoutFeatures(); // just print indices, not full readout

    // transfer features to featuresMat
    pair<int,int> nElsnDims = loadFeaturesVecToMat(*features, featuresMat);
    nElements = nElsnDims.first;
    nDims = nElsnDims.second;
    
    ofLogNotice("FlannCluster") << "featuresMat rows, cols: " << featuresMat.rows << ", " << featuresMat.cols;
}

//--------------------------------------------------------------
void FlannCluster::readoutFeatures(bool bPrintFull){
    ofLogNotice("FlannCluster") << "features readout:";
    cout << "-----------------------------------------" << endl;
    int row = 0;
    for (auto vec=features->begin(); vec!=features->end(); vec++){
        if (bPrintFull) { cout << endl; }
        cout << getImgAtIndex(row) << "\n\t\t\t\t\t\t\t\t"; // print img name
        cout << "row " << row << ", nFeatures: " << vec->size() << endl;
        for (auto it=vec->begin(); it!=vec->end() && (bPrintFull); it++){
            cout << ofToString(*it) << ";";
        }
        row++;
    }
}

//--------------------------------------------------------------
void FlannCluster::splitFeaturesToRois(int _rois){
    
    rois = _rois;
    
    roiFeaturesMats.clear();
    roiFeatures.clear();
    
    // if there are any features...
    
    if (features->size() > 0){
        
        roiFeatures.resize(rois);
        int roiSize = (*features)[0].size() / rois;
        roiNDims = roiSize;
        int imgIdx = 0;
        
        // loop through images in features vector
        for (auto img=features->begin(); img!=features->end(); ++img) {
            
            // loop through rois, split features into vectors
            
            auto first = img->begin();
            auto last = first + roiSize;
            int roi = 0;
            
            while (last <= img->end()){
                
                // copy into new vector
                vector<float> roiImgFeat(first,last);
                
                // save to roi vector
                roiFeatures[roi].push_back(roiImgFeat);
                
                first = last;
                last += roiSize;
                
                roi++;
            }
            
            ofLogNotice("FlannCluster") << "split img [" << imgIdx << "] into " << rois << " rois of " << roiFeatures[0][imgIdx].size() << " features each";
            
            imgIdx++;
        }
        
        // copy roiFeatures to roiFeaturesMat
        for (auto it=roiFeatures.begin(); it!=roiFeatures.end(); ++it){
            Mat roiMat;
            pair<int,int> nElsnDims = loadFeaturesVecToMat(*it, roiMat);
            roiFeaturesMats.push_back(roiMat);
        }
        
        ofLogNotice("FlannCluster") << "splitFeaturesToRois completed with " << roiFeatures.size() << " rois";
    }
    else {
        ofLogError("FlannCluster") << "tried to split empty features vector into roi!";
    }
}

//--------------------------------------------------------------
void FlannCluster::makeIndex(string _indexFile){
    
    // KdTree with 5 random trees
    flann::KDTreeIndexParams kdtreeParams(5); // indexParams(5);
    
    // Create the Index
    index = new flann::Index(featuresMat, kdtreeParams);
    
    bHasIndex = true;
    
    // save to file
    if (_indexFile =="")
        indexFile = "flannIndex_" + ofToString(ofGetUnixTime()) + ".fnn";
    else
        indexFile = _indexFile;
    
    index->save(ofToDataPath(indexFile));
    
    ofLogNotice("FlannCluster") << "index created: " << indexFile;

}

//--------------------------------------------------------------
void FlannCluster::makeRoiIndexes(string _indexFileBase){
    
    roiIndexes.clear();
    roiIndexes.resize(rois);
    
    // KdTree with 5 random trees
    flann::KDTreeIndexParams kdtreeParams(5);
    
    // filename prep
    if (_indexFileBase == "")
        _indexFileBase = "flannIndex_" + ofToString(ofGetUnixTime());
    
    roiIndexFile = _indexFileBase;
    
    // create the indexes
    for (int r=0; r<rois; r++){
        roiIndexes[r] = new flann::Index(roiFeaturesMats[r], kdtreeParams);
        
        string fn = roiIndexFile + "_roi-" + ofToString(r) + ".fnn";
        roiIndexes[r]->save(ofToDataPath(fn));
        ofLogNotice("FlannCluster") << r << " roi index created: " << fn;

    }

    ofLogNotice("FlannCluster") << rois << " roi indexes created";
    
    bHasRoiIndexes = true;
    
}

//--------------------------------------------------------------
void FlannCluster::loadIndex(string _indexFile){
    indexFile = _indexFile;
    index->load(featuresMat, _indexFile);
    bHasIndex = true;
}

//--------------------------------------------------------------
bool FlannCluster::knnSearch(Mat& queries, int k, Mat& _indices, Mat& _distances){
    // indices = returns Mat of indexes of NN
    // distances = returns Mat of distances of NN to query pts
    
    int nQueries = queries.rows;
    
    Mat indices = Mat::zeros(nQueries, k, CV_32S);
    Mat distances = Mat::zeros(nQueries, k, CV_32F);
    
    if (!bHasIndex){
        ofLogError("FlannCluster") << "knnSearch tried without index!";
        return false; // fail
    }
    
    float startSearchTime = ofGetElapsedTimeMillis();
    // search
    index->knnSearch(queries, indices, distances, k, flann::SearchParams(64));
    float timeTook = ofGetElapsedTimeMillis() - startSearchTime;
    
    _indices = indices;
    _distances = distances;
    
    // print results
    ofLogNotice("FlannCluster") << "searched for " << nQueries << " queries in " << timeTook << " ms";
    cout << "Results:" << endl << "--------" << endl;
    for (int row = 0; row < indices.rows; row++){
        cout << "query " << row << " - index, dist: " << endl;
        for (int col = 0; col < indices.cols; col++){
            cout << "\t" << indices.at<int>(row,col) << ", " << distances.at<float>(row,col) << endl;
        }
        cout << endl;
    }
    
    return true; // success
}

//--------------------------------------------------------------
bool FlannCluster::knnSearchRoi(int roi, Mat& queries, int k, Mat& _indices, Mat& _distances){
    // indices = returns Mat of indexes of NN
    // distances = returns Mat of distances of NN to query pts
    
    int nQueries = queries.rows;
    
    Mat indices = Mat::zeros(nQueries, k, CV_32S);
    Mat distances = Mat::zeros(nQueries, k, CV_32F);
    
    if (!bHasRoiIndexes){
        ofLogError("FlannCluster") << "ROI knnSearch tried without ROI indexes!";
        return false; // fail
    }
    
    float startSearchTime = ofGetElapsedTimeMillis();
    // search roi
    roiIndexes[roi]->knnSearch(queries, indices, distances, k, flann::SearchParams(64));
    float timeTook = ofGetElapsedTimeMillis() - startSearchTime;
    
    _indices = indices;
    _distances = distances;
    
    // print results
    ofLogNotice("FlannCluster") << "searched for " << nQueries << " queries in " << timeTook << " ms";
    cout << "Results:" << endl << "--------" << endl;
    for (int row = 0; row < indices.rows; row++){
        cout << "query " << row << " - index, dist: " << endl;
        for (int col = 0; col < indices.cols; col++){
            cout << "\t" << indices.at<int>(row,col) << ", " << distances.at<float>(row,col) << endl;
        }
        cout << endl;
    }
    
    return true; // success
    
}

//--------------------------------------------------------------
Mat FlannCluster::getFeaturesAtIndex(int i){
    
    Mat feats = Mat::zeros(1,nDims,CV_32F);
    if (i < featuresMat.rows){
        featuresMat.row(i).copyTo(feats.row(0));
    } else {
        ofLogError("FlannCluster") << "Tried to get features at out-of-bounds index" << i;
    }
    return feats;
}

//--------------------------------------------------------------
Mat FlannCluster::getRoiFeaturesAtIndex(int i, int roi){
    
    Mat feats = Mat::zeros(1,roiNDims,CV_32F);
    if (i < roiFeaturesMats[roi].rows){
        roiFeaturesMats[roi].row(i).copyTo(feats.row(0));
    } else {
        ofLogError("FlannCluster") << "Tried to get ROI features at out-of-bounds index" << i;
    }
    return feats;
}

//--------------------------------------------------------------
string FlannCluster::getImgAtIndex(int i){
    if (imgNames.size() > i){
        return imgNames[i];
    }
    ofLogError("FlannCluster") << "tried to get img at out-of-bounds index " << i;
    return "";
}
vector<string> FlannCluster::getImgsAtIndices(vector<int>& indices){
    vector<string> imgs;
    for (auto it=indices.begin(); it!=indices.end(); it++){
        imgs.push_back(getImgAtIndex(*it));
    }
    return imgs;
}
vector<string> FlannCluster::getImgsAtIndices(Mat& indices){
    // only look at first row
    const int* p = indices.ptr<int>(0);
    vector<int> idsVec(p, p + indices.cols);
    return getImgsAtIndices(idsVec);
}



