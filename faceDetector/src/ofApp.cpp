#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(0);
    //ofSetFrameRate(120);
    
    shotsCsv.loadFile(ofToDataPath("shots.csv"));
    
    currentShot = 0;
    currentFrame = 0;
    
    // read shots.csv
    
    for (int r=0; r<shotsCsv.numRows; r++){
        
        shot s;
        s.shotNum = shotsCsv.getInt(r,0);
        s.start = shotsCsv.getInt(r,1);
        s.end = shotsCsv.getInt(r,2);
        shots.push_back(s);
    }
    
    // 1 ObjectFinder per shot
    for (int s=0; s<shots.size(); s++){
        finders.push_back(ObjectFinder());
        finders.back().setup("visionary_FACES_01_LBP_5k_7k_50x50.xml");
        finders.back().setPreset(ObjectFinder::Sensitive);
        finders.back().setUseHistogramEqualization(true);
        finders.back().setMinSizeScale(.05); //AFTER rescale
        finders.back().setCannyPruning(true);
        finders.back().setMinNeighbors(3);
    }
    
    vidFile = "THESIS_CUT_1_SMALL.mov";
    
    vid.load(vidFile);
    vid.setLoopState(OF_LOOP_NONE);
    vid.play();

}

//--------------------------------------------------------------
void ofApp::update(){
//    
//    if (currentFrame == 0){
//        vid.setPaused(true); // hack to actually load video if paused
//    }
    
    vid.update();

    
    if (!done){
        
        // movie done? save csv
        
        if (vid.getIsMovieDone()){
            saveFacesCsv();
            ofLogNotice() << "saved faces csv";
            done = true;
        }
        
        if (!done && vid.isFrameNew()){
            
            currentFrame = vid.getCurrentFrame();
            if (currentFrame < 0){
                currentFrame = 0;
            }
            
            // update shot #
            
            if (currentFrame > shots[currentShot].end){
                currentShot++;
            }
            
            // detect faces
            
            finders[currentShot].update(vid);
            
            // store faces
            
            storeFaces(); // also saves face imgs
            
        }
        
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    vid.draw(0,0);
    
    finders[currentShot].draw();
    
    ofPushMatrix();
    ofTranslate(0,ofGetHeight()-100);
    
    ofDrawBitmapString("shot: " + ofToString(currentShot),10,50);
    ofDrawBitmapString("frame: " + ofToString(currentFrame),10,65);
    ofDrawBitmapString("# faces: " + ofToString(finders[currentShot].size()),10,80);
    
    ofPopMatrix();

}

//--------------------------------------------------------------
void ofApp::storeFaces(){
    
    frame fr;
    fr.shotNum = currentShot;
    fr.frame = currentFrame;
    
    if (fr.frame < 0){
        ofLogError("storeFaces") << "frame readout! " << fr.frame;
    }
    
    // save faces to vector
    
    fr.faces.clear();
    
    for (int i=0; i<finders[currentShot].size(); i++){
        
        fr.labels.push_back(finders[currentShot].getLabel(i));
        fr.faces.push_back(finders[currentShot].getObject(i));
        
        
        // save face image
            
        // get face rect x,y,w,h
        
        ofRectangle& face = fr.faces.back();
        float x = face.getX();
        float y = face.getY();
        float w = face.getWidth();
        float h = face.getHeight();
            
        ofImage img;
        img.setFromPixels(vid.getPixels(), vid.getWidth(), vid.getHeight(), OF_IMAGE_COLOR);
        
        string frameStr = ofToString(currentShot) + "_" + ofToString(currentFrame) + "__";
        
        img.crop(x,y,w,h);
        string imgFile = "faces_" + vidFile + "/" + frameStr + ofToString(fr.labels.back()) + ".jpg";
        img.save(ofToDataPath(imgFile));
        
        ofLogNotice("storeFaces") << "saved " << imgFile;
        
        
        
    }
    
    frames.push_back(fr);
    
    // print status
    ofLogNotice() << "shot " << currentShot << " / frame " << currentFrame << " - " << fr.faces.size() << " faces";
    
}

//--------------------------------------------------------------
void ofApp::saveFacesCsv(){
    
    // setup facesCsv
    
    for (int f=0; f<frames.size(); f++){
        // store shot, frame #, # faces in csv
        
        int fN = frames[f].frame;
        int sN = frames[f].shotNum;
        vector<ofRectangle>& faces = frames[f].faces;
        vector<unsigned int>& labels = frames[f].labels;
        
        ofLogNotice("saveFaces") << "shot " << sN << " / frame " << fN << " - " << (int)faces.size() << " faces";
        
        facesCsv.setInt(f,0,(int)sN); // shot
        facesCsv.setInt(f,1,(int)fN); // frame #
        facesCsv.setInt(f,2,(int)faces.size()); // # faces in frame
        
        
        // store faces coords + sizes in csv
        
        int col = 3;
        
        for (int fa=0; fa<faces.size(); fa++){
            
            ofLogNotice("saveFaces") << "saving face " << fa;
            facesCsv.setInt(f,col, (int)labels[fa]); // save label
            
            // save rect x,y,w,h
            
            ofRectangle& face = faces[fa];
            facesCsv.setFloat(f,col+1, face.getX());
            facesCsv.setFloat(f,col+2, face.getY());
            facesCsv.setFloat(f,col+3, face.getWidth());
            facesCsv.setFloat(f,col+4, face.getHeight());
            
            col+=5;
        }
    }
    
    // save facesCsv
    
    facesCsv.saveFile(ofToDataPath(vidFile + "__faces_LBP-sensitive.csv"));
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
