#ifndef GAUSSIANPYRAMID_H
#define GAUSSIANPYRAMID_H
#include <picture.h>
#include <vector>

using namespace std;

class Level{
    Picture picture;
public:
    Level(Picture &picture){
        this->picture = move(picture);
    }
    void setPicture(Picture &picture){
        this->picture = move(picture);
    }
    void outputPicture(){
        this->picture.saveImage("Level");
    }
};

class GaussianPyramid{
    int countLevelsInOctave;
    double initialSigma;
    double zeroSigma;
    double k;
public:
    GaussianPyramid(Picture &picture, int numberLevelsInOctave);
};

#endif // GAUSSIANPYRAMID_H
