#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetFrameRate(24);
    
    // CHANGE THESE PER VIDEO/SEGMENT IMGS
    
    imgsPath = "blow_up"; // folder inside bin/data
    vidPath = "blow_up_segment-test_photojpeg.mov"; // video file
    
    vid.load(vidPath);
    
    getShots(); // analyzes img filenames, loads shots into shotTimes vector
 
    saveShots(); // saves results of getShots (shots vector) to csv
    
    vid.play();

}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (bSaveShotsImgsMode){
        
        if (currentSaveShot <= lastShotIdx) {
            
            // set save image path
            
            string saveImgPath = imgsPath + "_shotFrames/";
            saveImgPath += ofToString(currentSaveShot);
            if (bSaveStart){
                saveImgPath += "_0start_" + ofToString(shots[currentSaveShot].start) + ".jpg";
            } else {
                saveImgPath += "_1end_" + ofToString(shots[currentSaveShot].end) + ".jpg";
            }
            
            // seek to correct frame (shot start or end)
            
            if (bSaveStart){
                seekShot(currentSaveShot, true); // seek to start frame of shot
            } else {
                seekShot(currentSaveShot, false); // seek to end frame of shot
            }
            
             // save frame to jpeg
            
            ofImage saveImg;
            saveImg.setFromPixels(vid.getPixels(), vid.getWidth(), vid.getHeight(), OF_IMAGE_COLOR);
            saveImg.save(ofToDataPath(saveImgPath)); // save image
            
            ofLogNotice() << "saved " << saveImgPath;
            
            if (bSaveStart){
                bSaveStart = false;
            } else {
                currentSaveShot++;
                bSaveStart = true;
            }

        }
        
        else {
            ofLogNotice() << "save shot frames complete";
            currentSaveShot = 0;

            bSaveShotsImgsMode = false;
            vid.play();
        }
        
    }
    
    else {
        
        vid.update();

    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    vid.draw(0,0,1280,720);
    
    ofDrawBitmapString("shot: " + ofToString(currentShot),10, ofGetHeight()-90);
    ofDrawBitmapString("frame: " + ofToString(currentFrame),10, ofGetHeight()-70);

}

//--------------------------------------------------------------

void ofApp::getShots(){
    
    imgDir = ofDirectory(imgsPath);
    imgDir.allowExt("jpg");
    imgDir.listDir();
    
    vector<ofFile> imgs = imgDir.getFiles();
    
    shotTimes.clear();
    shotTimes.resize(imgs.size());
    
    // loop through files and get start and end times
    for (int i=0; i<imgs.size(); i++){
        
        string fn = imgs[i].getFileName();
        times t = getTimes(fn);
        shotTimes[t.shot] = t;
        
    }
    
    // convert start and end times to frame nums
    
    int nFrames = vid.getTotalNumFrames();
    float dur = vid.getDuration();
    
    for (int i=0; i<shotTimes.size(); i++){
        
        frames f;
        f.shot = shotTimes[i].shot;
        
        float startPct = shotTimes[i].start / dur;
        float endPct = shotTimes[i].end / dur;
        f.start = startPct * nFrames+1;
        f.end = endPct * nFrames-1; // way off for some reason, hack fix below
        
        shots.push_back(f);
    }
    
    // find end frames
    
    for (auto it = shots.begin(); it<shots.end(); it++){
        if (it==shots.end()-1){
            it->end = nFrames; // total frames in vid is end of last shot
        }
        else if (it->end <= it->start){
            it->end = it->start +1;
        }
//        else {
//            it->end = (it+1)->start-24;
//            if (it->end < it->start || it->end <= 0){
//                it->end = it->start+1;
//            }
//        }
    }
}

//--------------------------------------------------------------
void ofApp::saveShots(){
    
    csv.clear();
    
    // save results to CSV file
    
    for (int i=0; i<shots.size(); i++){
        
        csv.setInt(i,0,shots[i].shot);
        csv.setInt(i,1,shots[i].start);
        csv.setInt(i,2,shots[i].end);
        
        // print result
        ofLogNotice() << "shot " << shots[i].shot <<
        " - start: " << shots[i].start <<
        ", end: " << shots[i].end;
    }
    string savePath = removeExt(vidPath) + "_shots.csv";
    csv.saveFile(ofToDataPath(savePath));
    
}

//--------------------------------------------------------------
void ofApp::saveShotsImgs(){
    
    vid.setPaused(true);
    vid.firstFrame();
    currentFrame = 0;
    currentShot = 0;
    currentSaveShot = 0;
    
    lastShotIdx = shots.size()-1;
    
    ofLogNotice("saveShotsImgs") << lastShotIdx +1 << " shot frames to save...";
    
    bSaveShotsImgsMode = true;
    vid.play();
    
}


//--------------------------------------------------------------
ofApp::times ofApp::getTimes(string fn){
    
    vector<string> split1 = ofSplitString(fn,"@");
    vector<string> split2 = ofSplitString(split1[2],"_"); // removes "_K.jpg"
    vector<string> secs = ofSplitString(split2[0],"-"); // splits secs
    
    times t;
    t.shot = ofFromString<int>(split1[1]);
    t.start = ofFromString<float>(secs[0]);
    t.end = ofFromString<float>(secs[2]);
    
    return t;
}

//--------------------------------------------------------------
void ofApp::seekShot(int shot, bool bStart){
    
    if (bStart){
        vid.setFrame(shots[shot].start); // set to start frame of shot
    } else {
        vid.setFrame(shots[shot].end); // or end frame
    }
    
    vid.update();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    // frame movement
    if (key==OF_KEY_RIGHT){
        vid.nextFrame();
    }
    else if (key==OF_KEY_LEFT){
        vid.previousFrame();
    }
    
    // shot movement
    else if (key=='a'){ // previous shot
        if (currentShot > 0)
            seekShot(--currentShot);
        else {
            currentShot = shots.size()-1;
            seekShot(currentShot);
        }
    }
    else if (key=='s'){ // next shot
        if (currentShot < shots.size()-1)
            seekShot(++currentShot);
        else {
            currentShot = 0;
            seekShot(0);
        }
    }
    else if (key=='d'){ // end frame
        seekShot(currentShot, false);
    }
    else if (key == 'p'){
        bVidPause = !bVidPause;
        vid.setPaused(bVidPause);
    }
    else if (key=='f'){
        saveShotsImgs();
    }

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
