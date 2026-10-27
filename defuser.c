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

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    return 1;
}
