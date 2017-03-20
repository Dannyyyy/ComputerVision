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

class DescriptorSearch{
private:
    const int regionSizeX = 4;
    const int regionSizeY = 4;
    const int partsCount = 8;
    const int histogramSize = 4;
    Picture picture;
    vector<Descriptor> descriptors;
public:
    DescriptorSearch(const Picture &picture, BorderMode border);
    void descriptorNormalize(Descriptor &descriptor);
    void tresholdTrim(Descriptor &descriptor);
    unique_ptr<double[]> computeContent(InterestPoint &point, BorderMode border);
    void searchOverlap(DescriptorSearch &first, DescriptorSearch &second);
};

#endif // DESCRIPTORSEARCH_H
