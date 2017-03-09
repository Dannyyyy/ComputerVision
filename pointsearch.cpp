#include <pointsearch.h>
#include <iostream>


void PointSearch::harris(){
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
    searchInterestPoints(resultPicture);
}

void PointSearch::moravek(){
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
    searchInterestPoints(resultPicture);
}

void PointSearch::searchInterestPoints(Picture &resultPicture){
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    const double window = 2;
    const double treshold = 0.01;
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
                    points.emplace_back(InterestPoint{x,y,intensity});
                }
            }
        }
    }
    cout<<points.size()<<endl;
}

static double Dist(const int x1, const int x2,const int y1, const int y2){
    double dist = sqrt(pow((x2-x1),2) + pow((y2-y1),2));
    return dist;
}

void PointSearch::adaptiveNonMaxSuppression(const int needfulCountPoints){
    const int height = picture.getHeight();
    const int width = picture.getWidth();
    int radius = 0;
    const int maxRadius = Dist(0,height, 0,width);
    const double coef = 0.9;
    double distance, intensityI, intensityJ;
    while(points.size()>needfulCountPoints && radius<=maxRadius){
        for (int i = 0; i < points.size(); i++) {
            for (int j = i+1; j < points.size(); j++) {
                distance = Dist(points[i].x,points[j].x,points[i].y,points[j].y);
                intensityI = coef * points[i].intensity;
                intensityJ = points[j].intensity;
                if(distance <= radius && intensityI < intensityJ){
                    points.erase(points.begin()+i);
                }
            }
         }
    }
}

void PointSearch::drawAndSaveInterestPoints(const QString filePath) const{
    auto resultImage = picture.getImage();
    for(auto &point : points) {
        resultImage.setPixel(point.y, point.x ,qRgb(0,255,0));
    }
    resultImage.save(filePath,"jpg");
}
