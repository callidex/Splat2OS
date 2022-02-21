#include "mystdlib.h"
#include "utils.h"
#include "extern.h"

void trim(uint8_t* s) {
    short i = 0, k;
    while (s[i] != 0 && s[i] <= ' ') {
        i++;
    }
    k = 0;
    while (s[i] != 0) {
        s[k++] = s[i++];
    }
    do {
        s[k--] = 0;
    } while (k >= 0 && s[k] <= ' ');
}

uint8_t* skipSpaces(uint8_t* s) {
    while (isSpace(*s)) {
        s++;
    }
    return s;
}

uint8_t* skipDigits(uint8_t* s) {
    while (isDigit(*s)) {
        s++;
    }
    return s;
}

uint8_t charInStr(uint8_t c, uint8_t* s) {
    while (*s != 0) {
        if (*s == c) {
            return 1;
        }
        s++;
    }
    return 0;
}

uint8_t cmpNStrToStr(nstring* ns, uint8_t* s) {
    if (ns->len != strlen(s)) {
        return 0;
    }
    return memcmp(&(ns->text), s, ns->len) == 0;
}

void outputChar(uint8_t c) {
    sysPutc(c);
}

void outputStr(uint8_t* s) {
    while (*s) {
        sysPutc(*(s++));
    }
}

void outputNStr(nstring* t) {
    for (short i = 0; i < t->len; i++) {
        sysPutc(t->text[i]);
    }
};

void outputInt(long n) {
    long d = 1000000000;
    if (n < 0) {
        outputChar('-');
        n = -n;
    }
    if (n == 0) {
        sysPutc('0');
        return;
    }
    while (d > n) {
        d /= 10;
    }
    while (d > 0) {
        sysPutc(n / d + '0');
        n %= d;
        d /= 10;
    }
}

int decFromStr(uint8_t* s) {
    int8_t sign = 1;
    uint8_t base = 10;
    int res = 0;
    if (*s == '-') {
        sign = -1;
        s += 1;
    }
    if (*s == '0') {
        s += 1;
        if (toUpper(*s) == 'X') {
            base = 16;
            s += 1;
        } else if (toUpper(*s) == 'B') {
            base = 2;
            s += 1;
        } else {
            base = 8;
        }
    }
    while (isDigitBased(*s, base)) {
        res = res * base + makeDigit(*s++, base);
    }
    return res * sign;
}

void outputCr() {
    sysPutc('\n');
}

uint8_t toUpper(uint8_t c) {
    return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
}

uint8_t isAlpha(uint8_t c) {
    c = toUpper(c);
    return c >= 'A' && c <= 'Z';
}

uint8_t isDigit(uint8_t c) {
    return c >= '0' && c <= '9';
}

uint8_t isDigitBased(uint8_t c, uint8_t base) {
    if (c < '0') {
        return 0;
    } else if (c <= '9') {
        return base > c - '0';
    } else {
        c = toUpper(c);
        return c >= 'A' && base > c - 'A' + 10;
    }
}

uint8_t makeDigit(uint8_t c, uint8_t base) {
    if (c < '0') {
        return 0;
    } else if (c <= '9') {
        return c - '0';
    } else {
        return toUpper(c) - 'A' + 10;
    }
}

uint8_t isAlNum(uint8_t c) {
    return isDigit(c) || isAlpha(c);
}

uint8_t isSpace(uint8_t c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\v':
        case '\n':
        case '\r':
        case '\f':
            return 1;
    }
    return 0;
}

numeric hashOfNStr(nstring* t) {
    uint8_t i = 0, n = t->len;
    numeric res = 0;
    while (i < n) {
        res = (res << 1) ^ t->text[i];
        i++;
    }
    return res;
}

void pause(numeric millis) {
    numeric t0 = sysMillis(1);
    while (sysMillis(1) - t0 < millis) {
    }
}

