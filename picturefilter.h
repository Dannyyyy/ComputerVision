#ifndef PICTUREFILTER_H
#define PICTUREFILTER_H

#include <memory>
#include <iterator>

using namespace std;

class PictureFilterContent{
    int height;
    int width;
public:
    unique_ptr<double []> content;
    PictureFilterContent(){}
    PictureFilterContent(int height, int width){
        this->height = height;
        this->width = width;
        this->content = make_unique<double []>(height * width);
    }
    int getHeight() const { return height; }
    int getWidth() const { return width; }
    double getContentCell(int x, int y) const {return content[(y * width) + x];}
};

class PictureFilter{
public:
    static PictureFilterContent getSobelGX();
    static PictureFilterContent getSobelGY();
    static PictureFilterContent getGaussX(double sigma);
    static PictureFilterContent getGaussY(double sigma);
    static PictureFilterContent getGaussXY(double sigma);
};

#endif // PICTUREFILTER_H
