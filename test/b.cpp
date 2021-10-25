#include "a.h"
#include "b.h"

B::B(A &a)
{
    B::a = &a;
    a.test();
}