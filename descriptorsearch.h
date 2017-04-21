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
    static const int RegionSizeX = 4;
    static const int RegionSizeY = 4;
    static const int PartsCount = 8;
    static const int HistogramSize = 4;
    static const int SiftRegionSizeX = 1;
    static const int SiftRegionSizeY = 1;
    static const int SiftPartsCount = 36;
    static const int SiftHistogramSize = 16;
    vector<Descriptor> descriptors;
    static void descriptorNormalize(Descriptor &descriptor);
    static void tresholdTrim(Descriptor &descriptor);
    static unique_ptr<double[]> computeContent(const GaussianPyramid &pyramid, const Picture &sobelX, const Picture &sobelY, const InterestPoint &point, BorderMode border, double aroundAngle, const int regionSizeX, const int regionSizeY, const int partsCount, const int histogramSize, double sigma, bool distribution);
    static vector<double> calculateDistance(const DescriptorSearch &f, const DescriptorSearch &s);
    static void findPeaks(int &firstIndex, int &secondIndex, const unique_ptr<double []> &content);
    static Descriptor computeDescriptor(const GaussianPyramid &pyramid, const Picture &sobelX, const Picture &sobelY, const InterestPoint &point, BorderMode border, const unique_ptr<double []> &content, const int index);
public:
    DescriptorSearch(const GaussianPyramid &pyramid, const Picture &sobelX, const Picture &sobelY, BorderMode border, const vector<InterestPoint> &points);
    static vector<NearestDescriptors> searchOverlap(const DescriptorSearch &first,const DescriptorSearch &second);
    static void saveOverlaps(QImage &image, QString filePath, const vector<NearestDescriptors> &overlaps, const int width);
};

#endif // DESCRIPTORSEARCH_H
