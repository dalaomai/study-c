class A;

class B
{
public:
    B(A &a);

private:
    A *a;
};

#include "iosfwd"