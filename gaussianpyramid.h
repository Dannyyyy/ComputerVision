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
    vector<vector<Level>> differences;
public:
    int getCountOctaves() const;
    int getCountLevelsInOctave() const;
    void savePicture(int octave, int level, QString filePath) const;
    GaussianPyramid(const Picture &picture, int numberLevelsInOctave);
    void calculateDifferences();
    vector<Level> getDiffOctave(const int octave) const;
    Level getDiffLevel(const int octave, const int level) const;
    vector<Level> getOctave(const int octave) const;
    Level getLevel(const int octave, const int level) const;
};

#endif // GAUSSIANPYRAMID_H
