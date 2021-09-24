/*
 * CTK - The Cell Tool Kit Library
 * http://ctk-dev.sourceforge.net/
 * http://cell.fixstars.com/ctk/
 *
 * Copyright (C) 2006-2008 Fixstars Corp.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _CTK_SPU_DMA_LIST_H
#define _CTK_SPU_DMA_LIST_H

/**
 * @file ctk_spu_dma_list.h
 * @brief Definitions for structures / routines for DMA list operations
 */

#include "../ctk_common.h"

#ifdef __SPU__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DMA list element structure
 */
typedef struct {
    ctk_uint64_t notify:1;        /**< Stall-and-Notify Flag */
    ctk_uint64_t reserved:15;
    ctk_uint64_t size:16;         /**< List transfer size (LTS) */
    ctk_uint64_t eal:32;          /**< 32-bit EA Low addr (LEAL) */
} ctk_dma_list_elem_t;

/**
 * DMA list structure for up to 16 DMA list elements.
 * (In most cases it should suffice since; with each DMA list element
 * we can transfer 16Kb, therefore in total 16 elements can transfer
 * 16Kb * 16 = 256K bytes of memory (= LS size).)
 */
typedef struct {
    ls_addr_t           ls_base;
    ea_addr32_t         ea_base;        /**< 32-bit EA Hi addr */
    unsigned int        nelems;         /**< # of list elements */
    ctk_dma_list_elem_t elems[16] __attribute__((aligned(8)));
} ctk_dma_list_t;

/**
 * DMA list structure for variable number of DMA list elements.
 * (In most cases you should use #ctk_dma_list_t instead of #ctk_dma_list_va_t.)
 */
typedef struct {
    ls_addr_t           ls_base;
    ea_addr32_t         ea_base;        /**< 32-bit EA Hi addr */
    unsigned int        nelems;         /**< # of list elements */
    ctk_dma_list_elem_t *elems;         /**< pointer to the list elements */
} ctk_dma_list_va_t;

/**
 * Initializes the given ctk_dma_list_t.
 * param[in,out] list - the DMA list to be initialized
 * param[in] ls_base - the LS base addr for the DMA list transfer
 * param[in] ea_base - the EA base addr for the DMA list transfer
 */
static inline void
ctk_dma_list_init(ctk_dma_list_t *list, void *ls_base, ea_addr_t ea_base)
{
    list->ls_base = ptr2int(ls_base);
    list->ea_base = ea_hi32(ea_base);
    list->nelems = 0;
}

/**
 * Initializes the given ctk_dma_list_va_t.
 * @note the caller should also allocate a memory buffer for DMA list elements.
 * @see #ctk_dma_list_va_alloc_elems, #ctk_dma_list_va_alloca_elems
 * param[in,out] list - the DMA list to be initialized
 * param[in] ls_base - the LS base addr for the DMA list transfer
 * param[in] ea_base - the EA base addr for the DMA list transfer
 */
static inline void
ctk_dma_list_va_init(ctk_dma_list_va_t *list, void *ls_base, ea_addr_t ea_base)
{
    list->ls_base = ptr2int(ls_base);
    list->ea_base = ea_hi32(ea_base);
    list->nelems = 0;
}

/**
 * Allocates the given number of DMA list elements for a DMA list.
 * @note the allocated DMA list should be freed by #ctk_dma_list_va_free_elems.
 * @param[in,out] list - the DMA list for which a memory buffer for DMA
 * list elements is allocated.
 * @param[in] max_elems - the number of max list elements to be allocated.
 * @see #ctk_dma_list_va_alloca_elems
 */
static inline void
ctk_dma_list_va_alloc_elems(ctk_dma_list_va_t *list, int max_elems)
{
    list->elems = __cast(ctk_dma_list_elem_t *,
            ctk_malloc_align(max_elems * sizeof(ctk_dma_list_elem_t), 8));
}

/**
 * Frees memory of DMA list elements allocated by ctk_dma_list_va_alloc_elems.
 * @param[in] list - the DMA list for which the memory buffer for the list
 * elements is freed.
 */
static inline void
ctk_dma_list_va_free_elems(ctk_dma_list_va_t *list)
{
    ctk_free_align(list->elems);
}

/**
 * Allocates the given number of DMA list elements and initializes the list.
 * @note the allocated DMA list is on the current stack frame and do not
 * need to be freed.  Notice that the elements cannot be accessed from outside
 * of the current stack frame.
 * @param[in,out] list - the DMA list for which a memory buffer for DMA
 * list elements is allocated.
 * @param[in] max_elems - the number of max list elements to be allocated.
 * @see #ctk_dma_list_va_alloc_elems
 */
#define ctk_dma_list_va_alloca_elems(list, max_elems)    \
    char _tmp_ ## list[ALIGN_UP((max_elems)*sizeof(ctk_dma_list_t), 8) + 8]; \
    (list)->elems = (void*)ALIGN_UP(_tmp_ ## list, 8);

/**
 * Adds (sets) a new DMA list element in the list.
 * param[in,out] list - the DMA list to which an element is added
 * param[in] ea - the starting EA addr of the DMA transfer to be added
 * param[in] size - the size of the DMA transfer to be added
 */
static inline void
ctk_dma_list_add_elem(ctk_dma_list_t *list, ea_addr_t ea, size_t size)
{
    list->elems[list->nelems].eal = ea_lo32(ea);
    list->elems[list->nelems].size = size;
    list->nelems++;
}

/**
 * Adds (sets) a new DMA list element in the ctk_dma_list_va_t list.
 * param[in,out] list - the DMA list to which an element is added
 * param[in] ea - the starting EA addr of the DMA transfer to be added
 * param[in] size - the size of the DMA transfer to be added
 */
static inline void
ctk_dma_list_va_add_elem(ctk_dma_list_va_t *list, ea_addr_t ea, size_t size)
{
    list->elems[list->nelems].eal = ea_lo32(ea);
    list->elems[list->nelems].size = size;
    list->nelems++;
}

/**
 * Initiates a DMA transfer for the given DMA list.
 * param[in] list - the DMA list to be transfered.  The list must be
 * initialized by ctk_dma_list_init (or by directly filling the structure
 * members) and also have been added one or more ctk_dma_list_elem_t by
 * ctk_dma_list_add_elem).
 * param[in] tag - the MFC DMA tag
 * param[in] cmd - the MFC DMA command
 */
static inline void
ctk_dma_list_transfer(ctk_dma_list_t *list, int tag, int cmd)
{
    int listsize;
    CTK_DEBUG_ASSERT(list != NULL);
    listsize = list->nelems * sizeof(ctk_dma_list_elem_t);
    spu_mfcdma64((void*)list->ls_base, list->ea_base,
        ptr2int(&list->elems[0]), listsize, tag, (cmd | 0x4));
}

/**
 * Initiates a DMA transfer for the given DMA list.
 * param[in] list - the DMA list to be transfered.  The list must be
 * initialized by ctk_dma_list_va_init (or by directly filling the structure
 * members) and also have been added one or more ctk_dma_list_elem_t by
 * ctk_dma_list_va_add_elem).
 * param[in] tag - the MFC DMA tag
 * param[in] cmd - the MFC DMA command
 */
static inline void
ctk_dma_list_va_transfer(ctk_dma_list_va_t *list, int tag, int cmd)
{
    int listsize;
    CTK_DEBUG_ASSERT(list != NULL);
    listsize = list->nelems * sizeof(ctk_dma_list_elem_t);
    spu_mfcdma64((void*)list->ls_base, list->ea_base,
        ptr2int(list->elems), listsize, tag, (cmd | 0x4));
}

/*---------------------------------------------------------------------------*/
/**
 * DMA get operation (from EA to LS) using DMA list.
 * param[in] list - the DMA list to be transfered.
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 */
static inline void
ctk_dma_list_get(ctk_dma_list_t *list, int tag)
{
    ctk_dma_list_transfer(list, tag, MFC_GET_CMD);
}

/**
 * DMA get operation <b>with fence</b> using DMA list.
 * param[in] list - the DMA list to be transfered.
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 */
static inline void
ctk_dma_list_getf(ctk_dma_list_t *list, int tag)
{
    ctk_dma_list_transfer(list, tag, MFC_GETF_CMD);
}

/**
 * DMA get operation <b>with barrier</b> using DMA list.
 * param[in] list - the DMA list to be transfered.
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 */
static inline void
ctk_dma_list_getb(ctk_dma_list_t *list, int tag)
{
    ctk_dma_list_transfer(list, tag, MFC_GETB_CMD);
}

/**
 * blocking DMA get operation (from EA to LS) using DMA list.
 * param[in] list - the DMA list to be transfered.
 */
static inline void
ctk_dma_list_get_block(ctk_dma_list_t *list)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_list_transfer(list, tag, MFC_GET_CMD);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA get operation <b>with fence</b> using DMA list.
 * param[in] list - the DMA list to be transfered.
 */
static inline void
ctk_dma_list_getf_block(ctk_dma_list_t *list)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_list_transfer(list, tag, MFC_GETF_CMD);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA get operation <b>with barrier</b> using DMA list.
 * param[in] list - the DMA list to be transfered.
 */
static inline void
ctk_dma_list_getb_block(ctk_dma_list_t *list)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_list_transfer(list, tag, MFC_GETB_CMD);
    ctk_dma_wait(tag);
}
/*---------------------------------------------------------------------------*/
/**
 * DMA put operation (from EA to LS) using DMA list.
 * param[in] list - the DMA list to be transfered.
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 */
static inline void
ctk_dma_list_put(ctk_dma_list_t *list, int tag)
{
    ctk_dma_list_transfer(list, tag, MFC_PUT_CMD);
}

/**
 * DMA put operation <b>with fence</b> using DMA list.
 * param[in] list - the DMA list to be transfered.
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 */
static inline void
ctk_dma_list_putf(ctk_dma_list_t *list, int tag)
{
    ctk_dma_list_transfer(list, tag, MFC_PUTF_CMD);
}

/**
 * DMA put operation <b>with barrier</b> using DMA list.
 * param[in] list - the DMA list to be transfered.
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 */
static inline void
ctk_dma_list_putb(ctk_dma_list_t *list, int tag)
{
    ctk_dma_list_transfer(list, tag, MFC_PUTB_CMD);
}

/**
 * blocking DMA put operation (from EA to LS) using DMA list.
 * param[in] list - the DMA list to be transfered.
 */
static inline void
ctk_dma_list_put_block(ctk_dma_list_t *list)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_list_transfer(list, tag, MFC_PUT_CMD);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA put operation <b>with fence</b> using DMA list.
 * param[in] list - the DMA list to be transfered.
 */
static inline void
ctk_dma_list_putf_block(ctk_dma_list_t *list)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_list_transfer(list, tag, MFC_PUTF_CMD);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA put operation <b>with barrier</b> using DMA list.
 * param[in] list - the DMA list to be transfered.
 */
static inline void
ctk_dma_list_putb_block(ctk_dma_list_t *list)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_list_transfer(list, tag, MFC_PUTB_CMD);
    ctk_dma_wait(tag);
}

#ifdef __cplusplus
}
#endif
#endif /* __SPU__ */
#endif /* _CTK_SPU_DMA_LIST_H */
