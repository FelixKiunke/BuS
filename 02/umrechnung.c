#include <stdio.h>

int main()
{
    double k = 9.0/5.0;                         /* Gleitkommakonstante zum Umrechnen */

    printf("Temperatur in Grad Celsius: ");
    double celsius;                             /* Gleitkommavariable */
    scanf("%lf", &celsius);

    double fahrenheit = k * celsius + 32;

    printf("...ergibt in Fahrenheit: %6.2lf\n", fahrenheit);
    return 0;
}
