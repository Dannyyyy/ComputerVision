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

static double gaussXY(int x, int y) {
    const double sigma = 2;
    return exp(-(pow(x,2) + pow(y,2))/(2*pow(sigma,2)))/(2 * M_PI * pow(sigma,2));
}

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
        auto content = computeContent(sobelX, sobelY, point, border, 0, SiftRegionSizeX,SiftRegionSizeY, SiftPartsCount, SiftHistogramSize);
        DescriptorSearch::findPeaks(firstIndex, secondIndex, content);
        (content[firstIndex] * 0.8 < content[secondIndex] ?
            descriptors.emplace_back(computeDescriptor(sobelX,sobelY,point, border, content, firstIndex)),
            descriptors.emplace_back(computeDescriptor(sobelX,sobelY,point, border, content, secondIndex)) :
            descriptors.emplace_back(computeDescriptor(sobelX,sobelY,point, border, content, firstIndex))
        );
    }
}

Descriptor DescriptorSearch::computeDescriptor(const Picture &sobelX, const Picture &sobelY, const InterestPoint &point, BorderMode border, const unique_ptr<double[]> &content, const int index){
    const int size = SiftRegionSizeX * SiftRegionSizeY * SiftPartsCount;
    const double h0 = content[index] ;
    const double hm = content[(index - 1 + size) % size] ;
    const double hp = content[(index + 1 + size) % size] ;
     /* quadratic interpolation */
    const double di = - 0.5 * (hp - hm) / (hp + hm - 2 * h0) ;
    const double angle = 2 * M_PI * (index + di + 0.5) / size ;

    auto descriptor = Descriptor{point.x, point.y};
    descriptor.content = computeContent(sobelX, sobelY, point, border, angle, RegionSizeX, RegionSizeY, PartsCount, HistogramSize);
    descriptorNormalize(descriptor);
    tresholdTrim(descriptor);
    descriptorNormalize(descriptor);
    return descriptor;
}

unique_ptr<double[]> DescriptorSearch::computeContent(const Picture &sobelX, const Picture &sobelY, const InterestPoint &point, BorderMode border, double aroundAngle, const int regionSizeX, const int regionSizeY, const int partsCount, const int histogramSize){
    const int descriptorSize = regionSizeX * regionSizeY * partsCount;
    auto content = make_unique<double []>(descriptorSize);
    const int size = regionSizeX * histogramSize;
    const int halfSize = size/2;
    const double cosAngle = cos(aroundAngle);
    const double sinAngle = sin(aroundAngle);
    for(int x=-size; x<size; x++){
        for(int y=-size; y<size; y++){

            const int aroundX = (int)(x*cosAngle + y*sinAngle) + halfSize;
            const int aroundY = (int)(y*cosAngle - x*sinAngle) + halfSize;
            if(aroundX < 0 || aroundX >= size || aroundY < 0 || aroundY >= size){
                continue;
            }

            const int pointX = point.x + x;
            const int pointY = point.y + y;
            const double dx = sobelX.getIntensity(pointX, pointY, border);
            const double dy = sobelY.getIntensity(pointX, pointY, border);
            const double w = sqrt(pow(dx,2) + pow(dy,2)) * gaussXY(x,y);

            const int histogramNum = aroundX/histogramSize*regionSizeX + aroundY/histogramSize;

            double angle = atan2(dy, dx) + M_PI - aroundAngle;
            if (angle > 2 * M_PI) {
                angle = angle - 2 * M_PI;
            }
            if (angle < 0) {
               angle = angle + 2 * M_PI;
            }

            double partNum = angle / M_PI / 2 * partsCount;
            partNum = max(0.0, min(partsCount - 0.001, partNum));

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

//
vector<NearestDescriptors> DescriptorSearch::searchOverlap(const DescriptorSearch &f,const DescriptorSearch &s){
    const int fDescriptorsCount = f.descriptors.size();
    const int sDescriptorsCount = s.descriptors.size();
    const double treshhold = 0.2;
    vector<double> distances = calculateDistance(f,s);
    double firstOverlapDistance;
    double secondOverlapDistance;
    vector<NearestDescriptors> overlaps;
    overlaps.resize(fDescriptorsCount);
    int firstOverlap, secondOverlap;

    for(int i=0;i<fDescriptorsCount;i++){
        const int index = i*sDescriptorsCount;
        if (distances[index] < distances[index + 1]){
            firstOverlap = 0;
            firstOverlapDistance = distances[index];
            secondOverlap = 1;
            secondOverlapDistance = distances[index + 1];
        }
        else{
            firstOverlap = 1;
            firstOverlapDistance = distances[index+1];
            secondOverlap = 0;
            secondOverlapDistance = distances[index];
        }

        for(int j=2;j<sDescriptorsCount;j++){
            double distance = distances[index+j];
            if(distance < distances[index+firstOverlap]){
                secondOverlap = firstOverlap;
                secondOverlapDistance = firstOverlapDistance;
                firstOverlap = j;
                firstOverlapDistance = distance;
                continue;
            }
            if(distance < distances[index+secondOverlap]){
                secondOverlap = j;
                secondOverlapDistance = distance;
            }
        }
        if(firstOverlapDistance/secondOverlapDistance < 0.5)
         {
            const int fX = f.descriptors[i].x;
            const int fY = f.descriptors[i].y;
            const int sX = s.descriptors[firstOverlap].x;
            const int sY = s.descriptors[firstOverlap].y;
            overlaps.emplace_back(NearestDescriptors{fX,fY,sX,sY});
        }
    }
    return overlaps;
}
//
/*
vector<NearestDescriptors> DescriptorSearch::searchOverlap(const DescriptorSearch &f,const DescriptorSearch &s){
    const int fDescriptorsCount = f.descriptors.size();
    const int sDescriptorsCount = s.descriptors.size();
    const double treshhold = 0.18;
    vector<double> distances = calculateDistance(f,s);

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
*/
void DescriptorSearch::saveOverlaps(QImage &image, QString filePath, const vector<NearestDescriptors> &overlaps, const int width){
    srand(time(NULL));
    {
    QPainter painter(&image);
    for(auto overlap: overlaps){
        painter.setPen(qRgb(255,0,0));
        painter.drawEllipse(overlap.fY, overlap.fX, 2,2);
        painter.drawEllipse(overlap.sY + width, overlap.sX, 2,2);
        const int red = abs(rand())%255;
        const int green = abs(rand())%255;
        const int blue = abs(rand())%255;
        //cout<<red<<":"<<green<<":"<<blue<<endl;
        cout<<overlap.fY<<" : "<<overlap.fX<<" : "<<overlap.sY + width<<" : "<<overlap.sX<<endl;
        painter.setPen(QColor(red,green, blue));
        painter.drawLine(overlap.fY, overlap.fX, overlap.sY + width, overlap.sX);
    }
    }
    image.save(filePath + "result.jpg", "jpg");
}
