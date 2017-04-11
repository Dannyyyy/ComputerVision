#include <pointsearch.h>
#include <iostream>
#include <QPainter>

void PointSearch::harris(BorderMode border, double treshold){
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    auto resultPicture = Picture(heightPicture, widthPicture);

    const double sigma = windowHalfSize/3.;
    auto gauss = PictureFilter::getGaussXY(sigma);

    auto filterSobelX = PictureFilter::getSobelGX();
    auto sobelX = picture.useFilter(filterSobelX,border);
    auto filterSobelY = PictureFilter::getSobelGY();
    auto sobelY = picture.useFilter(filterSobelY,border);

    double A,B,C;
    for (int x = 0; x < heightPicture; x++) {
        for (int y = 0; y < widthPicture; y++) {
             A=B=C=0;
             for(int u=-windowHalfSize;u<=windowHalfSize;u++)
             {
                 for(int v=-windowHalfSize;v<=windowHalfSize;v++)
                 {
                     auto gaussW = gauss.getContentCell(u+windowHalfSize, v+windowHalfSize);
                     auto Ix = sobelX.getIntensity(x+u,y+v,border);
                     auto Iy = sobelY.getIntensity(x+u,y+v,border);
                     A += gaussW * pow(Ix,2);
                     B += gaussW * Ix * Iy;
                     C += gaussW * pow(Iy,2);
                 }
             }
             const double D = sqrt((A-C)*(A-C) + 4*B*B);
             const double L1 = abs((A+C+D)/2);
             const double L2 = abs((A+C-D)/2);
             resultPicture.setIntensity(x,y,min(L1,L2));
        }
     }
    searchInterestPoints(resultPicture, border, treshold);
}

void PointSearch::moravek(BorderMode border, double treshold){
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    auto resultPicture = Picture(heightPicture, widthPicture);
    vector<double> errors;
    double window = 2;
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
    searchInterestPoints(resultPicture, border, treshold);
}

Picture PointSearch::harrisValues(const Picture &picture, BorderMode border, int windowHalfSize) {
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    auto resultPicture = Picture(heightPicture, widthPicture);

    const double sigma = windowHalfSize/3.;
    auto gauss = PictureFilter::getGaussXY(sigma);

    auto filterSobelX = PictureFilter::getSobelGX();
    auto sobelX = picture.useFilter(filterSobelX,border);
    auto filterSobelY = PictureFilter::getSobelGY();
    auto sobelY = picture.useFilter(filterSobelY,border);

    double A,B,C;
    for (int x = 0; x < heightPicture; x++) {
        for (int y = 0; y < widthPicture; y++) {
             A=B=C=0;
             for(int u=-windowHalfSize;u<=windowHalfSize;u++)
             {
                 for(int v=-windowHalfSize;v<=windowHalfSize;v++)
                 {
                     auto gaussW = gauss.getContentCell(u+windowHalfSize, v+windowHalfSize);
                     auto Ix = sobelX.getIntensity(x+u,y+v,border);
                     auto Iy = sobelY.getIntensity(x+u,y+v,border);
                     A += gaussW * pow(Ix,2);
                     B += gaussW * Ix * Iy;
                     C += gaussW * pow(Iy,2);
                 }
             }
             const double D = sqrt((A-C)*(A-C) + 4*B*B);
             const double L1 = abs((A+C+D)/2);
             const double L2 = abs((A+C-D)/2);
             resultPicture.setIntensity(x,y,min(L1,L2));
        }
     }
    return resultPicture;
}

static bool checkPoint(const double initial, const GaussianPyramid & pyramid, const int x, const int y, const int octave, const int level, BorderMode border){
    bool min = true;
    bool max = true;
    for(int dz = -1; dz<2; dz++) {
        for(int dx = -1; dx < 2; dx++) {
            for(int dy = -1; dy<2; dy++) {
                if (dx != 0 || dy != 0 || dz != 0) {
                    cout<<"dz: "<<dz<<" dx: "<<dx<<" dy: "<<dy<<endl;
                    const int levelI = level + dz;
                    const int xI = x + dx;
                    const int yI = y + dy;
                    double selected = pyramid.getDiffLevel(octave, levelI).picture.getIntensity(xI,yI,border);
                    if(selected <= initial){
                        min = false;
                    }
                    if(selected >= initial){
                        max = false;
                    }
                }
            }
        }
    }
    if(max  || min){
        return true;
    }
    return false;
}

void PointSearch::blob(GaussianPyramid &pyramid, BorderMode border, double treshold){
    points.clear();
    const int countOctaves = pyramid.getCountOctaves();

    for(int octaveI = 0; octaveI < countOctaves; octaveI++){
        cout<<"Octave: "<<octaveI<<endl;
        int countLevels = pyramid.getDiffOctave(octaveI).size();
        const int height = pyramid.getDiffLevel(octaveI,0).picture.getHeight();
        const int width = pyramid.getDiffLevel(octaveI,0).picture.getWidth();
        for(int levelI = 1; levelI < countLevels - 1; levelI++){
            cout<<"Level: "<<levelI<<endl;
            auto diffLevel = pyramid.getDiffLevel(octaveI,levelI);
            auto harris = PointSearch::harrisValues(diffLevel.picture, border, windowHalfSize);
            for(int x = 0;x<height;x++){
                for(int y = 0; y< width; y++){
                    double initial = diffLevel.picture.getIntensity(x,y,border);
                    const double intensity = harris.getIntensity(x,y,border);
                    if(intensity > 0.001)
                    {
                        if(checkPoint(initial,pyramid,x,y,octaveI,levelI, border)){
                            cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
                            InterestPoint point;
                            point.x = x*pow(2,octaveI);
                            point.y = y*pow(2,octaveI);
                            point.localX = x;
                            point.localY = y;
                            point.intensity = intensity;
                            point.octave = octaveI;
                            point.level = levelI;
                            point.localSigma = diffLevel.localSigma;
                            point.globalSigma = diffLevel.globalSigma;
                            points.emplace_back(point);
                        }
                    }
                }
            }
        }
    }

}

void PointSearch::searchInterestPoints(Picture &resultPicture, BorderMode border, double treshold){
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    const double window = 2;
    points.clear();
    for (int x = 0; x < heightPicture; x++) {
        for (int y = 0; y < widthPicture; y++) {
            auto intensity = resultPicture.getIntensity(x,y,border);
            if (intensity > treshold) {
                bool isLocalMaximum = true;
                for (int u = -window; u <= window && isLocalMaximum == true; u++) {
                    for (int v = -window; v <= window; v++) {
                        if (u != 0 && v != 0) {
                            auto shiftIntensity = resultPicture.getIntensity(x+u,y+v,border);
                            if (shiftIntensity >= intensity){ isLocalMaximum = false; break;}
                        }
                    }
                }
                if(isLocalMaximum) {
                    InterestPoint point;
                    point.x = x;
                    point.y = y;
                    point.intensity = intensity;
                    points.emplace_back(point);
                }
            }
        }
    }
    cout<<"Initial: "<<points.size()<<endl;
}

static double Dist(const int x1, const int x2,const int y1, const int y2){
    return hypot(x2-x1,y2-y1);
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
            intensityI = coef * points[i].intensity;
            for (int j = i+1; j < points.size(); j++) {
                distance = Dist(points[i].x,points[j].x,points[i].y,points[j].y);
                intensityJ = points[j].intensity;
                if(distance <= radius && intensityI > intensityJ){
                    points.erase(points.begin()+j); j--;
                }
            }
         }
        radius++;
    }
    cout<<"Addaptive: "<<points.size()<<endl;
}

void PointSearch::drawAndSaveInterestPoints(const QString filePath) const{
    auto resultImage = picture.getImage();
    QPainter painter(&resultImage);
    painter.setPen(qRgb(255,255,0));
    for(auto point : points) {
        painter.drawEllipse(point.y,point.x, 2,2);
    }
    resultImage.save(filePath,"jpg");
}

void PointSearch::drawAndSaveInterestPointsBlob(const QString filePath) const{
    auto resultImage = picture.getImage();
    QPainter painter(&resultImage);
    painter.setPen(qRgb(255,255,0));
    for(auto point : points) {
        painter.drawEllipse(point.y,point.x, 2,2);
    }
    resultImage.save(filePath,"jpg");
}

vector<InterestPoint> PointSearch::Points()const{
    return points;
}
