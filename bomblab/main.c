#include<stdio.h>
void f(int *i);

int i = 0;

int main()
{
    i--;
    f(&i);
    f(&i);
    f(&i);
    return 0;
}

void f(int *i)
{
    *i++;
    printf("%d", *i);
}