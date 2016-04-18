#pragma once

#include "ofMain.h"
#include "BagOfWords.hpp"
#include "ofxTSNE.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void loadImgPaths(vector<string>& _imgPaths, string folder, int nImgs);
        void loadImgPaths(vector<string>& _imgPaths, string folder){
            loadImgPaths(_imgPaths, folder, -1); // loads all images
        }
    
        void loadImgsandFeatures(map<string,vector<float>>& featureMap); // scales down imgs to 200 width
        // loads imgs and BoW pre-described features for TSNE display

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
    
        BagOfWords BoW;
        vector<string> imgPaths; // for training only
        
        vector<ofImage> imgs; // for display
        
        string imgFolder, dictionaryFile, descriptorsFile;
        
        ofxTSNE tsne;
        
        vector<vector<float>> features;
        // img < vals
        
        vector<vector<double>> imgPts;
        // img < vals
    
    
        ofxPanel gui;
        ofParameter<float> scale;
        ofParameter<float> imageSize;
        ofParameter<int> roi;
        ofParameter<bool> bShowByRoi;
		
};
