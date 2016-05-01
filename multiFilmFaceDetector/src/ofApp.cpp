#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(0);
    
    startButton = ofRectangle(ofGetWidth()/2-120,ofGetHeight()-50,240,40);

}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (run){
        
        // run detector
        
        detectors[currentFilm].update();
        
        // if this film is done, start next one
        
        if (detectors[currentFilm].done && currentFilm < detectors.size()-1){ // if film done and more films left
            
            detectors[++currentFilm].start(); // start next film
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if (!run){

        ofDrawLine(ofGetWidth()/2,0,ofGetWidth()/2,ofGetHeight());
        
        ofDrawBitmapString("Drag Films Here", 10,20);
        
        ofPushMatrix();
        ofTranslate(0,50);
        for (int f=0; f<films.size(); f++){
            
            ofDrawBitmapString(removeDirs(films[f]), 10,0);
            ofTranslate(0,20);
        }
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2,0);
        
        ofDrawBitmapString("Drag Shot CSVs Here", 10,20);
        ofTranslate(0,50);
        for (int s=0; s<shotCsvs.size(); s++){
            
            ofDrawBitmapString(removeDirs(shotCsvs[s]), 10,0);
            ofTranslate(0,20);
        }
        
        ofPopMatrix();
        

        ofSetColor(buttonColor);
        ofDrawRectangle(startButton);
        ofSetColor(255);
        ofDrawBitmapString("Click to Start Detecting", startButton.x+20, startButton.y+20);

    }
    
    else {
        
        detectors[currentFilm].draw(0,0,1280,720);
        
    }
    

}
    
//--------------------------------------------------------------
void ofApp::start(){
    
    if (films.size() != shotCsvs.size()){
        ofLogError("start detection") << "mismatch of files!";
        return;
    } else if (films.size() == 0 || shotCsvs.size() == 0){
        ofLogError("start detection") << "no films or no csvs!";
        return;
    }
    
    else {
        
        for (int f=0; f<films.size(); f++){
            
            string filmExt = getExt(films[f]);
            string csvExt = getExt(shotCsvs[f]);
            
            if ( filmExt != "mp4" && filmExt != "mov" ){
                
                ofLogError() << "incorrect extension! " << removeDirs(films[f]);
                detectors.clear();
                return; // exit
            } else if (csvExt != "csv"){
                ofLogError("start detection") << "incorrect extension! " << removeDirs(shotCsvs[f]);
                detectors.clear();
                return; // exit
            }
            
            
            else { // populate detector vector
                
                detectors.push_back(FilmFaceDetector());
                detectors.back().load(films[f],shotCsvs[f]);
            }
        
        }
        
        detectors[0].start();
        
        run = true;
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
    
    if (startButton.inside(x,y)){
        buttonColor = ofColor(200);
    }

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    if (startButton.inside(x,y)){
        start();
    }
    buttonColor = ofColor(100);

}


//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
    ofRectangle left(0,0,ofGetWidth()/2,ofGetHeight());
    ofRectangle right(ofGetWidth()/2,0,ofGetWidth(),ofGetHeight());
    
    if (left.inside(dragInfo.position)){
        films = dragInfo.files;
        for (int i=0; i<films.size(); i++){
            films[i] = ofToDataPath(films[i]);
        }
    } else if (right.inside(dragInfo.position)) {
        shotCsvs = dragInfo.files;
        for (int i=0; i<shotCsvs.size(); i++){
            shotCsvs[i] = ofToDataPath(shotCsvs[i]);
        }
    }

}
