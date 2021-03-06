#define _USE_MATH_DEFINES
#include <picturefilter.h>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iostream>

using namespace std;

PictureFilterContent PictureFilter::getSobelGX(){
        auto pictureFilterContent = PictureFilterContent(3,3);
        double sobelGXContent[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
        copy(begin(sobelGXContent), end(sobelGXContent), pictureFilterContent.content.get());
        return pictureFilterContent;
}

PictureFilterContent PictureFilter::getSobelGY(){
        auto pictureFilterContent = PictureFilterContent(3,3);
        double sobelGYContent[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
        copy(begin(sobelGYContent), end(sobelGYContent), pictureFilterContent.content.get());
        return pictureFilterContent;
}

PictureFilterContent PictureFilter::getGaussX(double sigma){
    const int halfSize = (int)(sigma*3);
    const int size = halfSize*2;
    double sum = 0;
    auto filter = PictureFilterContent(1,size+1);
    for(int x=0; x<size; x++){
        filter.content[x] = exp(-(pow((x-halfSize),2)/(2*pow(sigma,2))))/(sqrt(2*M_PI)*sigma);
    }
    const int height = filter.getHeight();
    const int width = filter.getWidth();
    sum = accumulate(&filter.content[0], &filter.content[height*width],0.);
    transform(&filter.content[0], &filter.content[height*width],&filter.content[0],[&](double element){
       return element/sum;
    });
    return filter;
}

PictureFilterContent PictureFilter::getGaussY(double sigma){
    const int halfSize = (int)(sigma*3);
    const int size = halfSize*2;
    double sum = 0;
    auto filter = PictureFilterContent(size+1,1);
    for(int y=0; y<size; y++){
        filter.content[y] = exp(-(pow((y-halfSize),2)/(2*pow(sigma,2))))/(sqrt(2*M_PI)*sigma);
    }
    const int height = filter.getHeight();
    const int width = filter.getWidth();
    sum = accumulate(&filter.content[0], &filter.content[height*width],0.);
    transform(&filter.content[0], &filter.content[height*width],&filter.content[0],[&](double element){
       return element/sum;
    });
    return filter;
}

PictureFilterContent PictureFilter::getGaussXY(double sigma){
    const int halfSize = (int)(sigma*3);
    const int size = halfSize*2;
    auto filter = PictureFilterContent(size+1,size+1);
    double sum = 0;
    for(int x=0; x<size; x++){
        for(int y=0; y<size; y++){
            filter.content[y*(size+1) + x] = exp(-((pow((x-halfSize),2) + pow((y-halfSize),2))
                                / (2*pow(sigma,2))))/(2*M_PI*pow(sigma,2));
        }
    }
    const int height = filter.getHeight();
    const int width = filter.getWidth();
    sum = accumulate(&filter.content[0], &filter.content[height*width],0.);
    transform(&filter.content[0], &filter.content[height*width],&filter.content[0],[&](double element){
       return element/sum;
    });
    return filter;
}


