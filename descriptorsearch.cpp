#define _USE_MATH_DEFINES
#include <cmath>
#include <descriptorsearch.h>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <vector>
#include <QPainter>
#include <cstdlib>
#include <ctime>

using namespace std;

void DescriptorSearch::findPeaks(int &firstIndex, int &secondIndex, const unique_ptr<double[]> &content){
    const int descriptorSize = SiftRegionSizeX * SiftRegionSizeY * SiftPartsCount;
    (content[firstIndex] > content[secondIndex] ?
        firstIndex = 0, secondIndex = 1 :
        secondIndex = 0, firstIndex = 1
    );
    for (int i = 0; i < descriptorSize; i++) {
        (content[secondIndex] < content[i] ?
            (content[firstIndex] < content[i] ?
                secondIndex = firstIndex, firstIndex = i :
                secondIndex = i
            ) :
            firstIndex = firstIndex, secondIndex = secondIndex
        );
    }
}

DescriptorSearch::DescriptorSearch(const Picture &sobelX, const Picture &sobelY, BorderMode border, const vector<InterestPoint> &points){
    for(auto point : points){
        int firstIndex = 0, secondIndex = 1;
        auto content = DescriptorSearch::computeContent(sobelX, sobelY, point, border, 0, SiftRegionSizeX,SiftRegionSizeY, SiftPartsCount, SiftHistogramSize);
        DescriptorSearch::findPeaks(firstIndex, secondIndex, content);

        (content[firstIndex] * 0.8 < content[secondIndex] ?
            descriptors.emplace_back(move(DescriptorSearch::computeDescriptor(sobelX,sobelY,point, border, content, firstIndex))),
            descriptors.emplace_back(move(DescriptorSearch::computeDescriptor(sobelX,sobelY,point, border, content, secondIndex))) :
            descriptors.emplace_back(move(DescriptorSearch::computeDescriptor(sobelX,sobelY,point, border, content, firstIndex)))
        );

        /*auto descriptor = Descriptor{point.x, point.y};
        descriptor.content = DescriptorSearch::computeContent(sobelX, sobelY, point, border, 0);
        DescriptorSearch::descriptorNormalize(descriptor);
        DescriptorSearch::tresholdTrim(descriptor);
        DescriptorSearch::descriptorNormalize(descriptor);
        descriptors.emplace_back(move(descriptor));
        */
    }
}

Descriptor DescriptorSearch::computeDescriptor(const Picture &sobelX, const Picture &sobelY, const InterestPoint &point, BorderMode border, const unique_ptr<double[]> &content, const int index){
    const int size = SiftRegionSizeX * SiftRegionSizeY * SiftPartsCount;
    const double h0 = content[index] ;
    const double hm = content[(index - 1 + size) % size] ;
    const double hp = content[(index + 1 + size) % size] ;
     // quadratic interpolation
    const double di = - 0.5 * (hp - hm) / (hp + hm - 2 * h0) ;
    const double angle = 2 * M_PI * (index + di + 0.5) / size ;
    cout<<angle<<endl;

    auto descriptor = Descriptor{point.x, point.y};
    descriptor.content = DescriptorSearch::computeContent(sobelX, sobelY, point, border, angle, RegionSizeX, RegionSizeY, PartsCount, HistogramSize);
    DescriptorSearch::descriptorNormalize(descriptor);
    DescriptorSearch::tresholdTrim(descriptor);
    DescriptorSearch::descriptorNormalize(descriptor);
    return descriptor;
}

unique_ptr<double[]> DescriptorSearch::computeContent(const Picture &sobelX, const Picture &sobelY, const InterestPoint &point, BorderMode border, double aroundAngle, const int regionSizeX, const int regionSizeY, const int partsCount, const int histogramSize){
    const int descriptorSize = regionSizeX * regionSizeY * partsCount;
    auto content = make_unique<double []>(descriptorSize);
    const int size = regionSizeX * histogramSize;
    const int halfSize = size/2;
    auto gauss = PictureFilter::getGaussXY(histogramSize/2);
    const double cosAngle = cos(aroundAngle);
    const double sinAngle = sin(aroundAngle);
    for(int x=-size; x<size; x++){
        for(int y=-size; y<size; y++){

            const int aroundX = (int)(x*cosAngle + y*sinAngle);
            const int aroundY = (int)(y*cosAngle - x*sinAngle);
            if(aroundX < 0 || aroundX >= size || aroundY < 0 || aroundY >= size){
                continue;
            }

            const int pointX = point.x + x;
            const int pointY = point.y + y;
            const double dx = sobelX.getIntensity(pointX, pointY, border);
            const double dy = sobelY.getIntensity(pointX, pointY, border);
            const double w = sqrt(pow(dx,2) + pow(dy,2)) * gauss.getContentCell(x+size, y+size);

            const int histogramNum = aroundX/histogramSize*regionSizeX + aroundY/histogramSize;

            const double partNum = ((atan2(dy,dx) - aroundAngle) / M_PI + 1)*partsCount/2;

            const double partVariation = partNum - (int)partNum;
            int index = histogramNum * partsCount + (int)round(partNum) % partsCount;
            content[index] += w * (1 - partVariation);
            index = (int)(histogramNum * partsCount + partNum + 1) % partsCount;
            content[index] += w * partVariation;
        }
    }
    return content;
}

void DescriptorSearch::descriptorNormalize(Descriptor &descriptor){
    double sum = 0;
    const int size = RegionSizeX * RegionSizeY * PartsCount;
    auto accumulateFunc = [](double accumulator, double element){
       return accumulator + pow(element,2);
    };
    auto transformFunc = [&](double element){
       return element/sum;
    };
    sum = accumulate(&descriptor.content[0],&descriptor.content[size],0.,accumulateFunc);
    sum = sqrt(sum);
    transform(&descriptor.content[0],&descriptor.content[size],&descriptor.content[0],transformFunc);
}

void DescriptorSearch::tresholdTrim(Descriptor &descriptor){
    const double treshold = 0.2;
    const int size = RegionSizeX * RegionSizeY * PartsCount;
    auto transformFunc = [&](double element){
       return min(element, treshold);
    };
    transform(&descriptor.content[0],&descriptor.content[size],&descriptor.content[0],transformFunc);
}

vector<double> DescriptorSearch::calculateDistance(const DescriptorSearch &f, const DescriptorSearch &s){
    const int fDescriptorsCount = f.descriptors.size();
    const int sDescriptorsCount = s.descriptors.size();
    cout<<fDescriptorsCount<<endl;
    cout<<sDescriptorsCount<<endl;
    const int descriptorSize = RegionSizeX * RegionSizeY * PartsCount;
    vector<double> distances;
    distances.resize(fDescriptorsCount * sDescriptorsCount);
    for(int i=0;i<fDescriptorsCount;i++){
        for(int j=0;j<sDescriptorsCount;j++){
            double distance = 0;
            for(int n=0;n<descriptorSize;n++){
                const double dist = f.descriptors[i].content[n] - s.descriptors[j].content[n];
                distance += pow(dist,2);
            }
            distances[i*sDescriptorsCount+j] = sqrt(distance);
        }
    }
    return distances;
}

vector<NearestDescriptors> DescriptorSearch::searchOverlap(const DescriptorSearch &f,const DescriptorSearch &s){
    const int fDescriptorsCount = f.descriptors.size();
    const int sDescriptorsCount = s.descriptors.size();
    const double treshhold = 0.2;
    vector<double> distances = DescriptorSearch::calculateDistance(f,s);

    vector<NearestDescriptors> overlaps;
    overlaps.resize(fDescriptorsCount);
    int firstOverlap, secondOverlap;
    for(int i=0;i<fDescriptorsCount;i++){
        const int index = i*sDescriptorsCount;
        (distances[index] < distances[index + 1] ?
            firstOverlap = 0, secondOverlap = 1 :
            firstOverlap = 1, secondOverlap = 0
        );
        for(int j=2;j<sDescriptorsCount;j++){
            double distance = distances[index+j];
            double firstOverlapDistance = distances[index+firstOverlap];
            double secondOverlapDistance = distances[index+secondOverlap];
            (distance < secondOverlapDistance ?
                (distance < firstOverlapDistance ?
                    secondOverlap = firstOverlap, firstOverlap = j :
                    secondOverlap = j) :
                    (firstOverlap = firstOverlap, secondOverlap = secondOverlap)
            );
        }
        double firstOverlapDistance = distances[index+firstOverlap];
        double secondOverlapDistance = distances[index+secondOverlap];
        if(abs(firstOverlapDistance - secondOverlapDistance) > treshhold){
            const int fX = f.descriptors[i].x;
            const int fY = f.descriptors[i].y;
            const int sX = s.descriptors[firstOverlap].x;
            const int sY = s.descriptors[firstOverlap].y;
            overlaps.emplace_back(NearestDescriptors{fX,fY,sX,sY});
        }
    }
    return overlaps;
}

void DescriptorSearch::saveOverlaps(QImage &image, QString filePath, const vector<NearestDescriptors> &overlaps, const int width){
    srand(time(NULL));
    {
    QPainter painter(&image);
    for(auto overlap: overlaps){
        painter.setPen(qRgb(255,0,0));
        painter.drawEllipse(overlap.fX, overlap.fY, 2,2);
        painter.drawEllipse(overlap.sX + width, overlap.sY, 2,2);
        const int red = abs(rand())%255;
        const int green = abs(rand())%255;
        const int blue = abs(rand())%255;
        cout<<red<<":"<<green<<":"<<blue<<endl;
        painter.setPen(QColor(red,green, blue));
        painter.drawLine(overlap.fX, overlap.fY, overlap.sX + width, overlap.sY);
    }
    }
    image.save(filePath + "result.jpg", "jpg");
}
