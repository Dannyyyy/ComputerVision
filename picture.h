#ifndef PICTURE_H
#define PICTURE_H

#include <memory>
#include <vector>

using namespace std;

class Picture
{
private:
    int height;
    int width;
    unique_ptr<double []> content;
public:
    Picture();
    Picture(const int width, const int height);
    int Height(){ return this->height; }
    int Width(){ return this->width; }
};
#endif // PICTURE_H
