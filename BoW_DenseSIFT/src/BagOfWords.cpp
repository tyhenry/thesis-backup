//
//  BagOfWords.cpp
//  BoWClustersTSNE
//
//  Created by Tyler on 4/4/16.
//
//

#include "BagOfWords.hpp"


BagOfWords::BagOfWords(){
    
}

BagOfWords::BagOfWords(vector<string>& _imgPaths, int _nWords, string _dictionaryFile, string _descriptorsFile){
    
    imgPaths = &_imgPaths;
    
    nWords = _nWords;
    
    dictionaryFile = _dictionaryFile; descriptorsFile = _descriptorsFile;
    
}

//-------------------------------------

// TRAINER, GENERATES DICTIONARY (vocab) & SAVES TO DISK
    
void BagOfWords::train( float pctToBoW, int nIters, bool bSave, bool bDense ){
    
    SiftDescriptorExtractor detector;
    
    Mat featuresUnclustered; // stores all features of all images
    
    vector<KeyPoint> keypoints; // keypoint storage
    
    Mat descriptor; // descriptor storage
    
    
    if (bDense){
        ofLogNotice("BoW Trainer") << "using dense keypoints";
    } else {
        ofLogNotice("BoW Trainer") << "using auto SIFT detector keypoints";
    }
    
    
    int nImgs = pctToBoW * imgPaths->size(); // num images to extract BoW from (randomly)
    
    vector<int> usedImgs; // imgs we've already extracted from
    

    for (int i=0; i<nImgs; i++){
        
        int p = i;
        if (pctToBoW != 1.0){ // not extracting from all, get a random sample
        
             // find random spot in vector
            
            int p = (int)ofRandom (0,imgPaths->size()); // set to random img
            
            while (ofContains(usedImgs,p) || // check if used
                   p >= imgPaths->size()) // check if out of bounds
            {
                p = (int)ofRandom(0,imgPaths->size()); // retry
            }
            
            usedImgs.push_back(p); // save to spots vector
        }
        
        
        
        ofImage img;
        
        img.load((*imgPaths)[p]); // load img
        
        img.setImageType(OF_IMAGE_GRAYSCALE); // cvt to grayscale
        
        Mat input = toCv(img); // get Mat version of ofImage
        
        
    // FEATURE EXTRACTION
        
        if (bDense) {
        
            // use dense keypoints (regular grid)
            
            int step = 10; // 10 pixels spacing between kp's
            
            for (int y=step; y<input.rows-step; y+=step)
            {
                for (int x=step; x<input.cols-step; x+=step)
                {
                    // x,y,radius
                    keypoints.push_back(KeyPoint(float(x), float(y), float(step)));
                }
            }
            
        } else {
            
            // or auto-detect keypoints
            
            detector.detect(input, keypoints);
        }
        
        
        // compute descriptors
        
        detector.compute(input, keypoints, descriptor);
        
        // store described features
        
        featuresUnclustered.push_back(descriptor);
        
        
        ofLogNotice("BoW Trainer") << "Feature extraction - percent done: " << (float)i/(float)nImgs;
        
    }
    
    ofLogNotice("BoW Trainer") << "Feature extraction complete - cols: " << featuresUnclustered.cols << ", rows: " << featuresUnclustered.rows << ", datatype: " << type2str(featuresUnclustered.type());
    
    
    
    
    // TRAIN BAG OF WORDS = kMeans clustering
    
    
    TermCriteria tc(TermCriteria::MAX_ITER + TermCriteria::EPS,100,0.001);
    // term criteria: 100 iterations or 0.001 epsilon = when BoW kMeans should exit

    int flags = KMEANS_PP_CENTERS; // kMeans ++, more advanced centroid origins
    
    // BoW trainer object
    
    BOWKMeansTrainer bowTrainer(nWords,tc,nIters,flags);
    
    
    ofLogNotice("BoW Trainer") << "started clustering";
    
    dictionary = bowTrainer.cluster(featuresUnclustered); // cluster the results into vocabulary
    
    ofLogNotice("BOW Trainer") << "complete - dictionary stored in cols: " << dictionary.cols << ", rows: " << dictionary.rows << ", datatype: " << type2str(dictionary.type());
    
    
    if (bSave){
    
        FileStorage fs(ofToDataPath(dictionaryFile), FileStorage::WRITE);
        fs << "vocabulary" << dictionary;
        
        ofLogNotice("BoW Trainer") << "saved " << dictionaryFile;
        
    }
    
}


//-------------------------------------

// DESCRIBER, GENERATES HISTOGRAMS BASED ON VOCAB & SAVES TO DISK

void BagOfWords::describe(int nImgs, bool bDense){
    
    
    if (nImgs < 0 || nImgs > imgPaths->size()) // check if -1 (all images) or nImgs out of bounds
        nImgs = imgPaths->size(); // describe all images

    
    FileStorage fs(ofToDataPath(descriptorsFile), FileStorage::WRITE); // prep file storage
    
    // clear/prep descriptors storage Mat based on dictionary size
    descriptors = Mat::zeros(0,dictionary.cols,CV_32F);
    
    // clear feature map
    
    features.clear();
    
    
    // SETUP Bag of Words
    
    Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher); // FLANN matcher (kNN)
    
    Ptr<FeatureDetector> detector(new SiftFeatureDetector()); // SIFT detector
    
    Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor); // SIFT extractor
    
    BOWImgDescriptorExtractor bowDE(extractor,matcher); // BoW using above
    
    bowDE.setVocabulary(dictionary); // set vocab
    
    if (bDense){
        ofLogNotice("BoW Descriptor") << "using dense keypoints";
    } else {
        ofLogNotice("BoW Descriptor") << "using auto SIFT detector keypoints";
    }
    
    // LOOP through images and describe them
    
    for (int i=0; i<nImgs; i++){
        
        ofImage img;
        img.load((*imgPaths)[i]); // load from file
        img.setImageType(OF_IMAGE_GRAYSCALE); // cvt to grayscale
        Mat imgMat = toCv(img); // prep the image as a Mat
        
        
        vector<KeyPoint> keypoints; // SIFT keypoints storage
        
        if (bDense) {
            
            // use dense keypoints (regular grid)
            
            int step = 10; // 10 pixels spacing between kp's
            
            for (int y=step; y<imgMat.rows-step; y+=step)
            {
                for (int x=step; x<imgMat.cols-step; x+=step)
                {
                    // x,y,radius
                    keypoints.push_back(KeyPoint(float(x), float(y), float(step)));
                }
            }
            
        } else {
            
            // or auto-detect keypoints
            
            detector->detect(imgMat,keypoints); // detect keypoints

        }
        
        
        Mat histogram; // img response to BoW vocab
        
        bowDE.compute(imgMat,keypoints,histogram); //extract BoW (or BoF) descriptor from given image
        
        
        if (histogram.cols > 0){ // if there was any data
        
            // write histogram to descriptor file
            
            string imageTag = ofFilePath::removeExt(ofFilePath::getFileName((*imgPaths)[i])); // image name
            fs << (imageTag).c_str() << histogram;
            
            
            // save histogram
            
            descriptors.push_back(histogram);
            
            // save to feature map
            vector<float> hist;
            hist.assign((float*)histogram.datastart, (float*)histogram.dataend);
            features[imageTag] = hist;
        }
        
        else {
            
            ofLogError("BoW Descriptor") << endl << "Blank histogram response at [ " << (*imgPaths)[i] << " ]!" << endl;
        }
        
        ofLogNotice("BoW Descriptor") << "percent done: " << (float)i/(float)nImgs; // print the % done
        
    }
    
    fs.release();
    
    ofLogNotice("BoW Descriptor") << "saved " << descriptors << "with cols: " << descriptors.cols << ", rows: " << descriptors.rows << ", datatype: " << type2str(descriptors.type());
    
    
}

//-------------------------------------

void BagOfWords::describeSpatialPyramid(int nImgs, int nDivsX, int nDivsY, bool bAddFull, bool bDense){
    
    
    if (nImgs < 0 || nImgs > imgPaths->size()) // check if -1 (all images) or nImgs out of bounds
        nImgs = imgPaths->size();
    
    
    FileStorage fs(ofToDataPath(descriptorsFile), FileStorage::WRITE); // prep file storage
    
    // clear/prep descriptors storage Mat based on dictionary size * grid area
    descriptors = Mat::zeros(0,dictionary.cols*nDivsX*nDivsY,CV_32F);
    
    // clear feature maps
    
    features.clear();
    featuresByRoi.clear();
    
    
    // SETUP Bag of Words
    
    Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher); // FLANN matcher (kNN)
    
    Ptr<FeatureDetector> detector(new SiftFeatureDetector()); // SIFT detector
    
    Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor); // SIFT extractor
    
    BOWImgDescriptorExtractor bowDE(extractor,matcher); // BoW using above
    
    bowDE.setVocabulary(dictionary); // set vocab
    
    if (bDense){
        ofLogNotice("BoW SP Descriptor") << "using dense keypoints";
    } else {
        ofLogNotice("BoW SP Descriptor") << "using auto SIFT detector keypoints";
    }
    
    
    // LOOP through images and describe them
    
    for (int i=0; i<nImgs; i++){
        
        ofImage img;
        img.load((*imgPaths)[i]); // load from file
        img.setImageType(OF_IMAGE_GRAYSCALE); // cvt to grayscale
        Mat imgMat = toCv(img); // prep the image as a Mat
        
        Mat histogram(1,0,CV_32F); // concatenated roi histograms response to BoW vocab
        
        string imageTag = ofFilePath::removeExt(ofFilePath::getFileName((*imgPaths)[i])); // image name
        // for featureMaps and filesaves
        
        vector<vector<float>> histRoiVecs; // stores vector of roi features per image
        
        bool goodHist = true;
        
        
        if (bAddFull){
            
            // add response of whole image to histogram first
            
            vector<KeyPoint> keypoints; // SIFT keypoints storage
            
            detector->detect(imgMat,keypoints); // detect keypoints
            
            if (bDense) {
                
                // use dense keypoints (regular grid)
                
                int step = 10; // 5 pixels spacing between kp's
                
                for (int y=step; y<imgMat.rows-step; y+=step)
                {
                    for (int x=step; x<imgMat.cols-step; x+=step)
                    {
                        // x,y,radius
                        keypoints.push_back(KeyPoint(float(x), float(y), float(step)));
                    }
                }
                
            } else {
                
                // or auto-detect keypoints
                
                detector->detect(imgMat,keypoints); // detect keypoints
                
            }
            
            Mat hist; // img response to BoW vocab
            
            bowDE.compute(imgMat,keypoints,hist); //extract BoW (or BoF) descriptor from given image
            
            
            if (hist.cols > 0){ // if there was any data
                
                // save histogram
                
                hist.copyTo(histogram); // initialize histogram
                
            }
            
            else {
                
                ofLogError("BoW SP Descriptor") << endl << "Blank histogram response at [ " << (*imgPaths)[i] << " ]!" << endl;
                
                // flag (skip spatial pyramid)
                
                bool goodHist = false;
            }
            
            
        }
        
        
        if (goodHist){
            
            
            // iterate over spatial pyramid
            
            for (int r=0; r<nDivsY; r++){ // x
                for (int c=0; c<nDivsX; c++){ // y
            
                    // generate mask
                    int x = imgMat.cols / nDivsX * c; // x
                    int y = imgMat.rows / nDivsY * r; // y
                    int w = imgMat.cols / nDivsX; // width
                    int h = imgMat.rows / nDivsY; // height
                    
                    cv::Rect roi( x,y, w,h ); // selected rectangle for mask (region of interest)
                    
                    cout << "mask " << r*nDivsX+c << ": " << x << "," << y << " : " << w << "," << h << endl;
                    
                    Mat imgRoi = imgMat(roi); // apply roi rect to imgMat
            
            
                    vector<KeyPoint> keypoints; // SIFT keypoints storage
                    
                    if (bDense) {
                        
                        // use dense keypoints (regular grid)
                        
                        int step = 5; // 5 pixels spacing between kp's
                        
                        for (int y=step; y<=imgRoi.rows-step; y+=step)
                        {
                            for (int x=step; x<=imgRoi.cols-step; x+=step)
                            {
                                // x,y,radius
                                keypoints.push_back(KeyPoint(float(x), float(y), float(step)));
                            }
                        }
                        
                    } else {
                        
                        // or auto-detect keypoints
                        
                        detector->detect(imgRoi,keypoints); // detect keypoints
                        
                    }
                    
                    Mat histRoi; // imgRoi response to BoW vocab
                    
                    bowDE.compute(imgRoi,keypoints,histRoi); //extract BoW (or BoF) descriptor from given image
                    
                    if (histRoi.cols < 1){

                        ofLogError("BoW Descriptor") << endl << "Blank histogram response at [ " << (*imgPaths)[i] << " ] and " << r*nDivsX+c << "th spatial pyramid" << endl;
                        goodHist = false;
                        break;
                    }
                    
                    
                    // otherwise add histRoi to histogram - horizontally
                    
                    if (histogram.empty()){
                        histRoi.copyTo(histogram); // either initialize histogram
                    } else {
                        Mat concatHist;
                        hconcat(histogram,histRoi,concatHist); // on concatenate
                        concatHist.copyTo(histogram);
                    }
                    
                    // save roi histogram to featuresByRoi vector
                    vector<float> histRoiVec;
                    histRoiVec.assign((float*)histRoi.datastart, (float*)histRoi.dataend);
                    histRoiVecs.push_back(histRoiVec);
                }
                
                if (!goodHist) break; // break out of ROI for loop
            }
        }
        
        // done with img description
        // now check the histogram for completeness and flags
        
        if (histogram.cols > 0){ // if got data
            
            if (goodHist){ // only save if no flag
                
                // write histogram to descriptor file
                
                fs << (imageTag).c_str() << histogram;
                
                
                // save histogram
                
                descriptors.push_back(histogram);
                
                // save to feature maps
                vector<float> hist;
                hist.assign((float*)histogram.datastart, (float*)histogram.dataend);
                features[imageTag] = hist; // concatenated histogram
                featuresByRoi[imageTag] = histRoiVecs; // vector of roi histograms

            }
            
        } else {
            
            ofLogError("BoW SP Descriptor") << endl << "Blank histogram response at [ " << (*imgPaths)[i] << endl;
            
        }
        
        ofLogNotice("BoW SP Descriptor") << "percent done: " << (float)i/(float)nImgs; // print the % done
        
    }
    
    fs.release();
    
    ofLogNotice("BoW Spatial Pyramid Descriptor") << "saved " << descriptors << "with cols: " << descriptors.cols << ", rows: " << descriptors.rows << ", datatype: " << type2str(descriptors.type());
    
}





//-------------------------------------

// UTILITY FUNCTIONS


void BagOfWords::loadDictionary(string _dictionaryFile){
    
    dictionaryFile = _dictionaryFile;
    
    ofLogNotice("BoW") << "getting dictionary from  " << dictionaryFile << endl;
    
    FileStorage fs(ofToDataPath(dictionaryFile), FileStorage::READ);
    Mat _dictionary;
    fs["vocabulary"] >> _dictionary;
    _dictionary.copyTo(dictionary);
    fs.release();
    
}



void BagOfWords::loadImages(vector<string>& _imgPaths){
    
    imgPaths = &_imgPaths;
}




void BagOfWords::loadDescriptors(string _descriptorsFile){
    
    // load BoW descriptors
    
    descriptorsFile = _descriptorsFile;
    
    FileStorage descFile(ofToDataPath(descriptorsFile), FileStorage::READ);
    
    FileNode imgsBoW = descFile.root();
    ofLogNotice("file") << "size: " << imgsBoW.size(); // prints size of file (# images)
    
    FileNodeIterator it = imgsBoW.begin();
    FileNodeIterator it_end = imgsBoW.end();
    int idx = 0;
    
    for( ; it != it_end; ++it, idx++ ){
        
        string imgName = (*it).name(); // get image name
        cout << imgName << endl;
        
        Mat data;
        descFile[imgName] >> data; // get mat data
        
        
        // if data is empty
        
        if (data.cols == 0){
            cout << "empty data" << endl; // alert and ignore
        }
        
        
        vector<float> hist; // store histogram (descriptor)
        
        if (data.isContinuous()) {
            
            hist.assign((float*)data.datastart, (float*)data.dataend); // < converts 1d mat to vector
            
            features[imgName] = hist; // save to map
            
            cout << "[" << idx << "][0]: " << features[imgName][0] << endl;
            
        } else {
            cout << "data not continuous!" << endl;
        }
        
    }
    
}

void BagOfWords::deriveFeaturesByRoi(int nRoi){
    
    featuresByRoi.clear();
    
    for( map<string,vector<float>>::iterator it = features.begin(); it != features.end(); ++it ) {

        string imgTag = it->first;
        vector<float>& histogram = it->second;
        
        vector<vector<float>> histograms;
        
        int subSize = it->second.size() / nRoi;
        
        vector<float>::iterator first = histogram.begin();
        vector<float>::iterator last = first + subSize;
        
        while (last <= histogram.end()){
            
            // copy into new vector
            vector<float> histRoi(first,last);
            histograms.push_back(histRoi);
        
            first = last;
            last += subSize;
            
        }
        
        featuresByRoi[imgTag] = histograms;
        
        ofLogNotice("deriveFeaturesByRoi") << " pushed " << imgTag << " with " << featuresByRoi[imgTag].size() << " vectors of size: " << featuresByRoi[imgTag][0].size();
    }
    
    ofLogNotice("deriveFeaturesByRoi") << " finished with " << featuresByRoi.size() << " map count";
}



string BagOfWords::type2str(int type) {
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