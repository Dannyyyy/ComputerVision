#ifndef POINTSEARCH_H
#define POINTSEARCH_H
#include <picture.h>
#include <picturefilter.h>
#include <border.h>

using namespace std;

struct InterestPoint{
    int x,y;
};

class PointSearch{
private:
    Picture picture;
    vector<InterestPoint> points;
public:
    PointSearch(const Picture &picture): picture(picture){}
    void moravek() const;
    void harris() const;
    void searchInterestPoints(const Picture &resultPicture);
};

#endif // POINTSEARCH_H
