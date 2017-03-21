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
    Picture picture;
    vector<Descriptor> descriptors;
public:
    DescriptorSearch(const Picture &picture, BorderMode border);
    void descriptorNormalize(Descriptor &descriptor);
    void tresholdTrim(Descriptor &descriptor);
    unique_ptr<double[]> computeContent(InterestPoint &point, BorderMode border);
    static vector<NearestDescriptors> searchOverlap(const DescriptorSearch &first,const DescriptorSearch &second);
    static void saveOverlaps(QImage &image, QString filePath, vector<NearestDescriptors> overlaps, int width);
};

#endif // DESCRIPTORSEARCH_H
