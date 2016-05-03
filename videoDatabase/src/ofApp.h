#pragma once

#include "ofMain.h"
#include "VideoDatabase.hpp"
#include "FlannCluster.hpp"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void setupRois(int nRoisX, int nRoisY);
    void setCurrentRoi(float x, float y);

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    VideoDatabase vidDB;
    
    vector<ofRectangle> rois;
    int currentRoi;
		
};
