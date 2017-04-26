#include <cvhelper.h>

void CVHelper::preparePointSearch(PointSearch &pointSearch, BorderMode border, double treshold, int pointsCount){
    pointSearch.harris(border, treshold);
    pointSearch.adaptiveNonMaxSuppression(pointsCount);
}
