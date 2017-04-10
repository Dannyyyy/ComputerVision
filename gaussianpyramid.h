#ifndef GAUSSIANPYRAMID_H
#define GAUSSIANPYRAMID_H
#include <picture.h>
#include <vector>

using namespace std;

struct Level{
    Picture picture;
    double localSigma;
    double globalSigma;
};

class GaussianPyramid{
    const double initialSigma = 0.5;
    const double zeroSigma = 1.6;
    int countOctaves;
    int countLevelsInOctave;
    double k;
    vector<vector<Level>> octaves;
public:
    int getCountOctaves() const;
    int getCountLevelsInOctave() const;
    void savePicture(int octave, int level, QString filePath) const;
    GaussianPyramid(const Picture &picture, int numberLevelsInOctave);
};

#endif // GAUSSIANPYRAMID_H
