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
    static unique_ptr<PictureFilterContent> getSobelGX();
    static unique_ptr<PictureFilterContent> getSobelGY();
    static unique_ptr<PictureFilterContent> getGaussX(double sigma);
    static unique_ptr<PictureFilterContent> getGaussY(double sigma);
    static unique_ptr<PictureFilterContent> getGaussXY(double sigma);
};

#endif // PICTUREFILTER_H
