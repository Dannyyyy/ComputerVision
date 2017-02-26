#include <gaussianpyramid.h>

GaussianPyramid::GaussianPyramid(Picture &picture, int numberLevelsInOctave){
    this->initialSigma = 0.5;
    this->zeroSigma = 5;
    this->countLevelsInOctave = numberLevelsInOctave;
    this->k = pow(2,(1./this->countLevelsInOctave));

    auto initialPicture = picture.useFilter(*PictureFilter::getGaussXY(zeroSigma),BorderMode::ReflectBorderValue);
    auto firstLevel = new Level(*initialPicture);
    firstLevel->outputPicture();
}
