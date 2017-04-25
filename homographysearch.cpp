#include <homographysearch.h>

using namespace std;

vector<int> getRandomIndexes(const int size, const int count) {
    qsrand(QTime::currentTime().msec());

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

void calculateMatrixA(gsl_matrix *A, const vector<NearestDescriptors> &overlaps, const vector<int> &indexes)
{
    const int sizeMatrixA = A->size1;
    const int halfSize = sizeMatrixA/2;

    for(int i=0; i<halfSize; i++)
    {
        const int index = indexes[i];
        const double xR = overlaps[index].sY;
        const double yR = overlaps[index].sX;
        const double xL = overlaps[index].fY;
        const double yL = overlaps[index].fX;

        double rowF[] = {xR, yR, 1, 0, 0, 0, -xL*xR, -xL*yR, -xL};
        double rowS[] = {0, 0, 0, xR, yR, 1, -yL*xR, -yL*yR, -yL};
        for (int j = 0; j < 9; j++) {
            const int doubleIndex = i*2;
            gsl_matrix_set(A, doubleIndex, j, rowF[j]);
            gsl_matrix_set(A, doubleIndex + 1, j, rowS[j]);
        }
    }
}

void putMatrixH(gsl_matrix* H, gsl_vector_view columnH)
{
    auto vector = columnH.vector;

    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            gsl_matrix_set(H,i,j,gsl_vector_get(&vector,i*3+j));
        }
    }
}

void dlt(gsl_matrix *A, gsl_matrix *AT, gsl_matrix *U, gsl_matrix *H)
{
    gsl_vector* help1 = gsl_vector_alloc(3*3);
    gsl_vector* help2 = gsl_vector_alloc(3*3);

    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, A, A, 0.0, AT);
    gsl_linalg_SV_decomp(AT, U, help1, help2);

    auto columnH = gsl_matrix_column(U, 8);
    putMatrixH(H, columnH);

    gsl_vector_free(help1);
    gsl_vector_free(help2);
}

vector<int> getInliers(gsl_matrix* H, const vector<NearestDescriptors> overlaps, double treshold)
{
    vector<int> currentInliers;
    const int overlapsCount = overlaps.size();

    for(int i=0; i<overlapsCount; i++)
    {
        const double xS = overlaps[i].sY;
        const double yS = overlaps[i].sX;
        const double xF = overlaps[i].fY;
        const double yF = overlaps[i].fX;

        double initial[3] = {xS, yS, 1.0};
        double result[3];

        gsl_vector_view resultVector = gsl_vector_view_array(result, 3);
        gsl_vector_view initialVector = gsl_vector_view_array(initial, 3);
        gsl_blas_dgemv(CblasNoTrans, 1.0, H, &initialVector.vector, 0.0, &resultVector.vector);

        double scaleResult = result[2];
        double xResult = result[0]/scaleResult;
        double yResult = result[1]/scaleResult;
        double distance = sqrt(pow(xF - xResult,2) + (yF - yResult,2));

        if(distance < treshold)
        {
            currentInliers.emplace_back(i);
        }
    }
    return currentInliers;
}

vector<double> HomographySearch::ransac(const vector<NearestDescriptors> &overlaps) {
    gsl_matrix* A = gsl_matrix_alloc(8,9);
    gsl_matrix* AT = gsl_matrix_alloc(9,9);
    gsl_matrix* U = gsl_matrix_alloc(9,9);
    gsl_matrix* H = gsl_matrix_alloc(3,3);

    vector<double> wellTransform;
    wellTransform.resize(9);

    vector<int> wellInliers;

    for (int repeat = 0; repeat < repeats; repeat++) {
        vector<int> indexes;
        indexes.resize(4);
        indexes = getRandomIndexes(overlaps.size(), 4);
        calculateMatrixA(A, overlaps, indexes);
        dlt(A, AT, U, H);

        const double treshold = 5;
        vector<int> currentInliers = getInliers(H, overlaps, treshold);

        const int currentInliersCount = currentInliers.size();
        const int wellInliersCount = wellInliers.size();
        if(currentInliersCount > wellInliersCount)
        {
            wellInliers = currentInliers;
        }
    }
    // уточнение модели
    const int wellInliersCount = wellInliers.size();
    const int wellInliersDoubleCount = wellInliersCount * 2;
    gsl_matrix* resultA = gsl_matrix_alloc(wellInliersDoubleCount, 9);
    gsl_matrix* resultAT = gsl_matrix_alloc(9,9);

    calculateMatrixA(resultA, overlaps, wellInliers);
    dlt(resultA, resultAT, U, H);

    double scaleMatrix = gsl_matrix_get(H,2,2);
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            int index = j*3 + i;
            double value = gsl_matrix_get(H, i, j);
            wellTransform[index] = value/scaleMatrix;
        }
    }

    gsl_matrix_free(A);
    gsl_matrix_free(AT);
    gsl_matrix_free(H);
    gsl_matrix_free(U);

    gsl_matrix_free(resultA);
    gsl_matrix_free(resultAT);

    return wellTransform;
}
