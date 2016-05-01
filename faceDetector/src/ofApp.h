#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxCsv.h"

using namespace cv;
using namespace ofxCv;
using namespace wng;

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

class ofApp : public ofBaseApp{

public:
    
    void setup();
    void update();
    void draw();
    
    void storeFaces(); // push back to vector + saves face images
    void saveFacesCsv(); // save to csv

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
    
    string vidFile;
    
    ofVideoPlayer vid;
    ofxCsv shotsCsv, facesCsv;
    
    vector<ObjectFinder> finders; // ofxCv LBP cascade - 1 finder per shot
    
    vector<shot> shots;
    vector<frame> frames;
    
    int currentShot = 0;
    int currentFrame = 0;
    int numShots = 0;
    
    bool start = false;
    bool done = false;
    
    ofImage face;

    
		
};
