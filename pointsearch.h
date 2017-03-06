#ifndef POINTSEARCH_H
#define POINTSEARCH_H
#include <picture.h>

struct InterestPoint{
    int x,y;
};

class PointSearch{
private:
    Picture picture;
    vector<InterestPoint> points;
    PointSearch(const Picture &picture);
public:
    void moravek();
    void harris();
};

#endif // POINTSEARCH_H
