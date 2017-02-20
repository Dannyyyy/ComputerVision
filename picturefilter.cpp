#include <picturefilter.h>

unique_ptr<PictureFilterData> PictureFilter::getSobelGX(){
        auto pictureFilter = make_unique<PictureFilterData>(3,3);
        double pictureFilterData[9] = { -1, 0, 1,
                                 -2, 0, 2,
                                 -1, 0, 1 };
        copy(begin(pictureFilterData), end(pictureFilterData), pictureFilter->content.get());
        return pictureFilter;
}

unique_ptr<PictureFilterData> PictureFilter::getSobelGY(){
        auto pictureFilter = make_unique<PictureFilterData>(3,3);
        double pictureFilterData[9] = { -1, -2, 1,
                                 0, 0, 0,
                                 1, 2, 1 };
        copy(begin(pictureFilterData), end(pictureFilterData), pictureFilter->content.get());
        return pictureFilter;
}
