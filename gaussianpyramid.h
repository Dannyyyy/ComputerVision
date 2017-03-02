#ifndef GAUSSIANPYRAMID_H
#define GAUSSIANPYRAMID_H
#include <picture.h>
#include <vector>

using namespace std;

struct Level{
    Picture picture;
    double sigma;
};

class GaussianPyramid{
    int countOctaves;
    int countLevelsInOctave;
    double initialSigma;
    double zeroSigma;
    double k;
    vector<vector<Level>> octaves;
public:
    GaussianPyramid(Picture &picture, int numberLevelsInOctave);
    void outputPyramid();
};

#endif // GAUSSIANPYRAMID_H
