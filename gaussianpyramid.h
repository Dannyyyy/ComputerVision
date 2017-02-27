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
    void outputPicture(){
        this->picture.saveImage("Level");
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
};

#endif // GAUSSIANPYRAMID_H
