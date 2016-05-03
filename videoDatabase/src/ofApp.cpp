#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    string featuresFile = "segment_testingDescriptors.yml"; // contains img descriptors
    string flannIndexFile = "flannIndex_segment_testing"; // filename to save flann indexes
    string videoMapImgFolder = "segment_testing_sm"; // img folder for reading img names
    
    vidDB = VideoDatabase(featuresFile, flannIndexFile, 9, true);
    
    vidDB.loadVideo("blow_up_segment-test_photojpeg.mov", "blow_up");
    vidDB.loadVideo("peeping_tom_segment-test_photojpeg.mov", "peeping_tom");
    vidDB.loadVideo("rear_window_segment-test_photojpeg.mov", "rear_window");
    
    setupRois(3,3);
    
    vidDB.startVideo(); // start playing at random frame of random video?

}

//--------------------------------------------------------------
void ofApp::update(){
    
    float mouseX = ofGetMouseX();
    float mouseY = ofGetMouseY();
    float scnW = ofGetWidth();
    float scnH = ofGetHeight();
    
    float cropW = 200;
    float cropH = 200;
    
    setCurrentRoi(ofGetMouseX(),ofGetMouseY());
    
    //vidDB.update(currentRoi); // update vid and offer gaze position (as an roi)
    ofImage grab;
    
    // crop values
    
     // center crop on mouse
    int x = mouseX - cropW * 0.5;
    int y = mouseY - cropH * 0.5;
    
    // unless mouse is too close to edge -
    
    // left edge
    if ( x < 0 ) { x = 0; }
    // right edge
    else if ( x + cropW > scnW ) { x = scnW - cropW; }
    
    // top edge
    if ( y < 0 ) { y = 0; }
    // bottom edge
    else if ( y + cropH > scnH ) { y = scnH - cropH; }

    grab.grabScreen(x,y, cropW,cropH); // grab screen crop to ofImage
    
    vidDB.update(grab); // does BoW on screen crop, finds NN
    
    // if grabScreen() too slow, try drawing into Fbo?
    // but still need readPixels() from texture - same problem as grabScreen()
    // solution? maybe only grab screen when eye is focused on position
    
    /*
     // Crop FBO:
     
     fbo1.begin();
        video.draw();
     fbo1.end();
     
     fbo2.allocate(cropped_w,cropped_h,GL_RGBA);
     fbo2.begin();
        fbo1.getTexture().drawSubsection(0,0,cropped_w,cropped_h, cropped_x,cropped_y,cropped_w,cropped_h);
        // draws into (0,0,cropped_w,cropped_h) the crop of fbo1
     fbo2.end();
     */

    

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(255);
    vidDB.draw();
    
    ofSetColor(255,20);
    ofDrawRectangle(rois[currentRoi]);

}

//--------------------------------------------------------------
void ofApp::setupRois(int nRoisX, int nRoisY){
    
    ofLogNotice("setupRois") << "setting up " << nRoisX << "x rois, " << nRoisY << "y rois from " << ofGetWidth() << "," << ofGetHeight();
    
    float roiWidth = ofGetWidth()/nRoisX;
    float roiHeight = ofGetHeight()/nRoisY;
    
    for (int rY=0; rY<nRoisY; rY++){
        for (int rX=0; rX<nRoisX; rX++){
            
            int x = rX * roiWidth;
            int y = rY * roiHeight;
            
            rois.push_back(ofRectangle());
            rois.back().set(x, y, roiWidth, roiHeight);
            
            ofLogNotice("setupRois") << "roi " << rY*nRoisX + rX << " x,y - w,h: " << x << "," << y << " - " << roiWidth << "," << roiHeight;
            
        }
    }
    
}

void ofApp::setCurrentRoi(float x, float y){
    
    int roiIdx = 0;
    for (auto roi = rois.begin(); roi != rois.end(); ++roi){
        if (roi->inside(x,y)){
            currentRoi = roiIdx;
            break;
        }
        roiIdx++;
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
