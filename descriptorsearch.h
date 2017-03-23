#ifndef DESCRIPTORSEARCH_H
#define DESCRIPTORSEARCH_H
#include <picture.h>
#include <picturefilter.h>
#include <border.h>
#include <pointsearch.h>

struct Descriptor{
    int x;
    int y;
    unique_ptr<double []> content;
};

struct NearestDescriptors{
    int fX;
    int fY;
    int sX;
    int sY;
};

class DescriptorSearch{
private:
    static const int regionSizeX = 4;
    static const int regionSizeY = 4;
    static const int partsCount = 8;
    static const int histogramSize = 4;
    vector<Descriptor> descriptors;
    static void descriptorNormalize(Descriptor &descriptor);
    static void tresholdTrim(Descriptor &descriptor);
    static unique_ptr<double[]> computeContent(const Picture &sobelX, const Picture &sobelY, const InterestPoint &point, BorderMode border);
    static vector<double> calculateDistance(const DescriptorSearch &f, const DescriptorSearch &s);
public:
    DescriptorSearch(const Picture &sobelX, const Picture &sobelY, BorderMode border, const vector<InterestPoint> &points);
    static vector<NearestDescriptors> searchOverlap(const DescriptorSearch &first,const DescriptorSearch &second);
    static void saveOverlaps(QImage &image, QString filePath, const vector<NearestDescriptors> &overlaps, const int width);    
};

#endif // DESCRIPTORSEARCH_H
