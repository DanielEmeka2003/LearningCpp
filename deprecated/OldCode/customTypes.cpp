#include "customTypes.h"

void Point3d::print() const
{
    std::cout << "Point: " << '<' << x << ", " << y << ", " << z << '>' << '\n';
}
bool Point3d::isEqual(const Point3d& obj)
{
    return (x == obj.x) && (y == obj.y) && (z == obj.z);
}
void Point3d::moveByVector(const Vector3d& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}
// supports chaining
Point3d& Point3d::firstPointAddedTo_others()
{
    x += y += z;
    return *this;
}

std::ostream& operator<<(std::ostream& stream, const Point2d& display)
{
    stream << '(' << display.x << ", " << display.y << ')' << '\n';
    return stream;
}
Point2d Point2d::getAdjacentPoint(PuzzleCommands direction) const
{
    if(direction == up and y > 0)
    return {x, y - 1};
    else if(direction == down and y < 3)
    return {x, y + 1};

    if(direction == left and x > 0)
    return {x - 1, y};
    else if(direction == right and x < 3)
    return {x + 1, y};

    return {x, y};
}
double distanceFrom(Point2d& a, Point2d& b)
{
    double square{std::pow((a.x - b.x), 2.0) + std::pow((a.y - b.y), 2.0)};
    return std::sqrt(square);
}
bool operator==(const Point2d& a, const Point2d& b)
{
    return (a.x == b.x) && (a.y == b.y);
}
bool operator!=(const Point2d& a, const Point2d& b)
{
    return !(a == b);
}

void Vector3d::print() const
{
    std::cout << "Vector: " << '<' << x << ", " << y << ", " << z << '>' << '\n';
}

ArrayOFInts::ArrayOFInts(size_t a) : length{a}
{
    assert(length > 0 && "Must be greater than 0, asshart.");
    ptr = new int[length];
}
ArrayOFInts::ArrayOFInts(const ArrayOFInts& source)
{
    deepCopy(source);
}
ArrayOFInts::~ArrayOFInts() 
{
    delete[] ptr;
}
size_t ArrayOFInts::size()
{
    return length;
}
int& ArrayOFInts::operator[](size_t index)
{
    assert( ( (int) index >= 0 || index < length) && "invalid index" );
    return ptr[index];
}
int ArrayOFInts::operator[](size_t index) const
{
    assert( ( (int)index >= 0 || index < length) && "invalid index" );
    return ptr[index];
}
ArrayOFInts& ArrayOFInts::operator=(const ArrayOFInts& source)
{
    if(this != &source)
    deepCopy(source);
    
    return *this;
}
void ArrayOFInts::deepCopy(const ArrayOFInts& source)
{
    delete[] ptr;
    length = source.length;
    if (source.ptr)
    {
        ptr = new int[length];
        for (int i{0}; i < static_cast<int>(length); ++i)
        {
            ptr[i] = source.ptr[i];
        }
    }
    else
    ptr = nullptr;
}
std::ostream& operator<<(std::ostream& out, const ArrayOFInts& arr)
{
    for (int* i{arr.ptr}; i != (arr.ptr + arr.length); ++i)
    {
        out << *i << ' ';
    }
    return out;
}

Fraction_c Fraction_c::reduce()
{
    int gcd{std::gcd(fractionTemplate.numerator, fractionTemplate.denominator)};
    
    if(gcd == 0) return *this;

    fractionTemplate.numerator /= gcd;
    fractionTemplate.denominator /=gcd;
    return *this;
}
Fraction_c Fraction_c::operator-()
{
    return Fraction_c{-fractionTemplate.numerator, fractionTemplate.denominator};
}
Fraction_c operator*(const Fraction_c& a, const Fraction_c& b) 
{
    int new_numerator{a.fractionTemplate.numerator * b.fractionTemplate.numerator};
    int new_denominator{a.fractionTemplate.denominator * b.fractionTemplate.denominator};
    return Fraction_c{new_numerator, new_denominator};
}
Fraction_c operator*(const Fraction_c& a, const int value)
{
    int new_numerator{a.fractionTemplate.numerator * value};
    return Fraction_c{new_numerator, a.fractionTemplate.denominator};
}
Fraction_c operator*(const int value, const Fraction_c& b)
{
    return b * value;
}
Fraction_c operator+(const Fraction_c& a, const Fraction_c& b)
{
    int new_denominator{std::lcm(a.fractionTemplate.denominator, b.fractionTemplate.denominator)};
    int first_numerator{(new_denominator/a.fractionTemplate.denominator) * a.fractionTemplate.numerator};
    int second_numerator{(new_denominator/b.fractionTemplate.denominator) * b.fractionTemplate.numerator};
    return Fraction_c{(first_numerator + second_numerator), new_denominator};
}
Fraction_c operator+(const Fraction_c& a, const int value)
{
    return {(a.fractionTemplate.numerator + (value * a.fractionTemplate.denominator)), a.fractionTemplate.denominator};
} 
Fraction_c operator+(const int value, const Fraction_c& b)
{
    return b + value;
} 
Fraction_c operator-(const Fraction_c& a, const Fraction_c& b)
{
    int new_denominator{std::lcm(a.fractionTemplate.denominator, b.fractionTemplate.denominator)};
    int first_numerator{(new_denominator/a.fractionTemplate.denominator) * a.fractionTemplate.numerator};
    int second_numerator{(new_denominator/b.fractionTemplate.denominator) * b.fractionTemplate.numerator};
    return Fraction_c{(first_numerator - second_numerator), new_denominator};
}
Fraction_c operator-(const Fraction_c& a, const int value)
{
    return {(a.fractionTemplate.numerator - (value * a.fractionTemplate.denominator)), a.fractionTemplate.denominator};
} 
Fraction_c operator-(const int value, const Fraction_c& b)
{
    return b - value;
}
bool operator==(const Fraction_c& a, const Fraction_c& b)
{
    return (a.fractionTemplate.numerator == b.fractionTemplate.numerator) && (a.fractionTemplate.denominator == b.fractionTemplate.denominator);   
} 
bool operator!=(const Fraction_c& a, const Fraction_c& b)
{
    return !(a == b);
} 
bool operator>(const Fraction_c& a, const Fraction_c& b)
{
    // As the numerator of the fraction increases the value of entire fraction increases(direct relation)
    // As the denominator of the fraction increases the value of the entire fraction decreases(inverse relation) 
    if(a.fractionTemplate.denominator == b.fractionTemplate.denominator && a.fractionTemplate.numerator != b.fractionTemplate.numerator)
    return a.fractionTemplate.numerator > b.fractionTemplate.numerator;
    else if(a.fractionTemplate.numerator == b.fractionTemplate.numerator && a.fractionTemplate.denominator != b.fractionTemplate.denominator)
    {
        if(a.fractionTemplate.denominator > b.fractionTemplate.denominator)
        return false;
        else
        return true;
    }
    else
    {
        if(a.fractionTemplate.numerator * b.fractionTemplate.denominator > b.fractionTemplate.numerator * a.fractionTemplate.denominator)
        return true;
        else
        return false;
    }
}
bool operator<(const Fraction_c& a, const Fraction_c& b)
{
    return b > a;
}
bool operator>=(const Fraction_c& a, const Fraction_c& b)
{
    if(a.fractionTemplate.denominator == b.fractionTemplate.denominator && a.fractionTemplate.numerator != b.fractionTemplate.numerator)
    return a.fractionTemplate.numerator >= b.fractionTemplate.numerator;
    else if(a.fractionTemplate.numerator == b.fractionTemplate.numerator && a.fractionTemplate.denominator != b.fractionTemplate.denominator)
    {
        if(a.fractionTemplate.denominator >= b.fractionTemplate.denominator)
        return false;
        else
        return true;
    }
    else
    {
        if(a.fractionTemplate.numerator * b.fractionTemplate.denominator >= b.fractionTemplate.numerator * a.fractionTemplate.denominator)
        return true;
        else
        return false;
    }
} 
bool operator<=(const Fraction_c& a, const Fraction_c& b)
{
    return b >= a; 
}  
std::ostream& operator<<(std::ostream& c_out, const Fraction_c& b)
{
    if(b.fractionTemplate.numerator == 0 && (b.fractionTemplate.denominator > 0 || b.fractionTemplate.denominator < 0))
    c_out << 0;
    else if(b.fractionTemplate.denominator == 0 && (b.fractionTemplate.numerator > 0 || b.fractionTemplate.numerator < 0))
    c_out << "complexInfinity";
    else if(b.fractionTemplate.numerator == 0 && b.fractionTemplate.denominator == 0)
    c_out << "Indeterminate";
    else if(b.fractionTemplate.denominator == 1 && (b.fractionTemplate.numerator > 0 || b.fractionTemplate.numerator < 0))
    c_out << b.fractionTemplate.numerator;
    else if(b.fractionTemplate.numerator == b.fractionTemplate.denominator)
    c_out << 1;
    else
    c_out << b.fractionTemplate.numerator << '/' << b.fractionTemplate.denominator;
    return c_out;
}
std::istream& operator>>(std::istream& c_in, Fraction_c& b)
{
    char slash{};
    c_in >> b.fractionTemplate.numerator >> slash >> b.fractionTemplate.denominator;
    return c_in;
}

