#include <descriptorsearch.h>
#include <numeric>
#include <algorithm>
#include <iostream>

DescriptorSearch::DescriptorSearch(const Picture &picture, BorderMode border):picture(picture){
    const double treshold = 0.01;
    const int descriptorSize = histogramXCount * histogramYCount * histogramComponentsCount;
    auto interestPoints = new PointSearch(picture);
    interestPoints->harris(border, treshold);
    interestPoints->adaptiveNonMaxSuppression(500);
    for(auto point : interestPoints->Points()){
        auto descriptor = Descriptor{point.x, point.y, descriptorSize};
        //descriptor content
        //descriptorNormalize(descriptor);
        //tresholdTrim(descriptor);
        //descriptorNormalize(descriptor);
        //descriptors.emplace_back(descriptor);
    }
}

void DescriptorSearch::computeContent(BorderMode border){
    auto sobelX = picture.useFilter(PictureFilter::getSobelGX(),border);
    auto sobelY = picture.useFilter(PictureFilter::getSobelGY(),border);
    const int descriptorSize = histogramXCount * histogramYCount * histogramComponentsCount;
    auto content = make_unique<double []>(descriptorSize);
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
