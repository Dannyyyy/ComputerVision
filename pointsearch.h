#ifndef POINTSEARCH_H
#define POINTSEARCH_H
#include <picture.h>
#include <picturefilter.h>
#include <border.h>
#include <gaussianpyramid.h>

using namespace std;

struct InterestPoint{
    int x,y;
    double intensity;
    int octave, level;
    double localSigma, globalSigma;
    int localX, localY;
};

class PointSearch{
private:
    int windowSize = 7;
    int windowHalfSize = 3;
    Picture picture;
    vector<InterestPoint> points;
public:
    PointSearch(const Picture &picture): picture(picture){}
    PointSearch(){}
    void moravek(BorderMode border, double treshold);
    void harris(BorderMode border, double treshold);
    void searchInterestPoints(Picture &resultPicture, BorderMode border, double treshold);
    void adaptiveNonMaxSuppression(const int needfulCountPoints);
    void drawAndSaveInterestPoints(const QString filePath) const;
    void drawAndSaveInterestPointsBlob(const QString filePath) const;
    vector<InterestPoint> Points() const;
    void blob(GaussianPyramid &pyramid, BorderMode border);
    static Picture harrisValues(const Picture &picture, BorderMode border, const int windowHalfSize);
};

#endif // POINTSEARCH_H
