#ifndef _CTK_SPU_PPE_CALLBACK_H
#define _CTK_SPU_PPE_CALLBACK_H

#include <errno.h>

/* The following code is mainly derived from newlib/spu/c99ppe.h. 
 * (The copyright should be found in the same source package) */

#define SPE_CTK_SIGNALCODE  0x21ff

#define SPE_CTK_OP_SHIFT    	24
#define SPE_CTK_OP_MASK	    	0xff
#define SPE_CTK_DATA_MASK   	0xffffff

#ifdef __cplusplus
extern "C" {
#endif

struct spe_reg128{
  unsigned int slot[4];
};

static inline void
ctk_callback_send_to_ppe(int opcode, void *data)
{
    unsigned int combined = ( ( opcode<<24 )&0xff000000 ) | ( ( unsigned int )data & 0x00ffffff );
    struct spe_reg128* ret = (struct spe_reg128*)data;

    vector unsigned int stopfunc = {
            SPE_CTK_SIGNALCODE,     /* stop 0x210x*/
            (unsigned int) combined,
            0x4020007f,     /* nop */
            0x35000000      /* bi $0 */
    };

    typedef void (*ppe_callback_invoker_t)(void);
    ppe_callback_invoker_t f = (ppe_callback_invoker_t) &stopfunc;
    asm ("sync":::"memory");
    f();
    errno = ret->slot[3];
    return;
}

#ifdef __cplusplus
};
#endif

#endif /* _CTK_SPU_PPE_CALLBACK_H */
