#include "bignum.h"
#include "constant.h"
#include <stdbool.h>
#include <stdio.h>

/*
    Transform long long to BigNum struct.

    Args:
        - long long num: from

    Results:
        - struct BigNum ans
*/
struct BigNum long_to_BigNum(long long num)
{
    struct BigNum ans;
    int temp;
    for (int i = BIGNUM_LEN - 1; i >= 0; i--) {
        temp = num % 10;
        num /= 10;
        ans.num[i] = (char)(temp + 48);
    }
    return ans;
}

/*
    Add 2 BigNum struct.

    Args:
        - struct BigNum a
        - struct BigNum b

    Results:
        - struct BigNum ans: ans is a + b
*/
struct BigNum add(struct BigNum a, struct BigNum b)
{
    struct BigNum ans;
    short s, carry = 0;

    for (short i = BIGNUM_LEN - 1; i >= 0; i--) {
        s = (a.num[i] - 48) + (b.num[i] - 48) + carry;
        carry = s / 10;
        s %= 10;
        ans.num[i] = (char)(s + 48);
    }
    return ans;
}

/*
    Multiply a by units digit integer b

    Args:
        - struct BigNum a
        - int b: b must be an integer representing the units digit

    Results:
        - struct BigNum ans: ans is a * b
*/
struct BigNum mul(struct BigNum a, int b)
{
    struct BigNum ans;
    short s, carry = 0;

    for (short i = BIGNUM_LEN - 1; i >= 0; i--) {
        s = (a.num[i] - 48) * b + carry;
        carry = s / 10;
        s %= 10;
        ans.num[i] = (char)(s + 48);
    }

    return ans;
}
