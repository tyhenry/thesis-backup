//
//  videoMap.hpp
//  flannClusterer
//
//  Created by Tyler on 5/1/16.
//
//

#pragma once
#include "ofMain.h"

class VideoMap{
    
public:
    
    VideoMap(){}
    VideoMap(string _imgFolder);
    
    void loadImgFolder(string _imgFolder);
    string getImgAtIndex(int i);
    
    void getTimesFromImgNames();
    void readoutShots();
    
    ofDirectory imgDir;
    
private:
    
    struct shotFrames{
        // start + end frames of shot
        int start;
        int end;
    };
    
    string imgFolder;
    vector<string> imgs;
    map <string, map<int,shotFrames>> shots; // film, shots by shot # : {start,end} frames
    
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
        string file;
        for (int i=0; i<split.size()-1; i++){ file += split[i]; }
        return file;
    }
    bool getFrameData(string imgName, string& film, int& shot, int& startOrEnd){
        vector<string> split = ofSplitString(imgName, "-", true);
        film = split[0];
        vector<string> split2 = ofSplitString(split[1], "_", true);
        shot = ofFromString<int>(split2[0]);
        startOrEnd = ofFromString<int>(split2[2]);
        if (split2[1] == "0start"){
            return true; // start
        }
        return false; // end
    }
    

};
