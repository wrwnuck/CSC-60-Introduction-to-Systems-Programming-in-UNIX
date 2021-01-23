/*---------------------------------*/
/* Warren Wnuck                    */
/* class_stats.c                   */
/*---------------------------------*/

#include "lab5.h"

void class_stats(int grades[], int number_of_grades, double *average, int distribution[])
{
    *average = 0;
    int i, j;
    for (i = 0; i < DIST_SIZE; i++){
        distribution[i] = 0;
    }
    for (j = 0; j < number_of_grades; j++){
        *average = *average + grades[j];
        i = (int) grades[j] / 10;
        distribution[i] = distribution[i] + 1;
    }
    *average = *average / number_of_grades;
    return;
}

