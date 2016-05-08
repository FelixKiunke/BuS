#include <stddef.h>
#include <stdio.h>

/* Definition von globalen Variablen.
 * Diese Variablen können überall im Programm verändert werden.
 */
int max = 0;
int array[10] = {4, 6, 2, 0, 9, 1, 5, 7, 8, 3};
size_t anz = sizeof(array)/sizeof(*array);

/* Diese Funktion vergleicht und sortiert die beiden Feldeinträge
   "i1" und "i2" im Array "array".
 */
void exchange(size_t i1, size_t i2) {

    if(array[i1] > array[i2])
    {
        if (array[i1] > max){
            max = array[i1];
		}
    }
    else
    {
        if(array[i2] > max){
            max = array[i2];
		}
        int tmp = array[i1];
        array[i1] = array[i2];
        array[i2] = tmp;
    }
}

int main()
{
    for(size_t i = 0; i < anz; i++)
    {
        for(size_t j = i + 1; j < anz; j++)
            exchange(i, j);
    }

    printf("Die Zahlen in sortierter Reihenfolge:");
    for(size_t i=0; i < anz; i++){
        printf(" %d", array[i]);
	}
    printf("\nDas Maximum: %d\n", max);

    return 0;
}
