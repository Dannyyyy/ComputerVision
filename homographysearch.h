#ifndef HOMOGRAPHYSEARCH_H
#define HOMOGRAPHYSEARCH_H

#include <vector>
#include <QTime>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <descriptorsearch.h>

class HomographySearch{
private:
    const int repeats = 5000;
public:
    vector<double> ransac(vector<NearestDescriptors> overlaps);
};

#endif // HOMOGRAPHYSEARCH_H
