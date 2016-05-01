//
//  videoMap.cpp
//  flannClusterer
//
//  Created by Tyler on 5/1/16.
//
//

#include "videoMap.hpp"

VideoMap::VideoMap(string _imgFolder){
    
    loadImgFolder(_imgFolder);
}

void VideoMap::loadImgFolder(string _imgFolder){
    
    imgFolder = _imgFolder;
    
    ofDirectory imgDir(imgFolder);
    imgDir.allowExt("jpg");
    imgDir.listDir();
    
    for (int i=0; i<imgDir.size(); i++){ imgs.push_back(removeExt(imgDir.getName(i))); }
}

string VideoMap::getImgAtIndex(int i){
    
    if (i < imgs.size()){
        return imgs[i];
    }
    ofLogError("VideoMap") << "tried to get image at out-of-bounds index " << i;
    return "";
}

void VideoMap::getTimesFromImgNames(){
    
    shots.clear();
    
    for (auto it=imgs.begin(); it!=imgs.end(); ++it){
        
        // get values from filename
        
        // init values
        string film; int shot; int startOrEnd = 0;
        shotFrames sFrames; sFrames.start = 0; sFrames.end = 0;
        
        bool start = getFrameData(*it, film, shot, startOrEnd); // read filename data
        
        start ? (sFrames.start = startOrEnd) : (sFrames.end = startOrEnd); // is img start or end frame?
        
        // insert into shots map
        
        if (shots.find(film) != shots.end()){
            // film exists in map already
            
            if (shots[film].find(shot) != shots[film].end()){
                // shot exists in map already, so fill in missing shotFrames value
                
                start ? (shots[film][shot].start = sFrames.start) : (shots[film][shot].end = sFrames.end);
                // fill in start or end frame value
                
            } else {
                // shot not yet in map, init both shotFrames values
                shots[film][shot] = sFrames;
            }
        }
        else {
            // film isn't yet in map
            
            map<int,shotFrames> shotMap;
            shotMap[shot] = sFrames;
            shots[film] = shotMap; // insert into film map
        }
    }
}

void VideoMap::readoutShots(){
    ofLogNotice("FlannCluster") << "reading out shots: ";
    
    for (auto const& film : shots){ // c++11 loop iterator
        for (auto const& shot : film.second){
            cout << film.first << " shot " << shot.first << " frame start: " << shot.second.start << ", end: " << shot.second.end << endl;
        }
    }
}