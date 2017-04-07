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
                    points.emplace_back(InterestPoint{x,y,intensity});
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
            intensityI = points[i].intensity;
            for (int j = i+1; j < points.size(); j++) {
                distance = Dist(points[i].x,points[j].x,points[i].y,points[j].y);
                intensityJ = coef * points[j].intensity;
                if(distance <= radius && intensityI < intensityJ){
                    points.erase(points.begin()+i); break;
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
    painter.setPen(qRgb(255,0,0));
    for(auto point : points) {
        painter.drawEllipse(point.y,point.x, 2,2);
    }
    resultImage.save(filePath,"jpg");
}

vector<InterestPoint> PointSearch::Points()const{
    return points;
}
