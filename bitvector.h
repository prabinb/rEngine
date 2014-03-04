#ifndef _BITVECTOR__H_
#define _BITVECTOR__H_

#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BV_INITIALIZER { NULL, 0 }
#define BV_NUM_BITS_ALIGN 6 /* 64 bits */
#define BV_NUM_BITS (1 << BV_NUM_BITS_ALIGN)
#define BV_NUM_BYTES (BV_NUM_BITS >> 3)
#define BV_MASK (BV_NUM_BITS - 1)
#define BV_ROUNDUP_BYTES(x) (((x + BV_MASK) & ~(BV_MASK)) >> 3)
#define _BV_BIT_OP(i,op,op2) data[i >> BV_NUM_BITS_ALIGN] op (op2(1ULL << ((i) & (BV_NUM_BITS - 1))))
#define _BV_COMBINE(bv,bv_other,op)                                \
    if (bv->size != bv_other->size)                                     \
        return -1;                                                      \
    uint64_t *data = bv->data, *data_end = data + bv->size;             \
    uint64_t *data_other = bv_other->data;                              \
    for (; data != data_end; ++data, ++data_other) {                    \
        *data op *data_other;                                           \
    }                                                                   \

struct bitvect {
    void *data;
    size_t size;
};

static inline int bv_init(struct bitvect *bv, size_t size) {
    bv->data = malloc(BV_ROUNDUP_BYTES(size));
    memset(bv->data,0,BV_ROUNDUP_BYTES(size));
    bv->size = BV_ROUNDUP_BYTES(size) >> (BV_NUM_BITS_ALIGN - 3);
    return 0;
}

static inline int bv_init_mem(struct bitvect *bv, size_t size, void *mem) {
    bv->data = mem;
    bv->size = BV_ROUNDUP_BYTES(size) >> (BV_NUM_BITS_ALIGN - 3);
    return 0;
}

static inline size_t bv_calc_mem_size(size_t size) {
    return BV_ROUNDUP_BYTES(size);
}

static inline int bv_set(struct bitvect *bv, size_t i) {
    uint64_t *data = bv->data;
    _BV_BIT_OP(i,|=,);
    return 0;
}

static inline int bv_reset(struct bitvect *bv, size_t i) {
    uint64_t *data = bv->data;
    _BV_BIT_OP(i,&=,~);
    return 0;
}

static inline int bv_isset(struct bitvect *bv, size_t i) {
    uint64_t *data = bv->data;
    return _BV_BIT_OP(i,&,) ? 1 : 0;
}

static inline int bv_from_index(struct bitvect *bv, uint32_t *index, size_t size) {
    if (0 == size) return -1;
    uint64_t *data = bv->data;
    uint32_t *idx = index, *idx_end = index + size;
    size_t current = *idx >> BV_NUM_BITS_ALIGN;
    size_t a = 0;
    for (;;) {
        a |= 1ULL << (*idx & (BV_NUM_BITS - 1));
        ++idx;
        if (idx == idx_end) break;
        size_t loc = *idx >> BV_NUM_BITS_ALIGN;
        if (current != loc) {
            data[current] |= a;
            a = 0;
            current = loc;
        }
    }
    data[current] |= a;
    return 0;
}

static inline int bv_combine_and(struct bitvect *bv, struct bitvect *bv_other) {
    _BV_COMBINE(bv,bv_other,&=);
    return 0;
}

static inline int bv_combine_or(struct bitvect *bv, struct bitvect *bv_other) {
    _BV_COMBINE(bv,bv_other,|=);
    return 0;
}

static inline int bv_dump(struct bitvect *bv) {
    uint64_t *data = bv->data, *data_end = data + bv->size;
    size_t count = 0;
    for (; data != data_end; ++data, count += BV_NUM_BITS) {
        uint64_t v = *data;
        uint8_t bit = 0;
        while (v) {
            if (v & 0x1)
                printf("%zu\n", count + bit);
            v >>= 1;
            ++bit;
        }
    }
    return 0;
}


#endif // _BITVECTOR__H_


