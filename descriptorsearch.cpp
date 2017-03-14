#include <descriptorsearch.h>
#include <numeric>
#include <algorithm>
#include <iostream>

DescriptorSearch::DescriptorSearch(const Picture &picture, BorderMode border):picture(picture){
    auto interestPoints = new PointSearch(picture);
    interestPoints->harris(border, 0.01);
    interestPoints->adaptiveNonMaxSuppression(500);
    for(auto point : interestPoints->Points()){
        //нормализация
        //обрезание по пороговому значению(0.2)
        //повторная нормализация
        //descriptors.emplace_back(Descriptor{point.x,point.y});
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
