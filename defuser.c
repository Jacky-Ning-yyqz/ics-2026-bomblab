#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t hi  = (uint32_t)(challenge >> 32);
    uint32_t lo  = (uint32_t)challenge;
    uint32_t mid = (uint32_t)(challenge >> 16);

    uint32_t a = (lo + UINT32_C(0xfe5e)) ^ hi;
    uint32_t b = (mid ^ UINT32_C(0xa4af)) + lo * 2u;
    uint32_t r = (b - a) * 3u + UINT32_C(0xd180);

    values[0] = r;
    count = 1;

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t i;

    values[0] = 3u * (uint32_t)((challenge >> 36) & UINT64_C(0x3F));

    for (i = 1; i < 6u; ++i) {
        uint32_t t = (uint32_t)((challenge >> (i + 10u)) & UINT64_C(0xF));
        values[i] = (t + i) ^ (5u * values[i - 1u]);
    }

    count = 6;
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t sel = (uint32_t)(challenge & UINT64_C(7));
    uint32_t x   = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 3);
    uint32_t r;

    switch (sel) {
        case 0u:  r = (x ^ UINT32_C(0xba19)) + (x >> 2);      break;
        case 1u:  r = 5u * x + UINT32_C(0x42b8);              break;
        case 2u:  r = 5u * x + UINT32_C(0x3f601);             break;
        case 3u:  r = (2u * x) ^ UINT32_C(0xbc9);             break;
        case 4u:  r = 5u * x + UINT32_C(0x3728);              break;
        case 5u:  r = (x ^ UINT32_C(0xf30)) + (x >> 1);       break;
        case 6u:  r = (4u * x) ^ UINT32_C(0xb150);            break;
        default:  r = 5u * x + UINT32_C(0x79e2);              break;
    }

    values[0] = sel;
    values[1] = r;
    count = 2;

    return write_answer(out, cap, values, count);
}

static uint32_t p4_walk(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    uint32_t mid = b + ((c - b) >> 1);
    uint32_t key = mid + d;

    if (a == mid) {
        return (mid + UINT32_C(0xd6)) ^ d;
    }
    if (a < mid) {
        return (5u * p4_walk(a, b, mid - 1u, d)) ^ key ^ UINT32_C(0x13);
    }
    return (5u * p4_walk(a, mid + 1u, c, d)) ^ key ^ UINT32_C(0x09);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t a = (uint32_t)(challenge & UINT64_C(0x7F));
    uint32_t d = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);

    values[0] = p4_walk(a, 0u, UINT32_C(0x7F), d);
    count = 1;

    return write_answer(out, cap, values, count);
}

/* 第 5 关链表节点数据（从炸弹 .data 段读出，按链表物理顺序） */
static const uint32_t p5_a[12] = {
    UINT32_C(0x0b15), UINT32_C(0x0638), UINT32_C(0x090a), UINT32_C(0x0a7b),
    UINT32_C(0x005b), UINT32_C(0x043e), UINT32_C(0x0853), UINT32_C(0x0206),
    UINT32_C(0x051c), UINT32_C(0x0159), UINT32_C(0x0334), UINT32_C(0x0734)
};
static const uint32_t p5_b[12] = {
    UINT32_C(0xe0a0), UINT32_C(0x92ad), UINT32_C(0x2b91), UINT32_C(0xad7d),
    UINT32_C(0xa0eb), UINT32_C(0x6a61), UINT32_C(0x63f5), UINT32_C(0x9a5d),
    UINT32_C(0xca9a), UINT32_C(0xafe7), UINT32_C(0xc1f5), UINT32_C(0xe511)
};

static uint32_t p5_traverse(uint64_t challenge, uint32_t *aux) {
    uint32_t idx = (uint32_t)(challenge & UINT64_C(7));
    uint32_t n   = (uint32_t)((challenge >> 3) & UINT64_C(3));
    uint32_t v   = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    uint32_t i;

    v ^= UINT32_C(0xae9a);
    v = 3u * v + p5_b[idx];
    v ^= p5_a[idx];
    *aux = p5_a[idx];

    for (i = 0u; i < n; ++i) {
        idx++;
        v = 3u * v + p5_b[idx];
        v ^= p5_a[idx];
        *aux = p5_a[idx];
    }
    return v;
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t aux = 0u;

    values[0] = p5_traverse(challenge, &aux);
    values[1] = aux;
    count = 2;

    return write_answer(out, cap, values, count);
}

static uint32_t p6_op(uint32_t idx, uint32_t x) {
    switch (idx) {
        case 0u:
            return ((x >> 7) ^ x) + UINT32_C(0x5192);
        case 1u:
        case 3u:
            return ((x << 7) | (x >> 25)) ^ UINT32_C(0x4801);   /* rol x,7 */
        case 2u:
            return (4u * x + UINT32_C(0xb61)) ^ (5u * x);
        case 4u:
            return (x ^ UINT32_C(0x1117)) + (x << 7);
        case 5u:
        case 6u:
            return (x >> 2) | (x << 30);                        /* ror x,2 */
        default:
            return (x + UINT32_C(0x6e41)) ^ (x >> 7);
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t i1  = (uint32_t)(challenge & UINT64_C(7));
    uint32_t i2  = (uint32_t)((challenge >> 3) & UINT64_C(7));
    uint32_t arg = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);

    uint32_t r1 = p6_op(i1, arg);
    uint32_t r2 = p6_op(i2, r1 ^ UINT32_C(0x5341));

    values[0] = r2 + UINT32_C(0xbe24);
    count = 1;

    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap)
{
    uint64_t A = challenge;            /* secret_gate 的 rsi */
    uint64_t B = context;              /* secret_gate 的 rdx */
    uint32_t c, amix, bmix, k, x, m, v1, v2, v3;
    size_t n = 0u, i;
    int w;

    c    = (uint32_t)B & UINT32_C(3);
    amix = (uint32_t)(A >> 32) ^ (uint32_t)A;
    bmix = (uint32_t)(B >> 32) ^ (uint32_t)B;
    k    = (c * UINT32_C(0xb5c9)) ^ (uint32_t)(B >> 16);

    x = amix ^ bmix;
    m = (((x << 5) | (x >> 27)) + UINT32_C(0xb5c9)) ^ UINT32_C(0x9b19);

    v1 = c;
    v2 = m ^ k;
    v3 = v2 ^ (bmix + c + UINT32_C(0x9b19) + (amix >> 5));

    if (cap < 96u) return 1;

    for (i = 0u; i < 32u; ++i)         /* 32 字节伪装 */
        out[n++] = 'A';
    out[n++] = 'O';                    /* 偏移 32：OPEN */
    out[n++] = 'P';
    out[n++] = 'E';
    out[n++] = 'N';

    w = snprintf(out + n, cap - n, "%u %u %u", v1, v2, v3);
    if (w < 0 || (size_t)w >= cap - n) return 1;

    return 0;
}
