#ifndef _CTK_SPU_GLOBAL_PTR_TPP
#define _CTK_SPU_GLOBAL_PTR_TPP

extern "C" {
extern void *memcpy(void *, const void *, size_t size);
};

namespace ctk {

//------------------------------------------------------------------
// No Sync policy (we just need a declaration and this does nothing)
template <typename T, int SIZE = sizeof(T) >
struct NoSync { };

//------------------------------------------------------------------
// R/W Sync policy
template <typename T, int SIZE = sizeof(T), 
    bool smallerThan16 = (SIZE < 16), 
    bool smallerThan128 = (SIZE < 128), 
    bool smallerThan16K = (SIZE <= MFC_MAX_DMA_SIZE) >
struct RWSync { /* generic one; this would catch false case */ };

// specialized one (16K < sizeof(T))
template <typename T, int SIZE>
struct RWSync <T, SIZE, false, false, false> {
    static void read(ea_addr_t ea, char *buf, int *offset) {
        CTK_STATIC_ASSERT((SIZE % 16) == 0, Size_must_be_16B_aligned);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        *offset = ea - align_ea;
        ctk_dma_get_block(buf, align_ea, SIZE);
    }
    static void write(ea_addr_t ea, char *buf, volatile T *t) {
        CTK_STATIC_ASSERT((SIZE % 16) == 0, Size_must_be_16B_aligned);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        ::memcpy((void*)&buf[offset], (void*)t, SIZE);
        ctk_dma_putb((void*)buf, align_ea, SIZE, 0);
    }
};

// specialized one (128 <= sizeof(T) <= 16K)
template <typename T, int SIZE>
struct RWSync <T, SIZE, false, false, true> {
    static void read(ea_addr_t ea, char *buf, int *offset) {
        CTK_STATIC_ASSERT((SIZE % 16) == 0, Size_must_be_16B_aligned);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        *offset = ea - align_ea;
        ctk_dma_get_small_block((void*)buf, align_ea, SIZE);
    }
    static void write(ea_addr_t ea, char *buf, volatile T *t) {
        CTK_STATIC_ASSERT((SIZE % 16) == 0, Size_must_be_16B_aligned);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        ::memcpy((void*)&buf[offset], (void*)t, SIZE);
        ctk_dma_putb_small((void*)buf, align_ea, SIZE, 0);
    }
};

// specialized one (sizeof(T) < 128)
template <typename T, int SIZE>
struct RWSync <T, SIZE, false, true, true> {
    static void read(ea_addr_t ea, char *buf, int *offset) {
        CTK_STATIC_ASSERT(((SIZE) & (SIZE - 1)) == 0, Size_must_be_power_of_2);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        *offset = ea - align_ea;
        spu_mfcdma64((void*)buf, ea_hi32(align_ea), ea_lo32(align_ea),
                128, 0, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);
    }
    static void write(ea_addr_t ea, char *buf, volatile T *t) {
        CTK_STATIC_ASSERT(((SIZE) & (SIZE - 1)) == 0, Size_must_be_power_of_2);
        int tag = 0, status = 1;
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        ::memcpy((void*)&buf[offset], (void*)t, SIZE);
        spu_mfcdma64((void*)buf, ea_hi32(align_ea), ea_lo32(align_ea),
                128, tag, MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
        while (status) {
            spu_mfcdma64((void*)buf, ea_hi32(align_ea), ea_lo32(align_ea),
                    128, tag, MFC_GETLLAR_CMD);
            spu_readch(MFC_RdAtomicStat);
            ::memcpy((void*)&buf[offset], (void*)t, SIZE);
            spu_mfcdma64((void*)buf, ea_hi32(align_ea), ea_lo32(align_ea),
                    128, tag, MFC_PUTLLC_CMD);
            status = spu_readch(MFC_RdAtomicStat);
        }
    }
};

// specialized one (sizeof(T) < 16)
template <typename T, int SIZE>
struct RWSync <T, SIZE, true, true, true> {
    static void read(ea_addr_t ea, volatile T *t) {
        char *ls;
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        ctk_alloca_align_typed(ls, char *, 128, 128);
        spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea),
                128, 0, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);
        ::memcpy((void*)t, (void*)&ls[offset], SIZE);
    }
    static void write(ea_addr_t ea, volatile T *t) {
        int tag = 0, status = 1;
        char *ls;
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        ctk_alloca_align_typed(ls, char *, 128, 128);
        ::memcpy((void*)&ls[offset], (void*)t, SIZE);
        spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea),
                128, tag, MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
        while (status) {
            spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea),
                    128, tag, MFC_GETLLAR_CMD);
            spu_readch(MFC_RdAtomicStat);
            ::memcpy((void*)&ls[offset], (void*)t, SIZE);
            spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea),
                    128, tag, MFC_PUTLLC_CMD);
            status = spu_readch(MFC_RdAtomicStat);
        }
    }
};

#define DECLARE_RWSYNC_FOR_PRIMITIVE_TYPE(_type_, _align_)  \
    template <> \
    struct RWSync <_type_, sizeof(_type_)> {     \
        static void read(ea_addr_t ea, volatile _type_ *t) { \
            volatile char *ls; \
            ea_addr_t align_ea = ALIGN_DOWN(ea, _align_); \
            int offset = ea - align_ea; \
            ctk_alloca_align_typed(ls, char *, _align_, _align_); \
            ctk_dma_get_small_block((void*)ls, align_ea, 128); \
            *t = *(_type_*)&ls[offset]; \
        }  \
        static void write(ea_addr_t ea, volatile _type_ *t) { \
            int tag = 0, status = 1; \
            volatile char *ls; \
            ea_addr_t align_ea = ALIGN_DOWN(ea, _align_); \
            int offset = ea - align_ea; \
            ctk_alloca_align_typed(ls, char *, _align_, _align_); \
            do { \
                spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea), \
                        _align_, tag, MFC_GETLLAR_CMD); \
                spu_readch(MFC_RdAtomicStat); \
                *(_type_*)&ls[offset] = *t; \
                spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea), \
                        _align_, tag, MFC_PUTLLC_CMD); \
                status = spu_readch(MFC_RdAtomicStat); \
            } while (status); \
        }  \
    };


//------------------------------------------------------------------
// Write Only Sync policy
template <typename T, int SIZE = sizeof(T), 
    bool smallerThan16 = (SIZE < 16), 
    bool smallerThan128 = (SIZE < 128), 
    bool smallerThan16K = (SIZE <= MFC_MAX_DMA_SIZE)>
struct WSync { /* default one */ };

// specialized one (16K < sizeof(T))
template <typename T, int SIZE>
struct WSync <T, SIZE, false, false, false> {
    static void write(ea_addr_t ea, char *buf, T *t) {
        CTK_STATIC_ASSERT((SIZE % 16) == 0, Size_must_be_16B_aligned);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        ::memcpy((void*)&buf[offset], (void*)t, SIZE);
        ctk_dma_putb((void*)buf, align_ea, ALIGN_UP(SIZE, 128), 0);
    }
};

// specialized one (128 <= sizeof(T) <= 16K)
template <typename T, int SIZE>
struct WSync <T, SIZE, false, false, true> {
    static void write(ea_addr_t ea, char *buf, T *t) {
        CTK_STATIC_ASSERT((SIZE % 16) == 0, Size_must_be_16B_aligned);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        ::memcpy((void*)&buf[offset], (void*)t, SIZE);
        ctk_dma_putb_small((void*)buf, align_ea, ALIGN_UP(SIZE, 128), 0);
    }
};

// specialized one (sizeof(T) < 128)
template <typename T, int SIZE>
struct WSync <T, SIZE, false, true, true> {
    static void write(ea_addr_t ea, char *buf, T *t) {
        int tag = 0, status = 1;
        CTK_STATIC_ASSERT(((SIZE) & (SIZE - 1)) == 0, Size_must_be_power_of_2);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        do {
            spu_mfcdma64((void*)buf, ea_hi32(align_ea), ea_lo32(align_ea),
                    128, tag, MFC_GETLLAR_CMD);
            spu_readch(MFC_RdAtomicStat);
            ::memcpy((void*)&buf[offset], (void*)t, SIZE);
            spu_mfcdma64((void*)buf, ea_hi32(align_ea), ea_lo32(align_ea),
                    128, tag, MFC_PUTLLC_CMD);
            status = spu_readch(MFC_RdAtomicStat);
        } while (status);
    }
};

// specialized one (sizeof(T) < 16)
template <typename T, int SIZE>
struct WSync <T, SIZE, true, true, true> {
    static void write(ea_addr_t ea, T *t) {
        int tag = 0, status = 1;
        volatile char *ls;
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        ctk_alloca_align_typed(ls, char *, 128, 128);
        do {
            spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea),
                    128, tag, MFC_GETLLAR_CMD);
            spu_readch(MFC_RdAtomicStat);
            ::memcpy((void*)&ls[offset], (void*)t, SIZE);
            spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea),
                    128, tag, MFC_PUTLLC_CMD);
            status = spu_readch(MFC_RdAtomicStat);
        } while(status);
    }
};

#define DECLARE_WSYNC_FOR_PRIMITIVE_TYPE(_type_, _align_)  \
    template <> \
    struct WSync <_type_, sizeof(_type_)> {     \
        static void write(ea_addr_t ea, _type_ *t) { \
            int tag = 0, status = 1; \
            volatile char *ls; \
            ea_addr_t align_ea = ALIGN_DOWN(ea, _align_); \
            int offset = ea - align_ea; \
            ctk_alloca_align_typed(ls, char *, _align_, _align_); \
            do { \
                spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea), \
                        _align_, tag, MFC_GETLLAR_CMD); \
                spu_readch(MFC_RdAtomicStat); \
                *(_type_*)&ls[offset] = *t; \
                spu_mfcdma64((void*)ls, ea_hi32(align_ea), ea_lo32(align_ea), \
                        _align_, tag, MFC_PUTLLC_CMD); \
                status = spu_readch(MFC_RdAtomicStat); \
            } while (status); \
        }  \
    };


//------------------------------------------------------------------
// Read Only Sync policy
template <typename T, int SIZE = sizeof(T), 
    bool smallerThan16 = (SIZE < 16), 
    bool smallerThan128 = (SIZE < 128), 
    bool smallerThan16K = (SIZE <= MFC_MAX_DMA_SIZE) >
struct RSync { /* selector */ };

// specialized one (16K < sizeof(T))
template <typename T, int SIZE>
struct RSync <T, SIZE, false, false, false> {
    static void read(ea_addr_t ea, char *buf, int *offset) {
        CTK_STATIC_ASSERT((SIZE % 16) == 0, Size_must_be_16B_aligned);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        *offset = ea - align_ea;
        ctk_dma_get_block(buf, align_ea, SIZE);
    }
};

// specialized one (128 <= sizeof(T) <= 16K)
template <typename T, int SIZE>
struct RSync <T, SIZE, false, false, true> {
    static void read(ea_addr_t ea, char *buf, int *offset) {
        CTK_STATIC_ASSERT((SIZE % 16) == 0, Size_must_be_16B_aligned);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        *offset = ea - align_ea;
        ctk_dma_get_small_block((void*)buf, align_ea, SIZE);
    }
};

// specialized one (sizeof(T) < 128)
template <typename T, int SIZE>
struct RSync <T, SIZE, false, true, true> {
    static void read(ea_addr_t ea, char *buf, int *offset) {
        CTK_STATIC_ASSERT(((SIZE) & (SIZE - 1)) == 0, Size_must_be_power_of_2);
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        *offset = ea - align_ea;
        ctk_dma_get_small_block((void*)buf, align_ea, 128);
    }
};

// specialized one (sizeof(T) < 16)
template <typename T, int SIZE>
struct RSync <T, SIZE, true, true, true> {
    static void read(ea_addr_t ea, volatile T *t) {
        volatile char *ls;
        ea_addr_t align_ea = ALIGN_DOWN(ea, 128);
        int offset = ea - align_ea;
        ctk_alloca_align_typed(ls, char *, 128, 128);
        ctk_dma_get_small_block((void*)ls, align_ea, 128);
        ::memcpy((void*)t, (void*)&ls[offset], SIZE);
    }
};

#define DECLARE_RSYNC_FOR_PRIMITIVE_TYPE(_type_, _align_)  \
    template <> \
    struct RSync <_type_, sizeof(_type_)> {     \
        static void read(ea_addr_t ea, volatile _type_ *t) { \
            volatile char *ls; \
            ea_addr_t align_ea = ALIGN_DOWN(ea, _align_); \
            int offset = ea - align_ea; \
            ctk_alloca_align_typed(ls, char *, _align_, _align_); \
            ctk_dma_get_small_block((void*)ls, align_ea, _align_); \
            *t = *(_type_*)&ls[offset]; \
        }  \
    };


#define DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(_type_, _align_)  \
    DECLARE_RWSYNC_FOR_PRIMITIVE_TYPE(_type_, _align_) \
    DECLARE_WSYNC_FOR_PRIMITIVE_TYPE(_type_, _align_) \
    DECLARE_RSYNC_FOR_PRIMITIVE_TYPE(_type_, _align_)


DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(char, 128)
DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(short, 128)
DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(int, 128)
DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(long, 128)
DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(long long, 128)
DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(unsigned char, 128)
DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(unsigned short, 128)
DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(unsigned int, 128)
DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(unsigned long, 128)
DECLARE_SYNCS_FOR_PRIMITIVE_TYPE(unsigned long long, 128)

#undef DECLARE_RWSYNC_FOR_PRIMITIVE_TYPE
#undef DECLARE_WSYNC_FOR_PRIMITIVE_TYPE
#undef DECLARE_RSYNC_FOR_PRIMITIVE_TYPE
#undef DECLARE_SYNCS_FOR_PRIMITIVE_TYPE

// forward declaration
template <typename T, int SIZE, typename SYNC>
struct _GlobalPtrWrapper;

// internal structure
template <typename T, int SIZE, typename SYNC, 
    bool smallerThan16 = (SIZE < 16) >
struct _GlobalPtrValue { };

// internal structure - specialized one (for small data)
template <typename T, int SIZE, typename SYNC>
struct _GlobalPtrValue <T, SIZE, SYNC, true> {
    ea_addr_t m_ea;
    bool m_dirty;
    volatile T t;
    _GlobalPtrValue(ea_addr_t ea) : m_ea(ea), m_dirty(false) { }
    /* write */
    _GlobalPtrValue& operator=(T &t) {
        SYNC::write(m_ea, &t);
        m_dirty = false;
        return *this;
    }
    _GlobalPtrValue& operator=(const T &t) {
        SYNC::write(m_ea, (T*)&t);
        m_dirty = false;
        return *this;
    }
    /* read */
    operator T() {
        SYNC::read(m_ea, &t);
        return (T)t;
    }
    operator T*() {
        SYNC::read(m_ea, &t);
        m_dirty = true;
        return (T*)&t;
    }
    /* clone */
    _GlobalPtrWrapper<T, SIZE, SYNC> operator &() {
        _GlobalPtrWrapper<T, SIZE, SYNC> p;
        p.m_body.m_ea = m_ea;
        return p;
    }
    /* sync */
    void load() {
        SYNC::read(m_ea, (T*)&t);
    }
    void flush() {
        SYNC::write(m_ea, (T*)&t);
        m_dirty = false;
    }
    /* reset */
    void reset(ea_addr_t ea) {
        if (m_dirty) {
            flush();
        }
        m_ea = ea;
    }

    /* destructor */
    ~_GlobalPtrValue() {
        if (m_dirty) {
            flush();
        }
    }
};

// internal structure - specialized one (for small data, Write Only)
template <typename T, int SIZE>
struct _GlobalPtrValue <T, SIZE, WSync<T, SIZE>, true> {
    ea_addr_t m_ea;
    volatile T t;
    _GlobalPtrValue(ea_addr_t ea) : m_ea(ea) { }
    /* write */
    _GlobalPtrValue& operator=(T &t) {
        WSync<T, SIZE>::write(m_ea, &t);
        return *this;
    }
    _GlobalPtrValue& operator=(const T &t) {
        WSync<T, SIZE>::write(m_ea, (T*)&t);
        return *this;
    }
    /* clone */
    _GlobalPtrWrapper<T, SIZE, WSync<T, SIZE> > operator &() {
        _GlobalPtrWrapper<T, SIZE, WSync<T, SIZE> > p;
        p.m_body.m_ea = m_ea;
        return p;
    }
    /* sync */
    void flush() {
        WSync<T, SIZE>::write(m_ea, (T*)&t);
    }
    /* reset */
    void reset(ea_addr_t ea) {
        m_ea = ea;
    }
};

// internal structure - specialized one (for small data, Read Only)
template <typename T, int SIZE>
struct _GlobalPtrValue <T, SIZE, RSync<T, SIZE>, true> {
    ea_addr_t m_ea;
    volatile T t;
    _GlobalPtrValue(ea_addr_t ea) : m_ea(ea) { }
    /* read */
    operator T() {
        RSync<T, SIZE>::read(m_ea, &t);
        return (T)t;
    }
    operator T*() {
        RSync<T, SIZE>::read(m_ea, &t);
        return (T*)&t;
    }
    /* clone */
    _GlobalPtrWrapper<T, SIZE, RSync<T, SIZE> > operator &() {
        _GlobalPtrWrapper<T, SIZE, RSync<T, SIZE> > p;
        p.m_body.m_ea = m_ea;
        return p;
    }
    /* sync */
    void load() {
        RSync<T, SIZE>::read(m_ea, (T*)&t);
    }
    /* reset */
    void reset(ea_addr_t ea) {
        m_ea = ea;
    }
};

// internal structure - specialized one (for small data, No Implicit Sync)
template <typename T, int SIZE>
struct _GlobalPtrValue <T, SIZE, NoSync<T, SIZE>, true> {
    ea_addr_t m_ea;
    volatile T m_data;
    _GlobalPtrValue(ea_addr_t ea) : m_ea(ea) { }
    /* write */
    _GlobalPtrValue& operator=(T &t) {
        m_data = (volatile T)t;
        return *this;
    }
    _GlobalPtrValue& operator=(const T &t) {
        m_data = (volatile T)t;
        return *this;
    }
    /* read */
    operator T() {
        return (T)m_data;
    }
    operator T*() {
        return (T*)&m_data;
    }
    /* clone */
    _GlobalPtrWrapper<T, SIZE, NoSync<T, SIZE> > operator &() {
        _GlobalPtrWrapper<T, SIZE, NoSync<T, SIZE> > p;
        p.m_body.m_ea = m_ea;
        p.m_body.m_data = m_data;
        return p;
    }
    /* sync */
    void load() {
        RSync<T, SIZE>::read(m_ea, (T*)&m_data);
    }
    void flush() {
        WSync<T, SIZE>::write(m_ea, (T*)&m_data);
    }
    /* reset */
    void reset(ea_addr_t ea) {
        m_ea = ea;
    }
};

// internal structure - specialized one (for larger data)
template <typename T, int SIZE, typename SYNC>
struct _GlobalPtrValue <T, SIZE, SYNC, false> {
    ea_addr_t m_ea;
    bool m_dirty, m_loaded;
    char m_buffer[ALIGN_UP(SIZE, 128) + 128];
    char *m_buf;
    int m_offset;
    _GlobalPtrValue(ea_addr_t ea) : m_ea(ea), m_dirty(false),
        m_loaded(false), 
        m_buf((char*)ALIGN_UP_PTR(m_buffer, 128)) { }
    /* write */
    _GlobalPtrValue& operator=(T &t) {
        SYNC::write(m_ea, m_buf, &t);
        m_dirty = false;
        m_loaded = true;
        return *this;
    }
    _GlobalPtrValue& operator=(const T &t) {
        SYNC::write(m_ea, m_buf, (T*)&t);
        m_dirty = false;
        m_loaded = true;
        return *this;
    }
    /* read */
    operator T() {
        if (m_loaded == false) {
            SYNC::read(m_ea, m_buf, &m_offset);
            m_loaded = true;
        }
        return *(T*)&m_buf[m_offset];
    }
    operator T*() {
        if (m_loaded == false) {
            SYNC::read(m_ea, m_buf, &m_offset);
            m_dirty = true;  /* we cannot control pointer access! */
            m_loaded = true;
        }
        return (T*)&m_buf[m_offset];
    }
    /* clone */
    _GlobalPtrWrapper<T, SIZE, SYNC> operator &() {
        _GlobalPtrWrapper<T, SIZE, SYNC> p(m_ea);
        return p;
    }
    /* sync */
    void load() {
        ea_addr_t align_ea = ALIGN_DOWN(m_ea, 128);
        ctk_dma_get_block((void*)m_buf, align_ea,
            ALIGN_UP(SIZE, 128));
        m_loaded = true;
        m_dirty = false;
    }
    void flush() {
        ea_addr_t align_ea = ALIGN_DOWN(m_ea, 128);
        ctk_dma_putb((void*)m_buf, align_ea, ALIGN_UP(SIZE, 128), 0);
        m_dirty = false;
    }
    /* reset */
    void reset(ea_addr_t ea) {
        if (m_dirty) {
            flush();
        }
        m_ea = ea;
        m_loaded = false;
    }
    /* destructor */
    ~_GlobalPtrValue() {
        if (m_dirty) {
            flush();
        }
    }
};

// internal structure - specialized one (for larger data, No Implicit Sync)
template <typename T, int SIZE>
struct _GlobalPtrValue <T, SIZE, NoSync<T, SIZE>, false> {
    ea_addr_t m_ea;
    ea_addr_t m_aligned_ea;
    char m_buffer[ALIGN_UP(SIZE, 128) + 128];
    char *m_buf;
    int m_offset;
    _GlobalPtrValue(ea_addr_t ea) : m_ea(ea), 
        m_aligned_ea(ALIGN_DOWN(ea, 128)),
        m_buf((char*)ALIGN_UP_PTR(m_buffer, 128)),
        m_offset(ea - ALIGN_DOWN(ea, 128)) { }
    /* write */
    _GlobalPtrValue& operator=(T &t) {
        ::memcpy((void*)&m_buf[m_offset], (void*)t, SIZE);
        return *this;
    }
    _GlobalPtrValue& operator=(const T &t) {
        ::memcpy((void*)&m_buf[m_offset], (void*)t, SIZE);
        return *this;
    }
    /* read */
    operator T() {
        return *(T*)&m_buf[m_offset];
    }
    operator T*() {
        return (T*)&m_buf[m_offset];
    }
    /* clone */
    _GlobalPtrWrapper<T, SIZE, NoSync<T, SIZE> > operator &() {
        _GlobalPtrWrapper<T, SIZE, NoSync<T, SIZE> > p(m_ea);
        return p;
    }
    /* sync */
    void load() {
        ctk_dma_get_block((void*)m_buf, m_aligned_ea, ALIGN_UP(SIZE, 128));
    }
    void flush() {
        ctk_dma_putb((void*)m_buf, m_aligned_ea, ALIGN_UP(SIZE, 128), 0);
    }
    /* reset */
    void reset(ea_addr_t ea) {
        m_ea = ea;
        m_aligned_ea = ALIGN_DOWN(ea, 128);
        m_offset = ea - m_aligned_ea;
    }
};

// internal structure - this wrapper is defined only to avoid apps from
// invoking type substitution of _GlobalPtrValue directly
template <typename T, int SIZE, typename SYNC>
struct _GlobalPtrWrapper {
    _GlobalPtrValue<T, SIZE, SYNC> m_body;
};

}; // namespace ctk

#endif /* _CTK_SPU_GLOBAL_PTR_TPP */
