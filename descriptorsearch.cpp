#include <descriptorsearch.h>

DescriptorSearch::DescriptorSearch(const Picture &picture, BorderMode border):picture(picture){
    auto interestPoints = new PointSearch(picture);
    interestPoints->harris(border, 0.01);
    interestPoints->adaptiveNonMaxSuppression(500);
    for(auto point : interestPoints->Points()){
        descriptors.emplace_back(Descriptor{point.x,point.y});
    }
}

void DescriptorSearch::descriptorNormalize(){

}
