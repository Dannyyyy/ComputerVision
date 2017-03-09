#ifndef POINTSEARCH_H
#define POINTSEARCH_H
#include <picture.h>
#include <picturefilter.h>
#include <border.h>

using namespace std;

struct InterestPoint{
    int x,y;
    double intensity;
};

class PointSearch{
private:
    Picture picture;
    vector<InterestPoint> points;
public:
    PointSearch(const Picture &picture): picture(picture){}
    void moravek();
    void harris();
    void searchInterestPoints(Picture &resultPicture);
    void adaptiveNonMaxSuppression(const int needfulCountPoints);
    void drawAndSaveInterestPoints(const QString filePath) const;
};

#endif // POINTSEARCH_H
