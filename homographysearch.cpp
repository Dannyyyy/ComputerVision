#include <homographysearch.h>

using namespace std;

vector<NearestDescriptors> getRandomOverlaps(vector<NearestDescriptors> overlaps, int count) {
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    vector<int> indexes;
    indexes.resize(count);

    vector<NearestDescriptors> selectedOverlaps;
    selectedOverlaps.resize(count);

    const int overlapsCount = overlaps.size();

    for (int i = 0; i < count; ++i)
        indexes[i] = qrand() % overlapsCount;

    repeat:
    for (int i = 0; i < count-1; ++i)
        for (int j = i+1; j < count; ++j)
            if (indexes[i] == indexes[j]){
                indexes[j] = qrand() % overlapsCount;
                goto repeat;
            }

    for(int i=0;i<count;i++){
        const int index = indexes[i];
        selectedOverlaps[i] = overlaps[index];
    }

    return selectedOverlaps;
}

vector<double> HomographySearch::ransac(vector<NearestDescriptors> overlaps) {
    int wellInliersCount = 0;

    vector<double> wellTransform;
    wellTransform.resize(9);

    for (int repeat = 0; repeat < repeats; repeat++) {
        vector<NearestDescriptors> selectedOverplaps;
        selectedOverplaps.resize(4);
        selectedOverplaps = getRandomOverlaps(overlaps, 4);
    }

    return wellTransform;
}
