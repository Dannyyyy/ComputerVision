#ifndef PICTUREFILTER_H
#define PICTUREFILTER_H

#include <memory>

using namespace std;

class PictureFilterData{
public:
    int height;
    int width;
    unique_ptr<double []> content;
    PictureFilterData(int height, int width){
        this->height = height;
        this->width = width;
        this->content = make_unique<double []>(height * width);
    }
};

class PictureFilter{
public:
    static unique_ptr<PictureFilterData> getSobelGX();
    static unique_ptr<PictureFilterData> getSobelGY();
};

#endif // PICTUREFILTER_H
