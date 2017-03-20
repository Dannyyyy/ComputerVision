#define _USE_MATH_DEFINES
#include <cmath>
#include <descriptorsearch.h>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

DescriptorSearch::DescriptorSearch(const Picture &picture, BorderMode border):picture(picture){
    const double treshold = 0.01;
    auto interestPoints = new PointSearch(picture);
    interestPoints->harris(border, treshold);
    interestPoints->adaptiveNonMaxSuppression(15);
    for(auto point : interestPoints->Points()){
        auto descriptor = Descriptor{point.x, point.y};
        descriptor.content = computeContent(point, border);
        descriptorNormalize(descriptor);
        tresholdTrim(descriptor);
        descriptorNormalize(descriptor);
        descriptors.emplace_back(move(descriptor));
    }
}

unique_ptr<double[]> DescriptorSearch::computeContent(InterestPoint &point, BorderMode border){
    auto sobelX = picture.useFilter(PictureFilter::getSobelGX(),border);
    auto sobelY = picture.useFilter(PictureFilter::getSobelGY(),border);

    const int descriptorSize = regionSizeX * regionSizeY * partsCount;
    auto content = make_unique<double []>(descriptorSize);
    const int size = regionSizeX * histogramSize;
    const int halfSize = size/2;
    auto gauss = PictureFilter::getGaussXY(histogramSize/2);
    double dx, dy;

    for(int x=-halfSize; x<halfSize; x++){
        for(int y=-halfSize; y<halfSize; y++){
            int pointX = point.x + x;
            int pointY = point.y + y;
            dx = sobelX.getIntensity(pointX, pointY, border);
            dy = sobelY.getIntensity(pointX, pointY, border);
            double w = sqrt(pow(dx,2) + pow(dy,2)) * gauss.getContentCell(x+halfSize, y+halfSize);
            const int histogramNum = (x + halfSize)/histogramSize*regionSizeX + (y+halfSize)/histogramSize;
            double partNum = (atan2(dy,dx)+M_PI)*partsCount/(2*M_PI);
            const double partVariation = partNum - (int)partNum;
            int index = histogramNum * partsCount + (int)partNum % partsCount;
            content[index] += w * (1 - partVariation);
            index = histogramNum * partsCount + (int)(partNum+1) % partsCount;
            content[index] += w * partVariation;
        }
    }
    return content;
}

void DescriptorSearch::descriptorNormalize(Descriptor &descriptor){
    double sum = 0;
    const int size = regionSizeX * regionSizeY * partsCount;
    auto accumulateFunc = [](double accumulator, double element){
       return accumulator + pow(element,2);
    };
    auto transformFunc = [&](double element){
       return element/sum;
    };
    sum = accumulate(&descriptor.content[0],&descriptor.content[size],0.,accumulateFunc);
    sum = sqrt(sum);
    transform(&descriptor.content[0],&descriptor.content[size],&descriptor.content[0],transformFunc);
    /*
    vector<double> v = { 1, 2, 3, 4 };
    sum = accumulate(v.begin(),v.end(),0.,accumulateFunc);
    sum = sqrt(sum);
    transform(v.begin(), v.end(),v.begin(),transformFunc);
    */
}

void DescriptorSearch::tresholdTrim(Descriptor &descriptor){
    const double treshold = 0.2;
    const int size = regionSizeX * regionSizeY * partsCount;
    auto transformFunc = [&](double element){
       return min(element, treshold);
    };
    transform(&descriptor.content[0],&descriptor.content[size],&descriptor.content[0],transformFunc);
    /*
    vector<double> v = { 0.1, 2, 0.3, 0.04 };
    transform(v.begin(), v.end(),v.begin(),transformFunc);
    */
}

void DescriptorSearch::searchOverlap(DescriptorSearch &f, DescriptorSearch &s){
    const int fDescriptorsCount = f.descriptors.size();
    const int sDescriptorsCount = s.descriptors.size();
    const int descriptorSize = regionSizeX * regionSizeY * partsCount;
    double distance;
    vector<double> distances;
    distances.reserve(fDescriptorsCount * sDescriptorsCount);
    for(int i=0;i<fDescriptorsCount;i++){
        for(int j=0;j<sDescriptorsCount;j++){
            distance = 0;
            for(int n=0;n<descriptorSize;n++){
                const double dist = f.descriptors[i].content[n] - s.descriptors[j].content[n];
                distance += pow(dist,2);
            }
            distances[i*fDescriptorsCount+j] = sqrt(distance);
        }
    }
}
