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

#ifndef _CTK_SPE_TASK_COMMON_H
#define _CTK_SPE_TASK_COMMON_H

#include "ctk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CTK_SPE_TASK_START_ADDR
#define CTK_SPE_TASK_START_ADDR  0x80
#endif /* !CTK_SPE_TASK_START_ADDR */

#define CTK_SPE_TASK_LS_SIZE  (LS_SIZE - CTK_SPE_TASK_START_ADDR + 0x80)

#include "ctk_spe_task_status.h"

/**
 * SPE task reference type
 */
typedef ea_addr_t ctk_spe_task_ea_t;

/**
 * SPE task text image 
 */
typedef struct {
    unsigned int counter;
    unsigned int magic;
    ea_addr_t load_image;
    ls_addr_t entry;
    ls_addr_t load_addr;
    ls_addr_t load_size;
    char pad[0] __attribute__((aligned(16)));
/* 32 bytes */
} ctk_spe_task_image_body_t;

typedef ctk_spe_task_image_body_t *ctk_spe_task_image_t;

#define CTK_SPE_TASK_IMAGE_MAGIC        0xbaba1234

/**
 * SPE task info type (128 bytes)
 */
typedef struct ctk_spe_task_info {
    ea_addr_t next;             // next
    ea_addr_t task;             // self
    ea_addr_t text;             // text area
    ls_addr_t load_addr;
    ls_addr_t load_size;
/* 32 bytes */
    ls_addr_t entry;
    ls_addr_t data_end;
    ctk_uint16_t stack_size;
    ctk_uint8_t lock:1;
    ctk_uint8_t flag:3;
    volatile ctk_uint8_t status:4;
    ctk_val128_t rc __attribute__((aligned(16)));
/* 64 bytes */
    ctk_uint16_t id;
    ctk_uint64_t arg[2];
    ea_addr_t class_ea;
} __attribute__((aligned(128))) ctk_spe_task_info_t;

/**
 * SPE task save area structure
 */
typedef struct {
    unsigned char ls[CTK_SPE_TASK_LS_SIZE] __attribute__((aligned(128)));
    unsigned int gprs[4][128] __attribute__((aligned(128)));
} ctk_spe_task_save_t __attribute__((aligned(128)));

/** Flag for #ctk_spe_task_create; allocate task save area in the main 
  memory to allow the task to yield (i.e., to be swapped out). */
#define CTK_SPE_TASK_SAVE              0x0001

/** Flag for #ctk_spe_task_create; add access hints for text image
  for the task */
#define CTK_SPE_TASK_ADD_ACCESS_HINT   0x0002

/** Flag for #ctk_spe_task_create; detach the task (PPE cannot / should
   not wait the task) */
#define CTK_SPE_TASK_DETACHED          0x0004

#ifdef __cplusplus
};
#endif

#endif /* _CTK_SPE_TASK_COMMON_H */
