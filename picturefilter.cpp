#define _USE_MATH_DEFINES
#include <picturefilter.h>
#include <cmath>

unique_ptr<PictureFilterContent> PictureFilter::getSobelGX(){
        auto pictureFilterContent = make_unique<PictureFilterContent>(3,3);
        double sobelGXContent[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
        copy(begin(sobelGXContent), end(sobelGXContent), pictureFilterContent->content.get());
        return pictureFilterContent;
}

unique_ptr<PictureFilterContent> PictureFilter::getSobelGY(){
        auto pictureFilterContent = make_unique<PictureFilterContent>(3,3);
        double sobelGYContent[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
        copy(begin(sobelGYContent), end(sobelGYContent), pictureFilterContent->content.get());
        return pictureFilterContent;
}

unique_ptr<PictureFilterContent> PictureFilter::getGaussX(double sigma){
    const int halfSize = (int)(sigma*3);
    const int size = halfSize*2;
    auto pictureFilterContent = make_unique<PictureFilterContent>(1,size+1);
    for(int x=0; x<size; x++){
        pictureFilterContent->content[x] = exp(-(pow((x-halfSize),2)/(2*pow(sigma,2))))/(sqrt(2*M_PI)*sigma);
    }
    return pictureFilterContent;
}

unique_ptr<PictureFilterContent> PictureFilter::getGaussY(double sigma){
    const int halfSize = (int)(sigma*3);
    const int size = halfSize*2;
    auto pictureFilterContent = make_unique<PictureFilterContent>(size+1,1);
    for(int y=0; y<size; y++){
        pictureFilterContent->content[y] = exp(-(pow((y-halfSize),2)/(2*pow(sigma,2))))/(sqrt(2*M_PI)*sigma);
    }
    return pictureFilterContent;
}

unique_ptr<PictureFilterContent> PictureFilter::getGaussXY(double sigma){
    const int halfSize = (int)(sigma*3);
    const int size = halfSize*2;
    auto pictureFilterContent = make_unique<PictureFilterContent>(size+1,size+1);
    for(int x=0; x<size; x++){
        for(int y=0; y<size; y++){
            pictureFilterContent->content[y*(size+1) + x] = exp(-((pow((x-halfSize),2) + pow((y-halfSize),2))
                                / (2*pow(sigma,2))))/(2*M_PI*pow(sigma,2));
        }
    }
    return pictureFilterContent;
}


