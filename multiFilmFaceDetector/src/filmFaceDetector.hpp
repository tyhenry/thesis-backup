//
//  filmFaceDetector.hpp
//  multifilmFaceDetector
//
//  Created by Tyler on 4/23/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxCsv.h"

using namespace cv;
using namespace ofxCv;
using namespace wng;

class FilmFaceDetector {
    
public:
    
    struct shot {
        int shotNum; // shot #
        int start; // start frame
        int end; // end frame
    };
    
    struct frame {
        int shotNum;
        int frame;
        vector<unsigned int> labels;
        vector<ofRectangle> faces;
    };
    
    FilmFaceDetector();
    FilmFaceDetector(string _filmPath, string _csvPath);
    
    void load(string _filmPath, string _csvPath);
    void start();
    void update();
    void draw(float x, float y, float w, float h, bool drawInfo = true);
    
    void storeFaces(); // push back to vector + saves face images
    void saveFacesCsv(); // save to csv
    
    ofVideoPlayer vid;
    
    ofxCsv shotsCsv, facesCsv;
    
    int currentShot, currentFrame, numShots;
    
    bool started = false;
    bool done = false;
    
    vector<ObjectFinder> finders; // ofxCv LBP cascade - 1 finder per shot
    
    vector<shot> shots;
    vector<frame> frames;
    
    
private:
    
    string removeDirs(string path){
        vector<string> split = ofSplitString(path, "/", true);
        return split.back();
    }
    string getExt(string path){
        vector<string> split = ofSplitString(path, ".", true);
        return split.back();
    }
    string removeExt(string path){
        vector<string> split = ofSplitString(path, ".", true);
        return split[0];
    }
    
    string filmPath, csvPath;
    
};
