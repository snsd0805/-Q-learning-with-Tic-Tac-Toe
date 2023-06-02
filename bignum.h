#include "constant.h"
struct BigNum {
    char num[BIGNUM_LEN];
};
struct BigNum long_to_BigNum(long long num);
struct BigNum add(struct BigNum a, struct BigNum b);
struct BigNum mul(struct BigNum a, int b);