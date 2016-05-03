//
//  VideoDatabase.cpp
//  videoMapPlayer
//
//  Created by Tyler on 5/1/16.
//
//

#include "VideoDatabase.hpp"

VideoDatabase::VideoDatabase(string featuresFile,
                             string flannIndexFilenameBase,
                             bool makeNewIndexes,
                             int _nRois){
    
    loadFeaturesFile(featuresFile); // load flann features
    imgNames = &(flann.getAllImgs());
    
    setNumRois(_nRois); // set num rois for flann
    
    // either generate new indexes or load from files
    
    if (makeNewIndexes){
        
        // create indexes
        makeFlannIndexes(flannIndexFilenameBase);
    } else {
        
        // load indexes
        loadFlannIndexFiles(flannIndexFilenameBase);
    }
    flann.readoutFeatures();
}

//----------------------//
//---- FLANN SEARCH ----//
//----------------------//

void VideoDatabase::loadFeaturesFile(string featuresFile){
    flann.loadFeaturesFile(featuresFile); // load flann descriptors file
}

void VideoDatabase::loadFlannIndexFiles(string flannIndexFilenameBase){
    //flann.loadFlannIndexFiles(fn) doesn't exist yet
}

void VideoDatabase::setNumRois(int _nRois){
    nRois = _nRois;
    flann.splitFeaturesToRois(nRois); // split flann features into n rois
}

void VideoDatabase::makeFlannIndexes(string flannIndexFilenameBase){
    string indexFile = flannIndexFilenameBase + ".fnn";
    // make index of full images
    flann.makeIndex(indexFile);
    // make indexes of rois
    makeFlannRoiIndexes(flannIndexFilenameBase);
}

void VideoDatabase::makeFlannRoiIndexes(string flannIndexFilenameBase){
    flann.makeRoiIndexes(flannIndexFilenameBase); // make new flann roi indexes + save using roiIndexFilenameBase
}



//-----------------//
//---- VIDEODB ----//
//-----------------//

void VideoDatabase::loadVideo(string videoFile, string videoName){
    vids[videoName] = ofVideoPlayer();
    vids[videoName].load(videoFile);
}



void VideoDatabase::startVideo(){
    
    // for now just start at first shot of first video
    shot s;
    // get first start frame
    while (!getShotFromImgName(flann.getImgAtIndex(currentShotStartIdx),s)){
        currentShotStartIdx++;
        currentShotEndIdx++;
    }
    currentVidName = s.vidName;
    
    //set currentShotEndFrame
    shot e;
    bool isStart = getShotFromImgName(flann.getImgAtIndex(currentShotEndIdx), e);
    if (isStart){
        ofLogError("VideoDatabase") << "startVideo(): currentShotEndIdx not end frame!! vidName: " << e.vidName << ", shot: " << e.shot << ", start frame: " << e.start << ", end frame: " << e.end;
    }
    currentShotEndFrame = e.end;
    
    vids[s.vidName].play();
    vids[s.vidName].setFrame(s.start);
    
    ofLogNotice("VideoDatabase") << "video playing started at " << s.vidName << ", shot: " << s.shot << ", frame: " << s.start;
    
}

void VideoDatabase::update(ofImage& img){
    
    int currentFrame = vids[currentVidName].getCurrentFrame();
    
    // close to end, get next shot
    if (!hasNextShot && currentFrame > currentShotEndFrame - 10){
        getNextShot(roi);
        hasNextShot = true;
    }
    
    // if at end, switch shot
    if (hasNextShot && currentFrame >= currentShotEndFrame){
        
        // switch shot
        shot s;
        bool isStart = getShotFromImgName(flann.getImgAtIndex(nextShotStartIdx), s);
        
        if (!isStart){
            ofLogError("VideoDatabase") << "update(): switching shot, but nextShotStartIdx not start frame!! vidName: " << s.vidName << ", shot: " << s.shot << ", start frame: " << s.start << ", end frame: " << s.end;
        }
        vids[currentVidName].stop();
        
        currentVidName = s.vidName;
        currentShotStartIdx = nextShotStartIdx;
        currentShotEndIdx = currentShotStartIdx + 1;
        
        // set currentShotEndFrame
        shot e;
        isStart = getShotFromImgName(flann.getImgAtIndex(currentShotEndIdx), e);
        if (isStart){
            ofLogError("VideoDatabase") << "update(): switching shot, but currentShotEndIdx not end frame!! vidName: " << e.vidName << ", shot: " << e.shot << ", start frame: " << e.start << ", end frame: " << e.end;
        }
        currentShotEndFrame = e.end;
        
        hasNextShot = false; // reset
        
        vids[currentVidName].play();
        vids[currentVidName].setFrame(s.start);
        
        ofLogNotice("VideoDatabase") << "shot switched to " << s.vidName << ", shot: " << s.shot << ", frame: " << s.start;
        
    }
    
    vids[currentVidName].update();
    
    //ofLogNotice("VideoDatabase") << "video reports current frame as: " << vids[currentVidName].getCurrentFrame();
    
}

void VideoDatabase::update(int roi){
    
    int currentFrame = vids[currentVidName].getCurrentFrame();
    
    // close to end, get next shot
    if (!hasNextShot && currentFrame > currentShotEndFrame - 10){
        getNextShot(roi);
        hasNextShot = true;
    }
    
    // if at end, switch shot
    if (hasNextShot && currentFrame >= currentShotEndFrame){
        
        // switch shot
        shot s;
        bool isStart = getShotFromImgName(flann.getImgAtIndex(nextShotStartIdx), s);
        
        if (!isStart){
            ofLogError("VideoDatabase") << "update(): switching shot, but nextShotStartIdx not start frame!! vidName: " << s.vidName << ", shot: " << s.shot << ", start frame: " << s.start << ", end frame: " << s.end;
        }
        vids[currentVidName].stop();
        
        currentVidName = s.vidName;
        currentShotStartIdx = nextShotStartIdx;
        currentShotEndIdx = currentShotStartIdx + 1;
        
        // set currentShotEndFrame
        shot e;
        isStart = getShotFromImgName(flann.getImgAtIndex(currentShotEndIdx), e);
        if (isStart){
            ofLogError("VideoDatabase") << "update(): switching shot, but currentShotEndIdx not end frame!! vidName: " << e.vidName << ", shot: " << e.shot << ", start frame: " << e.start << ", end frame: " << e.end;
        }
        currentShotEndFrame = e.end;
        
        hasNextShot = false; // reset
        
        vids[currentVidName].play();
        vids[currentVidName].setFrame(s.start);
        
        ofLogNotice("VideoDatabase") << "shot switched to " << s.vidName << ", shot: " << s.shot << ", frame: " << s.start;

    }
    
    vids[currentVidName].update();
    
    //ofLogNotice("VideoDatabase") << "video reports current frame as: " << vids[currentVidName].getCurrentFrame();
    
}

void VideoDatabase::draw(){
    
    vids[currentVidName].draw(0,0,ofGetWidth(),ofGetHeight());
    
}



void VideoDatabase::getNextShot(int roi, bool bNewVideo){
    
    // in case of failure, replay current shot
    nextShotStartIdx = currentShotStartIdx;
    
    Mat query, indices, distances;
    query = flann.getRoiFeaturesAtIndex(currentShotEndIdx, roi);
    
    // flann search based on current video/shot/roi
    flann.knnSearchRoi(roi, query, 100, indices, distances); // find 100 closest shots
    
    for (int col=0; col<indices.cols; col++){
        int shotIdx = indices.at<int>(0,col);
        shot s;
        string imgName = flann.getImgAtIndex(shotIdx);
        
        bool isStart = getShotFromImgName(imgName, s); // load start, check if start frame
        
        if (isStart){
            if (shotIdx != currentShotStartIdx){
                // if found new shot, set it and break
                nextShotStartIdx = shotIdx;
                break;
            }
        }
    }
}


/*
void VideoDatabase::loadShots(){
    
    // loop through imgNames and parse into shots
    // save to shots vector
    
    for (auto it=imgNames->begin(); it<imgNames->end(); ++it){
        
        shot s;
        
    }
}
*/

/*
void VideoDatabase::getTimesFromImgNames(){
    
    shots.clear();
    
    for (auto it=imgs.begin(); it!=imgs.end(); ++it){
        
        // get values from filename
        
        // init values
        int startOrEnd = 0;
        shot s; s.vidName = ""; s.shot = 0; s.start = 0; s.end = 0;
        
        bool start = getFrameData(*it, s.vidName, s.shot, startOrEnd); // read filename data
        
        start ? (s.start = startOrEnd) : (s.end = startOrEnd); // is img start or end frame?
        
        // insert into shots map
        
        if (shots.find(s.vidName) != shots.end()){
            // film exists in map already
            
            if (shots[s.vidName].find(s.shot) != shots[s.vidName].end()){
                // shot exists in map already, so fill in missing shot value
                
                start ? (shots[s.vidName][s.shot].start = s.start) : (shots[s.vidName][s.shot].end = s.end);
                // fill in start or end frame value
                
            } else {
                // shot not yet in map, init both shot values
                shots[s.vidName][s.shot] = s;
            }
        }
        else {
            // film isn't yet in map
            
            map<int,shot> shotMap;
            shotMap[s.shot] = s;
            shots[s.vidName] = shotMap; // insert into film map
        }
    }
    
    // fill shotVec from shots map
    shotVec.clear();
    for (auto const& vid : shots){ // c++11 loop iterator: loop through vidNames
        for (auto const& shot : vid.second){ // loop through shots per film
            shotVec.push_back(shot.second); // add shot to shot vector
        }
    }
}
*/
















