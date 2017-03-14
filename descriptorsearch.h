#ifndef DESCRIPTORSEARCH_H
#define DESCRIPTORSEARCH_H
#include <picture.h>
#include <picturefilter.h>
#include <border.h>
#include <pointsearch.h>

struct Descriptor{
    int x;
    int y;
    int size;
    unique_ptr<double []> content;
};

class DescriptorSearch{
private:
    Picture picture;
    vector<Descriptor> descriptors;
public:
    DescriptorSearch(const Picture &picture, BorderMode border);
    void descriptorNormalize(Descriptor &descriptor);
};

#endif // DESCRIPTORSEARCH_H
