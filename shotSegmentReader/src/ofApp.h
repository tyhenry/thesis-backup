#pragma once

#include "ofMain.h"
#include "ofxCsv.h"

using namespace wng;

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
    string removeExt(string path){
        vector<string> split = ofSplitString(path, ".", true);
        return split[0];
    }
    
    struct times{
        // shot number
        int shot;
        // start + end times of shot in seconds
        float start;
        float end;
    };
    
    struct frames{
        // shot number
        int shot;
        // start + end frames of shot
        int start;
        int end;
    };
    
    void setup();
    void update();
    void draw();
    
    void getShots();
    void saveShots();
    void saveShotsImgs(); // saves start and end frames of each shot as jpg
    
    times getTimes(string fn);
    void seekShot(int shot, bool bStart = true);

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
    
    string imgsPath, vidPath;
    
    ofDirectory imgDir;
    ofVideoPlayer vid;
    
    int currentShot = 0;
    int currentFrame = 0;
    
    vector<times> shotTimes;
    vector<frames> shots;
    
    ofxCsv csv;
    
    bool bSaveShotsImgsMode = false;
    int currentSaveShot = 0;
    int lastShotIdx = 0;
    bool bSaveStart = true;
    
    bool bVidPause = false;
		
};
