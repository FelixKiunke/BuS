#include <stdio.h>

double fahrenheit(int celsius){
	return (9.0/5.0)*((double)celsius) + 32;

}

int main(){
	printf("Celsiuswert | Fahrenheitswert\n");
    for(int i=0; i<=20; i+=2){
		printf("%-12.2lf| %-.2lf\n", (double)i, fahrenheit(i));
	}
    return 0;
}
