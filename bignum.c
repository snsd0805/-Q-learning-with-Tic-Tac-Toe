#include <stdio.h>
#include <stdbool.h>
#include "bignum.h"
#include "constant.h"

struct BigNum long_to_BigNum(long long num) {
    struct BigNum ans;
    int temp;
    for (int i=BIGNUM_LEN-1; i>=0; i--) {
        temp = num % 10;
        num /= 10;
        ans.num[i] = (char)(temp + 48);
    }
    return ans;
}

struct BigNum add(struct BigNum a, struct BigNum b) {
    struct BigNum ans;
    short s, carry=0;

    for (short i=BIGNUM_LEN-1; i>=0; i--) {
        s = (a.num[i]-48) + (b.num[i]-48) + carry;
        carry = s / 10;
        s %= 10;
        ans.num[i] = (char)(s+48);
    }
    return ans;
}

struct BigNum mul(struct BigNum a, int b) {
    struct BigNum ans;
    short s, carry=0;

    for (short i=BIGNUM_LEN-1; i>=0; i--) {
        s = (a.num[i]-48) * b + carry;
        carry = s / 10;
        s %= 10;
        ans.num[i] = (char)(s+48);
        // printf("index(%hd): %c\n", i, (char)(s+48));
    }

    return ans;
}
