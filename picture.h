#ifndef PICTURE_H
#define PICTURE_H

#include <memory>

using namespace std;

class Picture
{
private:
    int height;
    int width;
public:
    Picture();
    Picture(const int width, const int height);
    int Height(){ return this->height; }
    int Width(){ return this->width; }
};
#endif // PICTURE_H
