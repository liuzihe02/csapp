/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y)
{
  /*
  note that  x XOR y = (~x & y) | (x & ~y) using kmaps
  then using demorgans law, this is equivalent to ~(~(~x & y) & ~(x & ~y));
  */
  return ~(~(~x & y) & ~(x & ~y));
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void)
{
  // this smallest value is 10000... etc all the way, for 32 bit integers
  return 1 << 31;
}

// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x)
{
  /* Some key properties
  - the largest 2s complement integer is 0111....
  - when you add 1 to Tmax, it carries over to Tmin 1000...
  - Tmin ^ Tmax is 1111... which has a value of -1
  - If we do !(111...) we will get 000...
  - Then we can just use the operators ! to check if this is indeed 0000...

  - AND also need to handle edge case of x being -1, 1111...
  - need to do !!(~x) operator to check if x is indeed -1
  */

  /*
  For Tmax: (1000...0000) ^ (0111...1111) = 1111...1111
 This gives all 1s only for x=Tmax and x=-1
  */
  int step1 = (x + 1) ^ x;
  /*
  Takes the complement of all 1s, giving us 0000...0000
 For non-Tmax numbers, this will be some non-zero value
  */
  int step2 = ~step1;
  /*
  Returns 1 if the previous step gave us all zeros
 Returns 0 otherwise
  */
  int mainCheck = !step2;
  /*
  For -1 (1111...1111), ~x is 0000...0000, so !!() gives 0
 For all other numbers, ~x is non-zero, so !!() gives 1
 This eliminates the -1 case
  */
  int negOneCheck = !!(~x);
  /*
  Finally we AND these 2 together, since both conditions must be true
  */
  return mainCheck & negOneCheck;
}

/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x)
{
  /*
  note that not XOR is basically the COMPARISON operator bitwise!
  */
  // first create a 32 bit mask
  int mask = 0xAAAAAAAA;
  // isolate the odd bits positions
  int oddBits = x & mask;
  // if these 2 are equal, check should return all zeros
  int check = oddBits ^ mask;
  // return 1 or 0 appropriately
  int res = !check;
  return res;
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x)
{
  /*
  note that `x + (-x) = 0`
  Since
  x + ((~x) + 1)
  = x + ~x + 1
  = 1111...1 + 1
  = 0000...0  (carrying over through all positions)

  therefore (~x) +1 works
  */
  return ~x + 1;
}
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x)
{
  /*
  0x30 is 0011 0000
  0x30 is 0011 1001

  basically, first check if -8 to -5 bit are 0011
  then check whether it falls from 0 to 9
    if -4 bit is 0 then pass
    if -4 bit is 1 and -2,-3 bits are 0 then pass
    else fail
  */
  int middleMask = 0x3;
  // threeCheck is 1 if front and middle are 0000 ..... 0011 xxxx
  int middleCheck = !((x >> 4) ^ middleMask);

  // isolate the last 4 bits
  int lastFour = x & 0xF;
  // 1 if the -4th bit is 0
  int fourCheck = !(lastFour >> 3);
  // 1 if the last 4 bit pattern is 100x
  int twoThreeCheck = !((lastFour >> 1) ^ 0x4);

  return middleCheck & (fourCheck | twoThreeCheck);
}

/*
 * conditional - same as x ? y : z

condition is for whether x is non-zero or zero

 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z)
{
  // 1 if x is non-zero
  int conBit = !!x;
  // when conBit is 1, mask is 111....
  // when conBit is 0, mask is 000....
  // note that ~0 is the same as the value of -1 (111....11)
  int mask = ~(~0 + conBit);
  // return the value
  return (mask & y) | (~mask & z);
}

/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y)
{
  /*
  first check for differing signs and handle those cases
  then assuming same signs, handle y-x is positive or negative
  */

  // Will be -1 (1111...) if negative
  // 0 if positive (000...00)
  // rmb arithmetic shift!
  int sign_x = x >> 31;
  int sign_y = y >> 31;
  // 1 if signs are opposite
  int signs_opp = !(~(sign_x ^ sign_y));
  // 1 if y is positive
  int is_y_pos = !(sign_y);

  // the following assume same sign
  // we use y+(~x+1) to compute y-x
  // we use the negate function basically
  int difference = y + (~x + 1);
  // 1 if (y-x) is positive
  int sign_difference = !(difference >> 31);

  return (signs_opp & is_y_pos) | (!signs_opp & sign_difference);
}

// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x)
{
  /*
Key idea: For any non-zero number x:
- Either x or -x will have a 1 in the sign bit (MSB)
- For zero, both x and -x will have 0 in the sign bit
*/

  // Get negative of x using two's complement
  int neg_x = ~x + 1;

  // Take sign bits of both x and -x using right shift
  // For non-zero numbers, at least one of these will be 1
  // For zero, both will be 0
  int sign_bits = (x >> 31) | (neg_x >> 31);

  // Add 1 to the sign_bits and mask with 1
  // If x was 0: sign_bits = 0, result = 1
  // If x was non-zero: sign_bits = -1, result = 0
  return (sign_bits + 1) & 1;
}

/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x)
{
  /*
  key insight is that for positive numbers, we need to find the leftmost 1
  for negative numbers, need to find the leftmost 0 - the left 1 to this zero is the sign bit

  note that for negative numbers, we have sign extension, where its all 1 leftwards from the sign bit

  we use a binary search strategy for this. NOTE: VERY DIFFICULT QUESTION
  */
  int b16, b8, b4, b2, b1, b0;
  int sign;

  // Get sign of x
  // if x positive, will be 0
  // if x negative will be -1
  sign = x >> 31;

  // If x is negative, flip all bits to find most significant 0
  // If x is positive, leave as is to find most significant 1
  x = (sign & ~x) | (~sign & x);

  // Binary search to find position of most significant 1
  // First check if high 16 bits contain a 1
  // >>16 gives us the high 16 bits
  // !!(..) turns any non-zero value to 1 and keeps 0 as 0
  // <<4 then shifts this 0 or 1 leftwards by 4 positions
  /// so b16 will either be 16(0b10000) if a 1 exists, or 0 if the high bits were all 0
  b16 = (!!(x >> 16)) << 4;
  // Shift x right by 16 if a 1 exists in the high bits (check this top half)
  // otherwise dont shift at all (check the bottom half)
  x = x >> b16;

  // Check if high 8 bits of remaining contain a 1
  b8 = !!(x >> 8) << 3;
  x = x >> b8;

  // Check if high 4 bits of remaining contain a 1
  b4 = !!(x >> 4) << 2;
  x = x >> b4;

  // Check if high 2 bits of remaining contain a 1
  b2 = !!(x >> 2) << 1;
  x = x >> b2;

  // Check if high 1 bit of remaining contains a 1
  b1 = !!(x >> 1);
  x = x >> b1;

  // Check if last bit is 1
  b0 = x;

  // Add 1 for sign bit
  // this gives the location of the most significant 1 or 0
  return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}

// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf)
{
  // Extract components of float
  // the & here gets rid of unneccessary stuff
  unsigned sign = (uf >> 31) & 1;   // Get sign bit (1 bit)
  unsigned exp = (uf >> 23) & 0xFF; // Get exponent (8 bits)
  unsigned frac = uf & 0x7FFFFF;    // Get fraction (23 bits)

  // Special case 1: If exp is all 1's (NaN or infinity), return unchanged
  if (exp == 0xFF)
  {
    return uf;
  }

  // Special case 2: If number is denormalized (exp = 0)
  if (exp == 0)
  {
    // Simply shift fraction left by 1 (effectively multiplying by 2)
    frac = frac << 1;
  }
  // Normal case: number is normalized
  else
  {
    exp = exp + 1; // Add 1 to exponent (equivalent to multiplying by 2)
  }

  // Reconstruct the float
  return (sign << 31) | (exp << 23) | frac;
}

/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *
 *   basically need truncate the decimal parts
 *
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf)
{
  // Extract components of float
  unsigned sign = (uf >> 31) & 1;   // Get sign bit (1 bit)
  unsigned exp = (uf >> 23) & 0xFF; // Get exponent (8 bits)
  unsigned frac = uf & 0x7FFFFF;    // Get fraction (23 bits)

  // Add implied 1 for normalized numbers, ie b 1000 00... 00
  unsigned M = frac | 0x800000; // M is 1.fraction in binary

  // Calculate E = exp - bias (bias is 127)
  int E = exp - 127; // True exponent

  // final result
  int result;

  // Special cases:
  // 1. If exp is all 1's (NaN/infinity) or number is too large
  if (exp == 0xFF || E > 31)
  {
    return 0x80000000u;
  }

  // 2. If E < 0, float is less than 1, truncates to 0
  if (E < 0)
  {
    return 0;
  }

  // 3. If denormalized (exp = 0), result is 0
  if (exp == 0)
  {
    return 0;
  }

  // Normal case: shift M based on E
  // If E < 23, shift right to truncate decimal bits
  // If E > 23, shift left to add zeroes

  if (E < 23)
  {
    result = M >> (23 - E);
  }
  else
  {
    result = M << (E - 23);
  }

  // Apply sign
  return sign ? -result : result;
}

/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x)
{
  // Remember: float = (-1)^sign * 2^(exp-bias) * 1.frac
  // For 2^x, sign=0, frac=0, we just need to set exp correctly

  // Some key values:
  // Smallest normalized: 2^-126 (exp=1)
  // Smallest denorm: 2^-149 (exp=0, frac=1)
  // Largest normalized: 2^127 (exp=254)

  // Handle numbers too small to represent
  if (x < -149)
  {
    return 0; // Too small even for denormalized
  }

  // Handle numbers too large to represent
  if (x > 127)
  {
    return 0xFF << 23; // Return +infinity (all 1s in exp, 0s in frac)
  }

  // Handle denormalized range (-149 <= x <= -127)
  if (x <= -127)
  {
    // For denorms: exp=0, shift 1 into correct frac position
    // We need to shift 1 left by (149+x) positions
    // -149 -> shift by 0
    // -148 -> shift by 1
    // etc.
    return 1 << (149 + x);
  }

  // Normal range (-126 <= x <= 127)
  // exp = x + bias = x + 127
  return (x + 127) << 23;
}
