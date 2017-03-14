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
    void moravek(BorderMode border, double treshold);
    void harris(BorderMode border, double treshold);
    void searchInterestPoints(Picture &resultPicture, BorderMode border, double treshold);
    void adaptiveNonMaxSuppression(const int needfulCountPoints);
    void drawAndSaveInterestPoints(const QString filePath) const;
    vector<InterestPoint> Points() const;
};

#endif // POINTSEARCH_H
