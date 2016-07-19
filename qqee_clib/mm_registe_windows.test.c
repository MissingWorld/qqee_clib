#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "mm_registe_windows.h"

int main(){
	int a=mm_registe_set_power("MACHINE\\SYSTEM\\Setup\\SetuprmHashStatus\\{26ACDB38-4812-4393-B91C-A315EB60E8B7}",0,2,268435456,"Everyone",0);
	printf("end=%d\n",a);
	return 1;
}
