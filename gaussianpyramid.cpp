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

    auto initialPicture = picture.useFilter(*PictureFilter::getGaussXY(zeroSigma),BorderMode::ReflectBorderValue);
    auto firstLevel = Level(*initialPicture);
    this->octaves[0].levels.emplace_back(firstLevel);
    this->octaves[0].levels[0].outputPicture();
}
