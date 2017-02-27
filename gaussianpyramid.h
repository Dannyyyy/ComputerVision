#ifndef GAUSSIANPYRAMID_H
#define GAUSSIANPYRAMID_H
#include <picture.h>
#include <vector>

using namespace std;

class Level{
public:
    Picture picture;
    double currentSigma;
    Level(){}
    Level(Picture &picture){
        this->picture = move(picture);
    }
    void setPicture(Picture &picture){
        this->picture = move(picture);
    }
    void outputPicture(int octave, int level){
        this->picture.saveImage("Octave_" + QString::number(octave) + "_level_" + QString::number(level));
    }
    void setCurrentSigma(double sigma){
        this->currentSigma = sigma;
    }
    double getCurrentSigma(){
        return this->currentSigma;
    }
};

class Octave{
public:
    vector<Level> levels;
};

class GaussianPyramid{
    int countOctaves;
    int countLevelsInOctave;
    double initialSigma;
    double zeroSigma;
    double k;
public:
    vector<Octave> octaves;
    GaussianPyramid(Picture &picture, int numberOctaves, int numberLevelsInOctave);
    double calculationDeltaSigma(double firstSigma, double secondSigma);
    double multiplySigmaToK(double sigma);
};

#endif // GAUSSIANPYRAMID_H
