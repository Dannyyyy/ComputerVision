#include <gaussianpyramid.h>

static double calculationDeltaSigma(double firstSigma, double secondSigma){
    double deltaSigma = sqrt(pow(firstSigma,2) - pow(secondSigma,2));
    return deltaSigma;
}

static int calculationCountOctaves(const int height,const int width){
    const int minPictureSizeOnOctave = 32;
    int minPictureSize = width;
    if(height < width){
        minPictureSize = height;
    }
    int count = (int)ceil(sqrt((floor)(minPictureSize/minPictureSizeOnOctave)));
    return count;
}

GaussianPyramid::GaussianPyramid(Picture &picture, int numberLevelsInOctave){
    initialSigma = 0.5;
    zeroSigma = 1.6;
    countOctaves = calculationCountOctaves(picture.getHeight(), picture.getWidth());
    countLevelsInOctave = numberLevelsInOctave;
    k = pow(2,(1./this->countLevelsInOctave));

    octaves.resize(countOctaves);

    double deltaSigma = calculationDeltaSigma(zeroSigma, initialSigma);
    auto initialPicture = picture.useFilter(*PictureFilter::getGaussXY(deltaSigma),BorderMode::ReflectBorderValue);
    auto firstLevel = Level();
    firstLevel.picture = move(*initialPicture);
    firstLevel.currentSigma = this->zeroSigma;
    octaves[0].emplace_back(move(firstLevel));
    for(int i = 0; i < countOctaves; i++) {
        for (int j = 0; j <= countLevelsInOctave; j++) {
            if (j == 0) {
                if (i != 0) {
                    auto level = Level();
                    level.picture = move(*octaves[i-1][this->countLevelsInOctave].picture.scalePicture());
                    level.currentSigma = octaves[i-1][this->countLevelsInOctave].currentSigma/2;
                    octaves[i].emplace_back(move(level));
                }
            }
            else {
                auto level = Level();
                auto prevCurrentSigma = octaves[i][j-1].currentSigma;
                auto currentSigma = prevCurrentSigma * this->k;
                level.currentSigma = currentSigma;
                deltaSigma = calculationDeltaSigma(currentSigma,prevCurrentSigma);
                auto levelPicture = octaves[i][j-1].picture.useFilter(*PictureFilter::getGaussXY(deltaSigma),BorderMode::ReflectBorderValue);
                level.picture = move(*levelPicture);
                octaves[i].emplace_back(move(level));
            }
        }
    }
}

void GaussianPyramid::outputPyramid(){
    for(int i = 0; i < this->countOctaves; i++) {
        for (int j = 0; j <= this->countLevelsInOctave; j++) {
            QString fileName =  "Picture_octave_" + QString::number(i) + "_level_" + QString::number(j);
            this->octaves[i][j].picture.saveImage(fileName);
        }
    }
}
