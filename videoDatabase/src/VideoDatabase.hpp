//
//  VideoDatabase.hpp
//  videoMapPlayer
//
//  Created by Tyler on 5/1/16.
//
//

#pragma once
#include "ofMain.h"
//#include "videoMap.hpp"
#include "FlannCluster.hpp"
#include "ofxCv.h"
#include "ofxOpenCv.h"
using namespace cv;
using namespace ofxCv;

class VideoDatabase{
    
public:
    
    VideoDatabase(){}
    VideoDatabase(string featuresFile, string flannIndexFilenameBase, bool makeNewRoiIndex, int _nRois);
    
    // flann
    void loadFeaturesFile(string featuresFile);
    void loadFlannIndexFiles(string flannIndexFilenameBase);
    void setNumRois(int _nRois);
    void makeFlannIndexes(string flannIndexFilenameBase);
    void makeFlannRoiIndexes(string flannIndexFilenameBase);
    
    // videoDB
    void loadVideo(string videoFile, string videoName);
    
    void startVideo();
    void update(ofImage& img);
    void update(int roi);
    void draw();
    
    void getNextShot(ofImage& img, bool bNewVideo);
    // does BoW description on img
    // flann searches full images for best match
    
    void getNextShot(int roi, bool bNewVideo); // returns most closely related shot in DB
    void getNextShot(int roi){ return getNextShot(roi, false); } // false = include same video
    
protected:

    int currentShotStartIdx = 0;
    int currentShotEndIdx = 1; // always one higher than startIdx
    int currentShotEndFrame = 0;
    
    int nextShotStartIdx = 0;
    bool hasNextShot = false;
    
    string currentVidName = "";
    
private:
    
    struct shot{
        string vidName;
        int shot;
        // start or end frame of shot
        int start;
        int end;
        
        // used for flann index only
        bool isStart; // start (true) or end (false) frame of shot?
    };
    
    map<string, ofVideoPlayer> vids;
    const vector<string>* imgNames; // pointer to Flann imgNames vector
    vector<shot> flannShots; // indexed by flann imgNames index (only contains start OR end frame)
    map<int, map<string, int>> startFrames; // flann index < vidName < startFrame
    
    map<string, map<int,shot>> shots; // vidName < shot# < shot
    
    FlannCluster flann;
    int nRois;
    
    void loadShots(); // load shots by parsing imgNames
    
    bool getShotFromImgName(string imgName, shot& _shot){
        vector<string> split = ofSplitString(imgName, "-", true);
        _shot.vidName = split[0];
        vector<string> split2 = ofSplitString(split[1], "_", true);
        _shot.shot = ofFromString<int>(split2[0]);
        int startOrEnd = ofFromString<int>(split2[2]);
        if (split2[1] == "0start"){
            _shot.start = startOrEnd;
            return true; // start
        }
        _shot.end = startOrEnd;
        return false; // end
    }
    
    bool getShotFromImgName(string imgName, string& vidName, int& shot, int& startOrEnd){
        vector<string> split = ofSplitString(imgName, "-", true);
        vidName = split[0];
        vector<string> split2 = ofSplitString(split[1], "_", true);
        shot = ofFromString<int>(split2[0]);
        startOrEnd = ofFromString<int>(split2[2]);
        if (split2[1] == "0start"){
            return true; // start
        }
        return false; // end
    }
};