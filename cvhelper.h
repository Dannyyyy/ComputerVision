#ifndef CVHELPER_H
#define CVHELPER_H

#include <picture.h>
#include <pointsearch.h>

const QString mainFilePath = "C:\\AGTU\\pictures\\";
const QString fPictureFilePath = mainFilePath + "first.jpg";
const QString sPictureFilePath = mainFilePath + "second.jpg";
const QString tPictureFilePath = mainFilePath + "third.jpg";
const QString neutralFilePath = mainFilePath + "neutral.jpg";
const QString resultFilePath = mainFilePath + "result.jpg";

class CVHelper{
  public:

    static void preparePointSearch(PointSearch &pointSearch, BorderMode border, double treshold, int pointsCount);
    static QString getFilePath(const int index);
};


#endif // CVHELPER_H
