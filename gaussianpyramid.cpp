#include <gaussianpyramid.h>
#include <iostream>

static double DeltaSigma(double firstSigma, double secondSigma){
    double deltaSigma = sqrt(pow(firstSigma,2) - pow(secondSigma,2));
    return deltaSigma;
}

static int CountOctaves(const int height,const int width){
    const int minPictureSizeOnOctave = 32;
    int minPictureSize = width;
    if(height < width){
        minPictureSize = height;
    }
    int count = (int)ceil(log2(minPictureSize/minPictureSizeOnOctave)+1);
    return count;

}

GaussianPyramid::GaussianPyramid(const Picture &picture, int numberLevelsInOctave){
    countOctaves = CountOctaves(picture.getHeight(), picture.getWidth());
    countLevelsInOctave = numberLevelsInOctave;
    k = pow(2,(1./this->countLevelsInOctave));

    octaves.resize(countOctaves);

    double deltaSigma = DeltaSigma(zeroSigma, initialSigma);
    auto filterX = PictureFilter::getGaussX(deltaSigma);
    auto filterY = PictureFilter::getGaussY(deltaSigma);
    auto initialPicture = picture.useFilter(filterX,filterY,BorderMode::ReflectBorderValue);
    octaves[0].emplace_back(Level{move(initialPicture), zeroSigma});
    for(int i = 0; i < countOctaves; i++) {
        auto &octave = octaves[i];
        for (int j = 0; j <= countLevelsInOctave; j++) {
            if (j == 0) {
                if(i !=0 ) {
                    auto &baseLevel = octaves[i-1][countLevelsInOctave];
                    auto picture = baseLevel.picture.scalePicture();
                    octave.emplace_back(Level{move(picture),baseLevel.sigma/2});
                }
            }
            else {
                auto &prevLevel = octaves[i][j-1];
                auto sigma = prevLevel.sigma * k;
                deltaSigma = DeltaSigma(sigma,prevLevel.sigma);
                filterX = PictureFilter::getGaussX(deltaSigma);
                filterY = PictureFilter::getGaussY(deltaSigma);
                auto picture = prevLevel.picture.useFilter(filterX,filterY, BorderMode::ReflectBorderValue);
                octave.emplace_back(Level{move(picture),sigma});
            }
        }
    }
}

int GaussianPyramid::getCountOctaves() const{
    return countOctaves;
}

int GaussianPyramid::getCountLevelsInOctave() const{
    return countLevelsInOctave;
}

void GaussianPyramid::savePicture(int octave, int level, QString filePath) const{
    octaves[octave][level].picture.saveImage(filePath);
}
