#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(0);
    
    imgFolder = "thesiscut";
    dictionaryFile = "dictionaryTestDense2.yml";
    descriptorsFile = "descriptorsTestDense.yml";
    int nWords = 2000;
    
    
    loadImgPaths(imgPaths, ofToDataPath(imgFolder));

    BoW = BagOfWords(imgPaths,
                     nWords, // # of words
                     dictionaryFile,
                     descriptorsFile);
    
    BoW.train(0.4, 3, true, true);
    // % imgs to extract, nIters (kMeans) = 3, save to yml = true, dense sift = true
    
    //BoW.loadDictionary(dictionaryFile);
    
    BoW.describe(true); // dense = true
    
    BoW.loadDescriptors(descriptorsFile);
    
    loadImgsandFeatures(BoW.getFeatureMap());

    ofLogNotice("setup()") << "features vector size: " << features.size();

    // run t-SNE and load image points to imagePoints
    
    ofLogNotice("setup()") << "Run t-SNE on images - concatenated features";
    
    imgPts = tsne.run(features, 2, 25, 0.1, true);
    
    // setup gui
    gui.setup();
    gui.add(scale.set("scale", 4.0, 0.0, 10.0));
    gui.add(imageSize.set("imageSize", 0.5, 0.0, 1.0));

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPushMatrix();
    ofTranslate(-ofGetMouseX() * (scale - 0.5), -ofGetMouseY() * (scale - 0.5));
    for (int i=0; i<imgPts.size(); i++) {
        float x = ofMap(imgPts[i][0], 0, 1, 0, scale * ofGetWidth());
        float y = ofMap(imgPts[i][1], 0, 1, 0, scale * ofGetHeight());
        imgs[i].draw(x, y, imageSize * imgs[i].getWidth(), imageSize * imgs[i].getHeight());
    }
    ofPopMatrix();
    
    gui.draw();

}

//* UTILS *//

void ofApp::loadImgPaths(vector<string>& _imgPaths, string folder, int nImgs){
    
    // load img paths into string vector
    
    ofLogNotice("Image path loader") << "Gathering images paths...";
    
    ofDirectory dir;
    dir.allowExt("png");
    dir.allowExt("jpg");
    dir.allowExt("gif");
    int nFiles = dir.listDir(folder);
    
    if (nImgs < 0 || nImgs > nFiles) nImgs = nFiles; // check if load all flag or out of bounds
    
    if (nFiles) {
        for(int i=0; i<nImgs; i++) {
            
            if (i % 20 == 0)
                ofLogNotice("Image loader") << "percent done: " << (float)i/(float)nImgs << " at image "<< i <<" / "<< nImgs;
            
            if (i == 0) _imgPaths.clear(); // clear the vector if 1 img
            string path = dir.getPath(i);
            _imgPaths.push_back(path);
        }
    }
    
}

void ofApp::loadImgsandFeatures(map<string,vector<float>>& featureMap){
    
    
    // load imgs into ofImage vector
    // load each vector<float> into features vector
    
    ofLogNotice("Image loader") << "Loading images based on feature map...";
    
    int idx = 0;
    
    for( map<string,vector<float>>::iterator it = featureMap.begin(); it != featureMap.end(); ++it ) {
        
        if (idx % 20 == 0)
            ofLogNotice("Image loader") << "loading image "<< idx <<" / "<< featureMap.size();
        
        if (idx == 0) {
            imgs.clear(); // clear the vector if 1st img
            features.clear();
        }
        
        
        ofImage img;
        imgs.push_back(img);
        
        string imgPath = imgFolder + "/" + it->first + ".jpg";
        cout << "loading image: " << imgPath << endl;
        
        imgs.back().load(imgPath); // relative to bin/data
        
        float scale = 200.0 / imgs.back().getWidth();
        imgs.back().resize(200,scale*imgs.back().getHeight()); // scale down
        
        
        // add features to features vector
        
        features.push_back(it->second);
        
        idx++;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
