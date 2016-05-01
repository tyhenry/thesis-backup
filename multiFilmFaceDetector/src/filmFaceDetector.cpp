//
//  filmFaceDetector.cpp
//  multifilmFaceDetector
//
//  Created by Tyler on 4/23/16.
//
//

#include "filmFaceDetector.hpp"


FilmFaceDetector::FilmFaceDetector(){
    
}

FilmFaceDetector::FilmFaceDetector(string _filmPath, string _csvPath){
    
    load(_filmPath,_csvPath);
    
}

void FilmFaceDetector::load(string _filmPath, string _csvPath){
    filmPath = _filmPath;
    csvPath = _csvPath;
    
    // read shots csv
    
    shotsCsv.loadFile(ofToDataPath(csvPath));
    
    for (int r=0; r<shotsCsv.numRows; r++){
        
        shot s;
        s.shotNum = shotsCsv.getInt(r,0);
        s.start = shotsCsv.getInt(r,1);
        s.end = shotsCsv.getInt(r,2);
        shots.push_back(s);
    }
    
    // init object finders
    
    for (int s=0; s<shots.size(); s++){
        finders.push_back(ObjectFinder());
        finders.back().setup("visionary_FACES_01_LBP_5k_7k_50x50.xml");
        finders.back().setPreset(ObjectFinder::Sensitive);
        finders.back().setUseHistogramEqualization(true);
        finders.back().setMinSizeScale(.05); //AFTER rescale
        finders.back().setCannyPruning(true);
        finders.back().setMinNeighbors(3);
    }
    
    vid.load(filmPath);
    vid.setLoopState(OF_LOOP_NONE);
    
    currentShot = 0;
    currentFrame = 0;
}

//--------------------------------------------------------------
void FilmFaceDetector::start(){
    
    vid.play();
    started = true;
    
}

//--------------------------------------------------------------
void FilmFaceDetector::update(){
    
    vid.update();
    
    if (!done){
        
        // movie done? save csv
        
        if (vid.getIsMovieDone()){
            saveFacesCsv();
            ofLogNotice("FilmFaceDetector") << "film done, saved faces csv";
            done = true;
        }
        
        // otherwise, detect faces on new frame
        
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
void FilmFaceDetector::draw(float x, float y, float w, float h, bool drawInfo){
    
    vid.draw(x,y,w,h);
    
    ofPushMatrix();
    ofTranslate(x,y); // translate to vid location
    ofScale(w/vid.getWidth(),h/vid.getHeight()); // scale to vid size
    
    finders[currentShot].draw();
    
    ofPopMatrix();
    
    if (drawInfo){
    
        // draw info
        
        ofPushMatrix();
        ofTranslate(x,y+h-80);
        ofPushStyle();
    
        ofSetColor(0,100);
        ofDrawRectangle(0,0,w,80);
        
        ofSetColor(255);
        
        ofDrawBitmapString("shot: " + ofToString(currentShot),10,30);
        ofDrawBitmapString("frame: " + ofToString(currentFrame),10,45);
        ofDrawBitmapString("# faces: " + ofToString(finders[currentShot].size()),10,60);
        
        ofPopStyle();
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void FilmFaceDetector::storeFaces(){
    
    frame fr;
    fr.shotNum = currentShot;
    fr.frame = currentFrame;
    
    if (fr.frame < 0){
        ofLogError("storeFaces") << "frame readout error! - frame # " << fr.frame;
    }
    
    // save faces to vector
    
    fr.faces.clear();
    
    for (int f=0; f<finders[currentShot].size(); f++){
        
        fr.labels.push_back(finders[currentShot].getLabel(f));
        fr.faces.push_back(finders[currentShot].getObject(f));
        
        
        // save face image
        
        // get face rect x,y,w,h
        
        ofRectangle& face = fr.faces.back();
        float x = face.getX();
        float y = face.getY();
        float w = face.getWidth();
        float h = face.getHeight();
        
        ofImage img;
        img.setFromPixels(vid.getPixels(), vid.getWidth(), vid.getHeight(), OF_IMAGE_COLOR);
        
        string filmName = removeExt(removeDirs(filmPath));
        string frameStr = ofToString(currentShot) + "_" + ofToString(currentFrame) + "__";
        
        img.crop(x,y,w,h);
        string imgFile = "faces_" + filmName + "/" + frameStr + ofToString(fr.labels.back()) + ".jpg";
        img.save(ofToDataPath(imgFile));
        
    }
    
    frames.push_back(fr);
    
    // print status
    ofLogNotice("FilmFaceDetector") << "shot " << currentShot << " / frame " << currentFrame << " - " << fr.faces.size() << " faces stored";
    if (fr.faces.size() > 0){
        string faceLog = "";
        for (int f=0; f<fr.faces.size(); f++){
            faceLog += "\n";
            faceLog += "\t\tface " + ofToString(f) + " x,y w,h: ";
            faceLog += ofToString(fr.faces[f].x) + "," + ofToString(fr.faces[f].y) + " " + ofToString(fr.faces[f].width) + "," + ofToString(fr.faces[f].height);
        }
        ofLogNotice("FilmFaceDetector") << faceLog;
    }
    
    
}

//--------------------------------------------------------------
void FilmFaceDetector::saveFacesCsv(){
    
    ofLogNotice("FilmFaceDetector") << "saving faces to csv";
    
    // loop through all frames in vector
    
    for (int f=0; f<frames.size(); f++){
        
        // store shot, frame #, # faces in csv
        
        int sN = frames[f].shotNum;
        int fN = frames[f].frame;

        vector<ofRectangle>& faces = frames[f].faces;
        vector<unsigned int>& labels = frames[f].labels;
        
        ofLogNotice("FilmFaceDetector") << "shot " << sN << " / frame " << fN << " - " << (int)faces.size() << " faces";
        
        facesCsv.setInt(f,0,(int)sN); // shot
        facesCsv.setInt(f,1,(int)fN); // frame #
        facesCsv.setInt(f,2,(int)faces.size()); // # faces in frame
        
        // store face labels + coords + sizes in csv
        
        int col = 3;
        
        for (int fa=0; fa<faces.size(); fa++){
            
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
    string filmName = removeExt(removeDirs(filmPath));
    facesCsv.saveFile(ofToDataPath(filmName + "__faces_LBP-sensitive.csv"));
    
}



