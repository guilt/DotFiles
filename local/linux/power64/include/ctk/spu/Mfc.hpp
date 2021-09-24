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

#ifndef _CTK_SPU_MFC_HPP
#define _CTK_SPU_MFC_HPP

/**
 * @file Mfc.hpp
 * @brief Definitions for CTK++ MFC interface
 */
#ifdef __SPU__

#include "ctk_spu_common.h"
#include "../Aligned.hpp"

namespace ctk {

/**
 * SignalRegister definitions
 */
struct SignalRegister {
    enum reg {
        reg1 = CTK_SIGNAL_REG1, 
        reg2 = CTK_SIGNAL_REG2,
    };
};

static unsigned int s_tagMask;

class Mfc {
    public:
        /**
         * Reads a 32-bit data from OUT mbox [Blocking]
         * @param[out] data - the read data
         * @see #ctk_mbox_read
         */
        inline static void readMbox(unsigned int &data) {
            ::ctk_mbox_read(&data);
        }

        /**
         * Writes a 32-bit data to IN mbox [Blocking]
         * @param[in] data - the data to be written
         * @see #ctk_mbox_write
         */
        inline static void writeMbox(unsigned int data) {
            ::ctk_mbox_write(data);
        }

        /**
         * Writes a 32-bit data to Interrupt mbox [Blocking]
         * @param[in] data - the data to be written
         */
        inline static void writeIntrMbox(unsigned int data) {
            ::ctk_mbox_interrupt(data);
        }

        /**
         * Writes a 32-bit data to Interrupt mbox [Blocking]
         * @param[in] data - the data to be written
         * @see also #writeIntrMbox 
         * @note this method is deprecated and replaced by the method
         *  #writeIntrMbox
         * @depcrecated
         */
        inline static void interruptMbox(unsigned int data) {
            ::ctk_mbox_interrupt(data);
        }

        /** for Naming convention */
        inline static void
        readBlocking(unsigned int &data) { ::ctk_mbox_read(&data); }

        /** for Naming convention */
        inline static void
        writeMboxBlocking(unsigned int data) { ::ctk_mbox_write(data); }

        /** for Naming convention */
        inline static void
        interruptMboxBlocking(unsigned int data) { ::ctk_mbox_interrupt(data); }

        /**
         * Returns the number of available data in mbox
         * @see #ctk_mbox_read_count
         */
        inline static int getMboxReadCount() {
            return ::ctk_mbox_read_count();
        }

        /**
         * Returns the number of empty (writable) data slot in mbox
         * @see #ctk_mbox_write_count
         */
        inline static int getMboxWriteCount() {
            return ::ctk_mbox_write_count();
        }

        /**
         * Returns the number of interrupt count in mbox
         * @see #ctk_mbox_interrupt_count
         * @throws #Error on error
         */
        inline static int getMboxInterruptCount() {
            return ::ctk_mbox_interrupt_count();
        }

        /*--------------------------------------------------------------*/
        /**
         * Reads the value of the signal notification register specified 
         * by #reg. [Blocking]
         * This blocks until it actually gets a signal notification.
         * @param[in] reg - specifies the signal notification register to be 
         *   read. The value should be one of either SignalRegister::reg1
         *   or SignalRegister::reg2.
         * @param[out] data - the 32-bit value
         * @see #ctk_signal_read
         */
        static inline void
        readSignalRegister(SignalRegister::reg reg, ctk_uint32_t &data) {
            ::ctk_signal_read((unsigned int)reg, &data);
        }

        /**
         * Reads the value of the signal notification register 1. [Blocking]
         * This blocks until it actually gets a signal notification.
         * @param[out] data - the 32-bit value
         * @see #ctk_signal_read_reg1
         */
        static inline void
        readSignalRegister1(ctk_uint32_t &data) {
            ::ctk_signal_read_reg1(&data);
        }

        /**
         * Reads the value of the signal notification register 2. [Blocking]
         * This blocks until it actually gets a signal notification.
         * @param[out] data - the 32-bit value
         * @see #ctk_signal_read_reg2
         */
        static inline void
        readSignalRegister2(ctk_uint32_t &data) {
            ::ctk_signal_read_reg2(&data);
        }

        /**
         * Sends a value to the signal notification register specified by the
         * given EA address.
         * [Blocking]
         * This blocks until it actually sends the signal notification.
         * @see BE Handbook pp.574 for more details.
         * @param[in] ea_addr - specifies EA addr of the target signal 
         *  notification register.
         * @param[in] data - the 32-bit value
         * @see #ctk_signal_send
         */
        static inline void
        sendSignal(ea_addr_t ea_addr, ctk_uint32_t data) {
            ::ctk_signal_send(ea_addr, data);
        }
        /*--------------------------------------------------------------*/
        /**
         * DMA get operation (non-blocking, returns immediately after
         * get is issued)
         * @note for small-size transfer you should try #get_small() instead.
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        getMemory(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag, MFC_GET_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA get operation with FENCE
         * @note for small-size transfer you should try #getf_small() instead.
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        getfMemory(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag,
                    MFC_GETF_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA get operation with BARRIER
         * @note for small-size transfer you should try #getbSmall() instead.
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        getbMemory(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag,
                    MFC_GETB_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * blocking DMA get operation
         * @note for small-size transfer you should try #getSmallBlocking()
         * instead.
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        getMemoryBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag, MFC_GET_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * blocking DMA get operation with FENCE 
         * @note for small-size transfer you should try #getfSmallBlocking()
         * instead.
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        getfMemoryBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag,
                    MFC_GETF_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * blocking DMA get operation with BARRIER 
         * @note for small-size transfer you should try #getbSmallBlocking()
         * instead.
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        getbMemoryBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag,
                    MFC_GETB_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * DMA get operation for small-size transfer
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        getMemorySmall(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size,
                    tag, MFC_GET_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA get operation with FENCE for small-size transfer
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        getbMemorySmall(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size,
                    tag, MFC_GETB_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * blocking DMA get operation for small-size transfer
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        getMemorySmallBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size, tag,
                    MFC_GET_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * blocking DMA get operation with FENCE
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        getfMemorySmallBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size, tag,
                    MFC_GETF_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * blocking DMA get operation with BARRIER
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        getbMemorySmallBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size, tag,
                    MFC_GETB_CMD);
            ::ctk_dma_wait(tag);
        }
        /*-------------------------------------------------------------------*/
        /**
         * DMA put operation
         * @note for small-size transfer you should try #putSmall() instead.
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        putMemory(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag,
                    MFC_PUT_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA put operation with FENCE
         * @note for small-size transfer you should try #putfSmall() instead.
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        putfMemory(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag,
                    MFC_PUTF_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA put operation with BARRIER
         * @note for small-size transfer you should try #putbSmall() instead.
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        putbMemory(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag,
                    MFC_PUTB_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * blocking DMA put operation
         * @note for small-size transfer you should try #putSmallBlocking()
         * instead.
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        putMemoryBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag, MFC_PUT_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * blocking DMA put operation with FENCE 
         * @note for small-size transfer you should try #putfSmallBlocking()
         * instead.
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        putfMemoryBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag,
                    MFC_PUTF_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * blocking DMA put operation with BARRIER 
         * @note for small-size transfer you should try #putbSmallBlocking()
         * instead.
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return void
         */
        static void
        putbMemoryBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer(ls_addr, ea_addr, size, tag,
                    MFC_PUTB_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * DMA put operation for small-size transfer
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        putMemorySmall(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size,
                    tag, MFC_PUT_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA put operation with FENCE for small-size transfer
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        putfMemorySmall(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size,
                    tag, MFC_PUTF_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA put operation with BARRIER for small-size transfer
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        putbMemorySmall(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size,
                    tag, MFC_PUTB_CMD);
            s_tagMask |= (1 << tag);
        }

        /**
         * blocking DMA put operation for small-size transfer
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        putMemorySmallBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size, tag,
                    MFC_PUT_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * blocking DMA put operation with FENCE
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        putfMemorySmallBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size, tag,
                    MFC_PUTF_CMD);
            ::ctk_dma_wait(tag);
        }

        /**
         * blocking DMA put operation with BARRIER
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (< #DMA_TRANSFER_SIZE)
         * @return void
         */
        static void
        putbMemorySmallBlocking(void * ls_addr, ea_addr_t ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_transfer_small(ls_addr, ea_addr, size, tag,
                    MFC_PUTB_CMD);
            ::ctk_dma_wait(tag);
        }

        /*-------------------------------------------------------------------*/
        /**
         * DMA get operation for constant-size transfer
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return void
         */
        template <typename _T> static void
        getMemory(_T& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_get((void*)&data, ea_addr, sizeof(_T), tag);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA get operation with FENCE for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return void
         */
        template <typename _T> static void
        getfMemory(_T& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_getf((void*)&data, ea_addr, sizeof(_T), tag);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA get operation with BARRIER for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return void
         */
        template <typename _T> static void
        getbMemory(_T& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_getb((void*)&data, ea_addr, sizeof(_T), tag);
            s_tagMask |= (1 << tag);
        }

        /**
         * blocking DMA get operation for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return void
         */
        template <typename _T> static void
        getMemoryBlocking(_T& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_get((void*)&data, ea_addr, sizeof(_T), 0);
            ::ctk_dma_wait(0);
        }

        /**
         * blocking DMA get operation with FENCE for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return void
         */
        template <typename _T> static void
        getfMemoryBlocking(_T& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_getf((void*)&data, ea_addr, sizeof(_T), 0);
            ::ctk_dma_wait(0);
        }

        /**
         * blocking DMA get operation with BARRIER for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return void
         */
        template <typename _T> static void
        getbMemoryBlocking(_T& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_getb((void*)&data, ea_addr, sizeof(_T), 0);
            ::ctk_dma_wait(0);
        }

        /*-------------------------------------------------------------------*/
        /**
         * DMA put operation for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return void
         */
        template <typename _T> static void
        putMemory(_T& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_put((void*)&data, ea_addr, sizeof(_T), tag);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA put operation with FENCE for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return void
         */
        template <typename _T> static void
        putfMemory(_T& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_putf((void*)&data, ea_addr, sizeof(_T), tag);
            s_tagMask |= (1 << tag);
        }

        /**
         * DMA put operation with BARRIER for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return void
         */
        template <typename _T> static void
        putbMemory(_T& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_putb((void*)&data, ea_addr, sizeof(_T), tag);
            s_tagMask |= (1 << tag);
        }

        /**
         * blocking DMA put operation for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return void
         */
        template <typename _T> static void
        putMemoryBlocking(_T& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_put((void*)&data, ea_addr, sizeof(_T), 0);
            ::ctk_dma_wait(0);
        }

        /**
         * blocking DMA put operation with FENCE for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return void
         */
        template <typename _T> static void
        putfMemoryBlocking(_T& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_putf((void*)&data, ea_addr, sizeof(_T), 0);
            ::ctk_dma_wait(0);
        }

        /**
         * blocking DMA put operation with BARRIER for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return void
         */
        template <typename _T> static void
        putbMemoryBlocking(_T& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_putb((void*)&data, ea_addr, sizeof(_T), 0);
            ::ctk_dma_wait(0);
        }

        /*-------------------------------------------------------------------*/
        /* specialized ones */
        template <typename _T, int _ALIGN> static void
        getMemory(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_get((void*)data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, tag);
            s_tagMask |= (1 << tag);
        }

        template <typename _T, int _ALIGN> static void
        getfMemory(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_getf(data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, tag);
            s_tagMask |= (1 << tag);
        }

        template <typename _T, int _ALIGN> static void
        getbMemory(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_getb(data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, tag);
            s_tagMask |= (1 << tag);
        }

        template <typename _T, int _ALIGN> static void
        getMemoryBlocking(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_get(data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, 0);
            ::ctk_dma_wait(0);
        }

        template <typename _T, int _ALIGN> static void
        getfMemoryBlocking(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_getf(data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, 0);
            ::ctk_dma_wait(0);
        }

        template <typename _T, int _ALIGN> static void
        getbMemoryBlocking(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_getb(data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, 0);
            ::ctk_dma_wait(0);
        }

        /*-------------------------------------------------------------------*/
        template <typename _T, int _ALIGN> static void
        putMemory(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_put((void*)data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, tag);
            s_tagMask |= (1 << tag);
        }

        template <typename _T, int _ALIGN> static void
        putfMemory(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_putf((void*)data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, tag);
            s_tagMask |= (1 << tag);
        }

        template <typename _T, int _ALIGN> static void
        putbMemory(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            int tag = ctk_dma_new_tag();
            ::ctk_dma_putb((void*)data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, tag);
            s_tagMask |= (1 << tag);
        }

        template <typename _T, int _ALIGN> static void
        putMemoryBlocking(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_put(data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, 0);
            ::ctk_dma_wait(0);
        }

        template <typename _T, int _ALIGN> static void
        putfMemoryBlocking(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_putf(data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, 0);
            ::ctk_dma_wait(0);
        }

        template <typename _T, int _ALIGN> static void
        putbMemoryBlocking(Aligned<_T, _ALIGN>& data, ea_addr_t ea_addr)
        {
            ::ctk_dma_putb(data.ptr(), ea_addr, Aligned<_T, _ALIGN>::size, 0);
            ::ctk_dma_wait(0);
        }

        /*-------------------------------------------------------------------*/
        /**
         * Waits all of DMA transfers issued so far.
         */
        static void
        waitMemoryAll()
        {
            ::ctk_dma_wait_all(s_tagMask);
            s_tagMask = 0;
        }

        /**
         * Waits any of DMA transfers issued so far.
         */
        static void
        waitMemoryAny()
        {
            ::ctk_dma_wait_any(s_tagMask);
        }

};

/* the single MFC instance */
//extern Mfc mfc;
static Mfc mfc __attribute__((unused));

#if 0

//#define DMA_LOOP_UNROLL

template <unsigned int __N, bool _aligned = (__N % 16 == 0 || __N < 16), bool _large = (__N > DMA_TRANSFER_SIZE)> struct dma_op { };

template <unsigned int __N> struct dma_op <__N, true, true> {
    static inline void transfer(void *ls_addr, ea_addr_t ea_addr, int tag, int cmd) {
        CTK_DEBUG_ASSERT(ptr2int(ls_addr)%16 == 0);
        CTK_DEBUG_ASSERT((ea_addr)%16 == 0);
#ifdef DMA_LOOP_UNROLL
        ea_addr32_t ea_hi32 = ea_hi32(ea_addr);
        ea_addr32_t ea_lo32 = ea_lo32(ea_addr);
        DEBUG_PRINTF("DMA[%x]: [LS]:%p, [EA]:%x %x, size:%u tag:%d\n", 
            cmd, ls_addr, ea_hi32, ea_lo32, DMA_TRANSFER_SIZE, tag);
        spu_mfcdma64(ls_addr, ea_hi32, ea_lo32, DMA_TRANSFER_SIZE, tag, cmd);
        dma_op<__N - DMA_TRANSFER_SIZE>::transfer(
            int2ptr(ptr2int(ls_addr) + DMA_TRANSFER_SIZE),
            ea_hi32, ea_lo32 + DMA_TRANSFER_SIZE, tag, cmd);
#else /* DMA_LOOP_UNROLL */
        ::ctk_dma_transfer(ls_addr, ea_addr, __N, tag, cmd);
#endif /* DMA_LOOP_UNROLL */
    }

    static inline void transfer(void *ls_addr, ea_addr32_t ea_hi32, ea_addr32_t ea_lo32, int tag, int cmd) {
        CTK_DEBUG_ASSERT(ptr2int(ls_addr)%16 == 0);
        CTK_DEBUG_ASSERT((ea_lo32)%16 == 0);
        DEBUG_PRINTF("DMA[%x]: [LS]:%p, [EA]:%x %x, size:%u tag:%d\n", 
            cmd, ls_addr, ea_hi32, ea_lo32, DMA_TRANSFER_SIZE, tag);
        spu_mfcdma64(ls_addr, ea_hi32, ea_lo32, DMA_TRANSFER_SIZE, tag, cmd);
        dma_op<__N - DMA_TRANSFER_SIZE>::transfer(
            int2ptr(ptr2int(ls_addr) + DMA_TRANSFER_SIZE),
            ea_hi32, ea_lo32 + DMA_TRANSFER_SIZE,
            tag, cmd);
    }
};

template <unsigned int __N> struct dma_op <__N, true, false> {
    static inline void transfer(void *ls_addr, ea_addr_t ea_addr, int tag, int cmd) {
        CTK_DEBUG_ASSERT(ptr2int(ls_addr)%16 == 0);
        CTK_DEBUG_ASSERT((ea_addr)%16 == 0);
        DEBUG_PRINTF("DMA[%x]: [LS]:%p, [EA]:%x %x, size:%u tag:%d\n", 
            cmd, ls_addr, ea_hi32(ea_addr), ea_lo32(ea_addr), __N, tag);
        spu_mfcdma64(ls_addr, ea_hi32(ea_addr), ea_lo32(ea_addr), __N, tag, cmd);
    }

    static inline void transfer(void *ls_addr, ea_addr32_t ea_hi32, ea_addr32_t ea_lo32, int tag, int cmd) {
        CTK_DEBUG_ASSERT(ptr2int(ls_addr)%16 == 0);
        CTK_DEBUG_ASSERT((ea_lo32)%16 == 0);
        DEBUG_PRINTF("DMA[%x]: [LS]:%p, [EA]:%x %x, size:%u tag:%d\n", 
            cmd, ls_addr, ea_hi32, ea_lo32, __N, tag);
        spu_mfcdma64(ls_addr, ea_hi32, ea_lo32, __N, tag, cmd);
    }
};

#endif /* 0 */

#endif /* __SPU__ */
};
#endif /* _CTK_SPU_MFC_HPP */
