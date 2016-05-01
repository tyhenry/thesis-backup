#pragma once

#include "ofMain.h"
#include "filmFaceDetector.hpp"


class ofApp : public ofBaseApp{

public:
    string removeDirs(string path){
        vector<string> split = ofSplitString(path, "/", true);
        return split.back();
    }
    string getExt(string path){
        vector<string> split = ofSplitString(path, ".", true);
        return split.back();
    }
    
    void setup();
    void update();
    void draw();
    void start();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y){};
    void mouseExited(int x, int y){};
    void windowResized(int w, int h){};
    void dragEvent(ofDragInfo dragInfo);
    
    vector<FilmFaceDetector> detectors;
    int currentFilm = 0;
    vector<string> films;
    vector<string> shotCsvs;
    
    ofRectangle startButton;
    ofColor buttonColor = ofColor(100);
    bool run = false;

		
};
