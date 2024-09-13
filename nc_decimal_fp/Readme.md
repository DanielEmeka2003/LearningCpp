# nc's decimal floating-point implementation
Due to the inherent limitations of storing decimal real-numbers in the IEEE-754 binary format, i decided that it was apt to come up with my
very own implementation of storing real-numbers on the computer for my language. Instead of converting to binary, where rounding and
conversion errors creep, and storing it, i store the real-number as decimal, our most natural base.

[NOTE]: the limitations of IEEE binary format is ommited for now, but experienced developers would understand right way how annoying it can be.
It would be added in a later revision.

### Quick explanation of normalization of scientific notation - prerequiste
[NOTE]: because nc is base agnostic, definitions such as (e/E) for the exponent indicator aren't exclusive to decimal real-numbers but
inclusive to all valid nc bases .i.e (2-36).

Scientific notation is a way of re-arranging really big or small numbers in way that is easier to work with and manipulate.
General format of scientific notation:
+/-mantissa(e/E)+/-exponent
- where the mantissa is the real-number in any base which contains both the [integer] and [real] parts
- the (e/E) is the exponent indicator and is also interpreted as : × base^exponent
- and the exponent is the base(10) number that represents the orginal position of the radix seperator

example using base(10) counting system:
{ 0.0000000000000000000000000000000000000000000000000000000000000000000000000000000008 } becomes { 8.0e-82 }
or
{ 9999999999999999999999.67 } becomes { 9.99999999999999999999967e+21 }

Now as you have seen, they help make really big and really small numbers easy to reason about and work with.
But there's a problem with how to arrange these numbers, the example above is devoid of this, so let's look at another, still using base(10)
counting system:
the real-number 150 can be arrange in many ways, { 1.5e+2 }, { 0.15e+3 }, { 150.0e+0 }, { 15.0e+1 }, e.t.c
The above example shows how quickly of a problem arranging these numbers can be because there are so many ways and all lead back to the same
number. To prevent this, a standard call normalization of scientific notation was devised to define how numbers in scientific notation
are normally meant to be arranged.

The standard states that when arranging real-numbers in the scientific notation form, we ought to choose an exponent that results in the absolute
value of the mantissa being > 1 and < the base of the real-number

in nc, the standard is revised to mean that the when arranging, you must pick an integer for the integer_part that is > 1 and
< the base of the real-number. Which can also be futher broken down to mean that the integer_part must contain the initial counting
digits of the base expect zero. i.e. for base(10){1 - 9}, base(2){1}, base(16){1-9, A-Z}, base(8){1-7}, base(29){1-9 A-S} and so on

Now armed with this knowledge, let's arrange base(10) real-number
150.0 becomes 1.5e+2
or
0.008 becomes 8.0e-3 and so on

What of zero?
─────────────
Wait, we forgot about a number, the only nothing digit we have, zero(0/0.0). With what was stated above, zero(0/0.0) cannot never be normalized
because zero(0/0.0) isn't allowed to occupy the integer_part part of the mantissa/real-number. Due to this an exception is made for zero(0/0.0),
which is that zero(0/0.0) is allowed to be the only denormalized(not normalized) real-number arranged in scientific notation. So, that means
whenever you come across real-numbers that are in normalized scientific notation, and you see the infamous zero(0/0.0) as the integer_part,
regardless of it's exponent, you are to assume that the real-number is zero(0/0.0).

What exactly are the advantages of normalization?
─────────────────────────────────────────────────
Remember earlier when i said arranging real-numbers in scientific notation helps to better manipulate it, it is true for cases such as
multiplication, division and comparsion of said arranged numbers,
example - still assuming the base to be 10:
3.4e-10 × 2.478e+20, instead of performing a large multiplication .i.e 0.00000000034 × 247800000000000000000, which is time consuming,
a multiplication of both the mantissa/real-number and exponent seperately would result in the same result but with less computation
3.4 × 2.478 = 8.4252
e-10 × e+20 = (10^-10 × 10^20) = (10^-10 + 20) = 10^10
final result is 8.4252e+10, the same opposite would be used for division -instead of multiplication, use division, and instead of addition of
the exponent, subtract them.
[NOTE]: the above use of multiplication and division is an already mathematical rule, i think called the rule of indices, and by factorizing
the problem above, we were able to use the rule for the exponents .i.e 3.4e-10 × 2.478e+20 = (3.4 × 2.478) × (10^-10 × 10^20)

nc's use of scientific notation
───────────────────────────────
nc makes substantial use of normalized real-numbers in scientific notation, it is used to implement the real-number comparsion
functions such as real_eq, real_lt, and so on. There is even a function that specializes in normalization and denormalization of real-numbers
(all functions can be found in either file misc_utility.cpp in the nc_misc namespace or pseudomain.cpp in the global namespace).
But with a little twist, because the language is base agnostic, use of e/E as the exponent indicator is non-viable because the initial counting
digts of bases(15-36) all make use of letter e to represent number 14, so a compromise is introduced, instead of making use of e/E as the
exponent indicator, symbol @ is used instead.
so in nc, this { 1.0e-34 } is { 1.0@-34 }, with the option of the base being in-between the real-number and the exponent indicator .i.e
{ 1.0₃@-34 } or { 1.eee₃₃@-56 }

The various storage format
──────────────────────────
(1). using 32 bits for storage
    [ s:1 ][ integer_part:4 ][ real_part:20 ][ exponent:7  ]

    * offers a precision of 6 digits i.e. [0 - 999'999]
    * offers an exponent range of [-64 to 63]

(2). using 64 bits for storage
    [ s:1 ][ integer_part:4 ][ real_part:50 ][ exponent:9  ]

    * offers a precision of 15 digits i.e. [0 - 999'999'999'999'999]
    * offers an exponent range of [-256 to 255]

(3). using 128 bits for storage
    [ s:1 ][ integer_part:4 ][ real_part:113 ][ exponent:10  ]

    * offers a precision of 34 digits i.e. [0 - 9'999'999'999'999'999'999'999'999'999'999'999]
    * offers an exponent range of [-512 to 511]

(4). using 256 bits for storage
    [ s:1 ][ integer_part:4 ][ real_part:240 ][ exponent:11  ]

    * offers a precision of 72 digits i.e. [0 - 999_999_999_999_999_999_999_999_999_999_999_999_999_999_999_999_999_999_999_999_999_999_999_999]
    * offers an exponent range of [-1024 to 1023]

(5). using 512 bits for storage
    [ s:1 ][ integer_part:4 ][ real_part:492 ][ exponent:15  ]

    * offers a precision of 148 digits i.e. [0 - 9'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999'999]
    * offers an exponent range of [-16384 to 16383]

──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

A few things to note from the above:
* The format is divided into four segements comprising of the [ sign ][ integer_part ][ real_part ][ exponent ]
* The constants among the segements are the [ sign ] and the [ integer_part ]
* The [ real_part ] is responsible for the precision of that particular storage format
* The [ exponent ] is responsible for the range of the that particular storage format
* The higher the [ real_part ] the higher the precision, while the higher the [ exponent ] the higher the range
* Precision means the number of real_part digits a particular storage format can handle before rounding occurs
* Range means how large and small the decimal real-number gets to be before overflow or underflow occurs
* For every storage format, the range is always going to be [ 1.0@[minimum exponent for that format] ] - [ 9.[precision number of that format]@[maximum exponent for that format] ]

Example of how it is stored
───────────────────────────
First let's break down how a binary32 IEEE-754 format is stored:
Before that let me show you the format of IEEE-754 binary format regardless of storage space
[ sign ][ exponent + 127 ][ mantissa - devoid of the integer_part ]

for 32bit, sign is 1bit, exponent is 8bit and mantissa is 23bits

345.05
(1) It is first converted to it's binary representation
    345₁₀ -> 101011001₂
    .05₁₀ -> .0000110011001100110011001100110011001100110011001100110011001100110011₂ ([NOTE]: .05 is a repeating real-number in binary, so this is an approximation to the nearest 70th digit)
    final result = 101011001.0000110011001100110011001100110011001100110011001100110011001100110011₂

    [NOTE]: if you wish to test it out yourself, file pseudomain.cpp and misc_utility.cpp bot contain function convertRealBase10ToBaseN, the latter
    being in namespace nc_misc, but there's a catch, the returned result is normalized for that base, meaning the result above was actually gotten
    as 1.010110010000110011001100110011001100110011001100110011001100110011001100110011@+8, don't worry it's more of a blessing than you think
    because the next step relies of it being normalized
(2) Normalize the result
    final result = 1.010110010000110011001100110011001100110011001100110011001100110011001100110011@+8
(3) Place the following to their respective sections
    [ sign: + ][ exponent: 8 + 127 ][ mantissa: 010110010000110011001100110011001100110011001100110011001100110011001100110011 ]
    further binary condensing, mantissa is truncated because max storage for it is 23 as stated above
    [ 0 ][ 10000111 ][ 01011001000011001100110 ]

    * A sign of zero is positive while of one is negative
    * Exponent is stored as an offset of +127(for 32bit), because it allows the IEEE dosen't like two's complement for some reason
    * The mantissa is stored without it's integer_part because the IEEE took advantage of normalization as we explained before by assuming
        that the integer_part is always going to be one(because base(2) initial counting digits are {0 and 1} and for normalized real-numbers,
        {1} is only valid) for normalized numbers and therefore leaves an extra space for an additional real_part digit. Call it a handy base
        specific perk.

Now to show that this is how it is stored, using C++(which uses binary32 IEEE compliant format known as float), this simple example would
verify the above:
Fisrt, [ 0 ][ 10000111 ][ 01011001000011001100110 ] = 1135380070₁₀
```
int main()
{
    //with the help of highly unsafe void* casting
    float x = 345.05;
    void* v_ptr = &x;
    unsigned* i_ptr = (unsigned*)v_ptr; //perform a pointer cast directly from here
    io::cout.writews_nl("x as base(10): ", *i_ptr);

    /*
    * The following can also be achieved by using reinterpret_cast(unsafe), function std::bit_cast<To_type>(From_type) (safe, in the <utility>
    * header), or by using the implacement new operator to construct an object from a memory argument (widely unsafe)
    * 
    * And incase you are wondering about the io::cout object, it is my stream library that provides alternatives to the default C++ stream
    * objects .i.e things like: std::cout and the infamous operator<<
    */
}
```
Now let me show you how a sample real-number is stored using nc's decimal floatingPoint format:
345.05
(1) Normalize the real-number
    final result = 3.4505@+2
(2) Place the following to their respective sections
    [ sign: + ][ integer_part: 3 ][ real_part: 4505 ][ exponent: 2 ]
    futher binary condensing:
    [ 0 ][ 0011 ][ 0000001000110011001 ][ 0000010 ]
    As simple as that, nothing more nothing less

Special values
──────────────
From what you have read and seen so far, if you are astute enough to notice, you would have complianed of the extra space that both the
integer_part and real_part leave unattended(the real part more so).
Explanation:
The integer_part being a constant across storage formats has 4bits to itself, but it only makes use of values 1-9 from thoses bits i.e.
2^4 = 16 possible values + 0, list of possible values in a 4 digit binary
0000 = 0 ├ unused value
0001 = 1
0010 = 2
0011 = 3
0100 = 4
0101 = 5
0110 = 6
0111 = 7
1000 = 8
1001 = 9
1010 = 10 ┐
1011 = 11 │
1100 = 12 │
1101 = 13 ├ unused values
1110 = 14 │
1111 = 15 ┘

It only makes use of 1-9 values due to the real-number always being normalized, so the only possible base(10) values that can be there are
values 1-9.
The real_part also has this issue, but with a greater range not be used, example:
* 32bit storage would be
    (2^20) - 999'999 = 48'577 values not being used
* 64bit storage would be
    (2^50) - 999'999'999'999'999 = 125'899'906'842'625 values not being used
    and so on.

The reasons are quite explained here:

(1) (+/-)infinity: defined values of (+/-)infinity dictates that an [integer_part] bit section contains a normalized base(10) digit .i.e. 1-9,
    and a [real_part] bit section that contains values greater than the maximum for that storage format -for 32bit that is values > 999'999:

    why?, couldn't i have just used the maximum range for that storage format and instead defined infinity in relation to the unused integer_part
    values:

    I couldn't because if i were to utilize the maximum range of the [real_part] bit sections, that would result in a rather fustrating and
    unintuitive workaround to ensure overflow of the [real_part] bit section dosen't happen, meaning for example:
    For the 32bit storage, if i were to allow the max which is 1'048'575, the precision would be 7digits, but those 7digits would not allow
    numbers > 1'048'575, any attempt to store it would result in an errorenous sequence of bits for the entire storage, and in case you are
    thinking that why don't you just round the number when it maxes out, i.e for example: 1.1'048'576 would get rounded to 6digits instead thus
    becoming 1.1'048'58, well, that's where the unintuitive part comes in, having to check that every time is simply highly unintuitive and
    not something i would like to defend in the nearest future.
    Advantages of not utilizing the full range:
    (1) Having to store numbers from 0 to 999'999(for 32bit storage for example) is way more intuitive, those six digits are always going to
        cover the max range that is from 0 to 999'999 and any real_part greater can just be simply rounded up, example:
        1.9'999'999 when stored would be appropriately rounded to this 2.0
        It's just more sensible to reason about it from the base(10) pov.
    (2) The unused range isn't just forgotten, as you read initially, they are used to indicate underflow, if the exponent is negative, and
        overflow, if the exponent is positive for values greater than the maximum for that storage format.

(2) nan: meaning not a number, defined values of nan dictates that an [integer_part] bit contains the ununsed values expect zero and
    an optional [real_part] bit section, meaning the [real_part] bit section is not considered when NAN is detected, so the [real_part] can
    contain any value including ones > the maximum defined for that storage format

(3) (+/-)0: due to the real-number always being normalized for storage, zero can never logically be attained, so an exception is made, the same
    exception is made for zero when normalizing. Defined value of zero dictates that the [integer_part] bit section contains a value of zero and
    an optional [real_part] bit section.

To enable optimizations when performing pre-condition checks, we assign precedence to the following special values:
- (+/-)0 and nan
- (+/-)inf - lowest precedence

(+/-)0 is zero regardless

Incase you are wondering about why i didn't mention the [exponent] bit section in any of these, it is because it is entirely irrelevant in
deciding the special values, so just imagine that the [exponent] bit section is irrelevant for all defined special values.