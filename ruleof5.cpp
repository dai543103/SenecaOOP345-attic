#include <iostream>
#include <cstring>    /// memcpy

using namespace std;

template <class T>
class X {
  size_t size = 0;         // C++14
  T *data     = nullptr;   // C++14
public:
  X() // : size(0), data(nullptr)    <-- initialization done above with C++14
  { cout << (void*) this << ": X constructor()\n"; }

  X(size_t s) : size(s), data(new T[size]) 
  { cout << (void*) this << ": X (" << s << ") constructor\n"; }

  ~X() { 
    cout << (void*) this << ": X destructor, size=" << size << "\n";
    delete [] data;
  }

  X& operator= (const X& rhs) // C++98 copy assignment operator
  {
    cout << (void*) this << ": assignment operator size,rhs.size=" << size << "," << rhs.size << "\n";
    if(this != &rhs) {
      delete [] data;

      data = nullptr;  // we are a zombie
      size = 0;

      if(rhs.data) {
        size = rhs.size;
        data = new T[size];
        // void *memcpy(void *dest, const void *src, size_t n);
        // SYNOPSIS
               // #include <string.h>
               // void *memcpy(void *dest, const void *src, size_t n);
        // DESCRIPTION
               // The  memcpy()  function  copies  n bytes from memory area src to memory
               // area dest.  The memory areas must not overlap.  Use memmove(3)  if  the
               // memory areas do overlap.
        // RETURN VALUE
               // The memcpy() function returns a pointer to dest.
        
        memcpy( data, rhs.data, size * sizeof(T) );  // returns dest (data in our case)
        // data = memcpy( new T[size], rhs.data, size * sizeof(T) );  // 1 line
        // for(size_t i=0; i<size;i++) data[i]=rhs.data[i]; // DEEP COPY using a for loop
      }
    } else {
      cout << (void*) this << ": copy assignment operator called on itself\n";
    }
    return *this;
  }

  X(const X& rhs) // C++98 copy constructor
  {
    cout << (void*) this << ": copy constructor rhs.size=" << rhs.size << "\n";
    data = nullptr;
    *this = rhs;      // let assignment operator do the work.
  }

  void print() 
  {
    cout << (void*)this << ": size=" << size << " (" << size*sizeof(T) << " BYTES)\n";
  }

  X&& operator= (X&& rhs) // C++11 move assignment operator
  {
    cout << (void*) this << ": move assignment operator size,rhs.size=" << size << "," << rhs.size << "\n";
    if(this != &rhs) {
      delete [] data;

      size = rhs.size; // Steal brains (copy size+pointer)
      data = rhs.data;

      rhs.size = 0;    // Zombie
      rhs.data = nullptr;
    } else {
      cout << (void*) this <<": move assignment operator called on itself\n";
    }
    return std::move(*this);
  }

  X(X&& rhs) // C++11 move constructor
  {
    cout << (void*) this << ": move constructor rhs.size=" << rhs.size << "\n";
    data = nullptr;
    *this = std::move(rhs);      // Let move assignment operator do the work.
  }
};

int main(int argc, char**argv)
{
 cout << "program " << argv[0] << "\n\n";

 X<double> x, y(125 * 1000 * 1000);  // 8 * 125 * 1000 * 1000 = 1,000,000,000 BYTES

 cout << "LINE " << __LINE__ << ": x = y;\n";
 x = y;

 cout << "LINE " << __LINE__ << ": X<double> z(x);\n";
 X<double> z(x);

 cout << "LINE " << __LINE__ << ": y = std::move(x);\n";
 y = std::move(x);
 cout << "x:"; x.print();
 cout << "y:"; y.print();

 cout << "LINE " << __LINE__ << ": x = std::move(y);\n";
 x = std::move(y);
 cout << "x:"; x.print();
 cout << "y:"; y.print();

 cout << "LINE " << __LINE__ << ": y = std::move(x);\n";
 y = std::move(x);
 cout << "x:"; x.print();
 cout << "y:"; y.print();

 cout << "LINE " << __LINE__ << ": x = std::move(y);\n";
 x = std::move(y);
 cout << "x:"; x.print();
 cout << "y:"; y.print();

 cout << "LINE " << __LINE__ << ": X<double> A(std::move(x));\n";
 X<double> A(std::move(x));
 cout << "x:"; x.print();
 cout << "A:"; A.print();

 x = x;
 x = std::move(x);
 
 cout << "--------------------\n";
 cout << "What happens with 'x = X<double> (1234);'?\n";
 cout << "Is right hand side 'X<double> (1234);' a l-value or a r-value?\n";
 x = X<double> (1234);
 cout << "--------------------\n";

 cout << "  ANSWER 'X<double> (1234);' <-- is a r-value, as expected\n";
 cout << "    PROOF PART 1: X (1234) constructor\n";
 cout << "    PROOF PART 2: move assignment operator size,rhs.size=0,1234\n";
 cout << "    PROOF PART 3: X destructor, size=0\n";
 cout << "--------------------\n";

 cout << "NOTE: these all call the basic constructor\n";
 cout << "  1. X<double> x1 = X<double> (1234); --> "; X<double> x1 = X<double> (1234);
 cout << "  2. X<double> x2(X<double> (1234) ); --> "; X<double> x2( X<double> (1234) );
 cout << "  3. X<double> x3(1234);              --> "; X<double> x3(1234);
 cout << "--------------------\n";

 cout << "LINE " << __LINE__ << ": MAIN OVER\n\n";
}
