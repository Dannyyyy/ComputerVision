#include <pointsearch.h>

void PointSearch::harris() const{
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    auto resultPicture = Picture(heightPicture, widthPicture);

    const double sigma = 1;
    auto gauss = PictureFilter::getGaussXY(sigma);
    const int centerGauss = gauss.getHeight()/2;

    BorderMode border = BorderMode::ReflectBorderValue;
    auto filterSobelX = PictureFilter::getSobelGX();
    auto sobelX = picture.useFilter(filterSobelX,border);
    auto filterSobelY = PictureFilter::getSobelGY();
    auto sobelY = picture.useFilter(filterSobelY,border);

    double A,B,C;
    const double k = 0.06;
    for (int x = 0; x < heightPicture; x++) {
        for (int y = 0; y < widthPicture; y++) {
             A=B=C=0;
             for(int u=-centerGauss;u<centerGauss;u++)
             {
                 for(int v=-centerGauss;v<centerGauss;v++)
                 {
                     auto gaussW = gauss.getContentCell(u,v);
                     auto Ix = sobelX.getIntensity(x,y,border);
                     auto Iy = sobelY.getIntensity(x,y,border);
                     A += gaussW * pow(Ix,2);
                     B += gaussW * Ix * Iy;
                     C += gaussW * pow(Iy,2);
                 }
             }
             double f = A*C - pow(B,2) - k*pow((A+C),2);
             resultPicture.setIntensity(x,y,f);
        }
     }
}

void PointSearch::moravek() const{
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    auto resultPicture = Picture(heightPicture, widthPicture);
    vector<double> errors;
    double window = 2;
    BorderMode border = BorderMode::ReflectBorderValue;
    for (int x = 0; x < heightPicture; x++) {
        for (int y = 0; y < widthPicture; y++) {
            errors.clear();
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx != 0 && dy != 0) {
                        double currentError = 0;
                        for (int u = -window; u <= window; u++) {
                            for (int v = -window; v <= window; v++) {
                                auto intensity = picture.getIntensity(x+u, y+v, border);
                                auto shiftIntensity = picture.getIntensity(x+u+dx, y+v+dy, border);
                                auto error = intensity - shiftIntensity;
                                currentError += pow(error,2);
                            }
                        }
                        errors.emplace_back(currentError);
                    }
                }
            }
            double minError = *min_element(errors.begin(),errors.end());
            resultPicture.setIntensity(x,y,minError);
        }
    }
}

void PointSearch::searchInterestPoints(const Picture &resultPicture){
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    const double window = 2;
    const double treshold = 4;
    points.clear();
    BorderMode border = BorderMode::ReflectBorderValue;
    for (int x = 0; x < heightPicture; x++) {
        for (int y = 0; y < widthPicture; y++) {
            auto intensity = resultPicture.getIntensity(x,y,border);
            if (intensity > treshold) {
                bool isLocalMaximum = true;
                for (int u = -window; u <= window; u++) {
                    for (int v = -window; v <= window; v++) {
                        if (u != 0 && v != 0) {
                            auto shiftIntensity = resultPicture.getIntensity(x+u,y+v,border);
                            if (shiftIntensity >= intensity) isLocalMaximum = false;
                        }
                    }
                }
                if(isLocalMaximum) {
                    points.emplace_back(InterestPoint{x,y});
                }
            }
        }
    }
}
