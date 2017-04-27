#include <cvhelper.h>

QString CVHelper::getFilePath(const int index){
    switch(index){
        case 0: return  fPictureFilePath;
        case 1: return sPictureFilePath;
        case 2: return tPictureFilePath;
    }
}

void CVHelper::preparePointSearch(PointSearch &pointSearch, BorderMode border, double treshold, int pointsCount){
    pointSearch.harris(border, treshold);
    pointSearch.adaptiveNonMaxSuppression(pointsCount);
}
