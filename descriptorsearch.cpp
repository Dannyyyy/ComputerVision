#define _USE_MATH_DEFINES
#include <cmath>
#include <descriptorsearch.h>
#include <numeric>
#include <algorithm>
#include <iostream>

DescriptorSearch::DescriptorSearch(const Picture &picture, BorderMode border):picture(picture){
    const double treshold = 0.01;
    const int descriptorSize = regionSizeX * regionSizeY * partsCount;
    auto interestPoints = new PointSearch(picture);
    interestPoints->harris(border, treshold);
    interestPoints->adaptiveNonMaxSuppression(3);
    for(auto point : interestPoints->Points()){
        auto descriptor = Descriptor{point.x, point.y, descriptorSize};
        this->computeContent(point, border);
        //descriptor content
        //descriptorNormalize(descriptor);
        //tresholdTrim(descriptor);
        //descriptorNormalize(descriptor);
        //descriptors.emplace_back(descriptor);
    }
}

void DescriptorSearch::computeContent(InterestPoint &point, BorderMode border){
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
            content[index] = w * (1 - partVariation);
            index = histogramNum * partsCount + (int)(partNum+1) % partsCount;
            content[index] += w * partVariation;
        }
    }
}
void DescriptorSearch::descriptorNormalize(Descriptor &descriptor){
    double sum = 0;
    const int size = descriptor.size;
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
    const int size = descriptor.size;
    auto transformFunc = [&](double element){
       return min(element, treshold);
    };
    transform(&descriptor.content[0],&descriptor.content[size],&descriptor.content[0],transformFunc);
    /*
    vector<double> v = { 0.1, 2, 0.3, 0.04 };
    transform(v.begin(), v.end(),v.begin(),transformFunc);
    */
}
