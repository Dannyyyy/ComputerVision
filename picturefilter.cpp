#include <picturefilter.h>

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
