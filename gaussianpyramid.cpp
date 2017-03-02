#include <gaussianpyramid.h>

static double DeltaSigma(double firstSigma, double secondSigma){
    double deltaSigma = sqrt(pow(firstSigma,2) - pow(secondSigma,2));
    return deltaSigma;
}

static int calculationCountOctaves(const int height,const int width){
    const int minPictureSizeOnOctave = 32;
    int minPictureSize = width;
    if(height < width){
        minPictureSize = height;
    }
    int count = (int)ceil(log2(minPictureSize/minPictureSizeOnOctave)+1);
    return count;

}

GaussianPyramid::GaussianPyramid(Picture &picture, int numberLevelsInOctave){
    initialSigma = 0.5;
    zeroSigma = 1.6;
    countOctaves = calculationCountOctaves(picture.getHeight(), picture.getWidth());
    countLevelsInOctave = numberLevelsInOctave;
    k = pow(2,(1./this->countLevelsInOctave));

    octaves.resize(countOctaves);

    double deltaSigma = DeltaSigma(zeroSigma, initialSigma);
    auto filter = PictureFilter::getGaussXY(deltaSigma);
    auto initialPicture = picture.useFilter(*filter,BorderMode::ReflectBorderValue);
    octaves[0].emplace_back(Level{*initialPicture, zeroSigma});
    for(int i = 0; i < countOctaves; i++) {
        auto &octave = octaves[i];
        for (int j = 0; j <= countLevelsInOctave; j++) {
            if (j == 0) {
                if(i !=0 ) {
                    auto &baseLevel = octaves[i-1][this->countLevelsInOctave];
                    octave.emplace_back(Level{*baseLevel.picture.scalePicture(),baseLevel.sigma/2});
                }
            }
            else {
                auto &prevLevel = octaves[i][j-1];
                auto sigma = prevLevel.sigma * k;
                deltaSigma = DeltaSigma(sigma,prevLevel.sigma);
                filter = PictureFilter::getGaussXY(deltaSigma);
                auto picture = prevLevel.picture.useFilter(*filter, BorderMode::ReflectBorderValue);
                octave.emplace_back(Level{*picture,sigma});
            }
        }
    }
}

void GaussianPyramid::outputPyramid(){
    for(int i = 0; i < countOctaves; i++) {
        for (int j = 0; j <= countLevelsInOctave; j++) {
            QString fileName =  "Picture_octave_" + QString::number(i) + "_level_" + QString::number(j);
            octaves[i][j].picture.saveImage(fileName);
        }
    }
}
