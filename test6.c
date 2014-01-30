#include <stdio.h>
#include <assert.h>
int compute (int a) {
	int res;
	//assert(a != 0);
	//int b;
	res = a/a;
	return res;
}

int main() {
	printf("%d", compute (0));
	return 0;
}

