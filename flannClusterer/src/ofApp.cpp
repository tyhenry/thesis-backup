#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    string featuresFile = "segment_testingDescriptors.yml";
    
    flann.loadFeaturesFile(featuresFile);
    
    // FULL IMAGES
    
    flann.makeIndex();
    Mat query = flann.getFeaturesAtIndex(100);
    cout << "searching img: " << flann.getImgAtIndex(100) << endl;
    Mat indices, distances;
    flann.knnSearch(query, 10, indices, distances);
    
    imgs = flann.getImgsAtIndices(indices);
    for (auto it = imgs.begin(); it!=imgs.end(); it++){
        cout << *it << endl;
    }
    
    // ROIS
    
    flann.splitFeaturesToRois(9);
    flann.makeRoiIndexes();
    Mat roiQuery = flann.getRoiFeaturesAtIndex(100, 8);
    cout << "searching roi 0 at img: " << flann.getImgAtIndex(100) << endl;
    Mat roiIndices, roiDistances;
    flann.knnSearchRoi(8, roiQuery, 10, roiIndices, roiDistances);
    
    roiImgs = flann.getImgsAtIndices(roiIndices);
    for (auto it=roiImgs.begin(); it!=roiImgs.end(); it++){
        cout << *it << endl;
    }
    
    // VIDEO MAP
    
    videoMap.loadImgFolder("segment_testing_sm");
    cout << videoMap.getImgAtIndex(100) << endl;
    videoMap.getTimesFromImgNames();
    videoMap.readoutShots();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

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
