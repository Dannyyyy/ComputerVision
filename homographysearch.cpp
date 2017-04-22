#include <homographysearch.h>

using namespace std;

vector<int> getRandomIndexes(const int size, const int count) {
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    vector<int> indexes;
    indexes.resize(count);


    for (int i = 0; i < count; ++i)
        indexes[i] = qrand() % size;

    repeat:
    for (int i = 0; i < count-1; ++i){
        for (int j = i+1; j < count; ++j){
            if (indexes[i] == indexes[j]){
                indexes[j] = qrand() % size;
                goto repeat;
            }
        }
    }

    return indexes;
}

void calculateMatrixA(gsl_matrix *A, vector<NearestDescriptors> overlaps, vector<int> indexes)
{
    const int sizeA = A->size1;
    const int halsfSize = sizeA/2;

    for(int i=0; i<halsfSize; i++)
    {
        const int index = indexes[i];
        const double xS = overlaps[index].sY;
        const double yS = overlaps[index].sX;
        const double xF = overlaps[index].fY;
        const double yF = overlaps[index].fX;


        gsl_matrix_set(A, i*2, 0, xS); gsl_matrix_set(A, i*2, 1, yS); gsl_matrix_set(A, i*2, 2, 1);

        gsl_matrix_set(A, i*2, 3, 0); gsl_matrix_set(A, i*2, 4, 0); gsl_matrix_set(A, i*2, 5, 0);

        gsl_matrix_set(A, i*2, 6, -xF*xS); gsl_matrix_set(A, i*2, 7, -xF*yS); gsl_matrix_set(A, i*2, 8, -xF);


        gsl_matrix_set(A, i*2+1, 0, 0); gsl_matrix_set(A, i*2+1, 1, 0); gsl_matrix_set(A, i*2+1, 2, 0);

        gsl_matrix_set(A, i*2+1, 3, xS); gsl_matrix_set(A, i*2+1, 4, yS); gsl_matrix_set(A, i*2+1, 5, 1);

        gsl_matrix_set(A, i*2+1, 6, -yF*xS); gsl_matrix_set(A, i*2+1, 7, -yF*yS); gsl_matrix_set(A, i*2+1, 8, -yF);
    }
}

vector<double> HomographySearch::ransac(vector<NearestDescriptors> overlaps) {

    gsl_matrix* A = gsl_matrix_alloc(8,9);

    vector<double> wellTransform;
    wellTransform.resize(9);

    for (int repeat = 0; repeat < repeats; repeat++) {
        vector<int> indexes;
        indexes.resize(4);
        indexes = getRandomIndexes(overlaps.size(), 4);
        calculateMatrixA(A, overlaps, indexes);
    }

    gsl_matrix_free(A);
    return wellTransform;
}
