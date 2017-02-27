#include <gaussianpyramid.h>

GaussianPyramid::GaussianPyramid(Picture &picture,int numberOctaves, int numberLevelsInOctave){
    this->initialSigma = 0.5;
    this->zeroSigma = 1.6;
    this->countOctaves = numberOctaves;
    this->countLevelsInOctave = numberLevelsInOctave;
    this->k = pow(2,(1./this->countLevelsInOctave));

    for(int i=0;i<this->countOctaves;i++)
    {
        this->octaves.emplace_back(Octave());
    }

    double deltaSigma = this->calculationDeltaSigma(this->zeroSigma, this->initialSigma);
    auto initialPicture = picture.useFilter(*PictureFilter::getGaussXY(deltaSigma),BorderMode::ReflectBorderValue);
    auto firstLevel = Level(*initialPicture);
    firstLevel.setCurrentSigma(this->zeroSigma);
    this->octaves[0].levels.emplace_back(firstLevel);
    for(int i = 0; i < this->countOctaves; i++) {
        for (int j = 0; j <= this->countLevelsInOctave; j++) {
            if (j == 0) {
                if (i != 0) {
                    auto level = Level(*octaves[i-1].levels[this->countLevelsInOctave].picture.scalePicture());
                    level.setCurrentSigma(octaves[i-1].levels[this->countLevelsInOctave].getCurrentSigma()/2);
                    this->octaves[i].levels.emplace_back(level);
                }
            }
            else {
                auto level = Level();
                level.setCurrentSigma(this->multiplySigmaToK(octaves[i].levels[j-1].getCurrentSigma()));
                deltaSigma = this->calculationDeltaSigma(level.getCurrentSigma(),octaves[i].levels[j-1].getCurrentSigma());
                auto levelPicture = octaves[i].levels[j-1].picture.useFilter(*PictureFilter::getGaussXY(deltaSigma),BorderMode::ReflectBorderValue);
                level.setPicture(*levelPicture);
                this->octaves[i].levels.emplace_back(level);
            }
        }
    }
    for(int i = 0; i < this->countOctaves; i++) {
        for (int j = 0; j <= this->countLevelsInOctave; j++) {
            this->octaves[i].levels[j].outputPicture(i,j);
        }
    }
}

double GaussianPyramid::calculationDeltaSigma(double firstSigma, double secondSigma){
    double deltaSigma = sqrt(pow(firstSigma,2) - pow(secondSigma,2));
    return deltaSigma;
}

double GaussianPyramid::multiplySigmaToK(double sigma){
    double resultSigma = sigma * this->k;
    return resultSigma;
}
