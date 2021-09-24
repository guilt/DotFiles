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

#ifndef _CTK_SPE_CONTEXT_HPP
#define _CTK_SPE_CONTEXT_HPP

#include "ctk_common.h"
#include "ctk_spe.h"
#include "ctk_signal.h"
#include "ctk_atomic.h"
#include "ctk_mbox.h"
#include "ctk_dma.h"
#include "Common.hpp"
#include "Exception.hpp"
#include "SpeImage.hpp"

/**
 * @file SpeContext.hpp
 * @brief Definitions for CTK++ SPE context interface
 */
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

/**
 * SPE generic interface
 */
class Spe {
        unsigned int m_tagMask;

    protected:
        /* implement me */
        virtual const ctk_spe_context_t& getInternalContext() const = 0;

    public:
        /**
         * SPE status
         */
        enum SpeStatus {
            UNKNOWN = ::CTK_SPE_STATUS_UNKNOWN, /**< unknown or error */
            CREATED = ::CTK_SPE_STATUS_CREATED, /**< created */
            LOADED = ::CTK_SPE_STATUS_LOADED,   /**< program loaded */
            READY = ::CTK_SPE_STATUS_READY,     /**< thread is ready to run */
            RUNNING = ::CTK_SPE_STATUS_RUNNING, /**< running */
            STOPPED = ::CTK_SPE_STATUS_STOPPED, /**< stopped */
        };

        virtual ~Spe() { }
        
        /** Returns true current SPE status */
        virtual SpeStatus getStatus() const {
            return (SpeStatus)::ctk_spe_context_get_status(getInternalContext());
        }

        /** Returns the mapped LS addr
         *  @see #ctk_spe_map_ls
         *  @throws #Error on error
         */
        virtual ea_addr_t getLS() {
            ea_addr_t addr;
            int ret = ::ctk_spe_map_ls(getInternalContext(), &addr);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            return addr;
        }

        /** Returns the mapped Signal Notification register 1
         *  @see #ctk_spe_map_signal1
         *  @throws #Error on error
         */
        virtual ea_addr_t getSignalReg1() {
            ea_addr_t addr;
            int ret = ::ctk_spe_map_signal1(getInternalContext(), &addr);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            return addr;
        }

        /** Returns the mapped Signal Notification register 2
         *  @see #ctk_spe_map_signal2
         */
        virtual ea_addr_t getSignalReg2() {
            ea_addr_t addr;
            ::ctk_spe_map_signal2(getInternalContext(), &addr);
            return addr;
        }

        /**
         * Reads a 32-bit data from OUT mbox [Non-blocking]
         * @param[out] the read data
         * @return 1 if the data is read, 0 otherwise
         * @see #ctk_mbox_read
         * @throws #Error on error
         */
        virtual int readMbox(unsigned int &data) {
            int ret = ::ctk_mbox_read(getInternalContext(), &data);
            if (ret == CTK_ERROR_NO_DATA)
                return 0;
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            return 1;
        }

        /**
         * Writes a 32-bit data to IN mbox [Non-blocking]
         * @param[in] the data to be written
         * @see #ctk_mbox_write
         * @throws #Error on error
         */
        virtual void writeMbox(unsigned int data) {
            int ret = ::ctk_mbox_write(getInternalContext(), data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Reads a 32-bit data from OUT mbox [Blocking]
         * @param[out] the read data
         * @see #ctk_mbox_read_block
         * @throws #Error on error
         */
        virtual void readMboxBlocking(unsigned int &data) {
            int ret = ::ctk_mbox_read_block(getInternalContext(), &data);
            if (ret != CTK_SUCCESS) 
                throw Error(ret);
        }

        /**
         * Writes a 32-bit data to IN mbox [Blocking]
         * @param[in] the data to be written
         * @see #ctk_mbox_write_block
         * @throws #Error on error
         */
        virtual void writeMboxBlocking(unsigned int data) {
            int ret = ::ctk_mbox_write_block(getInternalContext(), data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Reads a 32-bit data from OUT Interrupt mbox [Non-blocking]
         * @param[out] the read data
         * @return 1 if the data is read, 0 otherwise
         * @see #ctk_mbox_read
         * @throws #Error on error
         */
        virtual int readIntrMbox(unsigned int &data) {
            int ret = ::ctk_intr_mbox_read(getInternalContext(), &data);
            if (ret == CTK_ERROR_NO_DATA)
                return 0;
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            return 1;
        }

        /**
         * Reads a 32-bit data from OUT Interrupt mbox [Blocking]
         * @param[out] the read data
         * @see #ctk_mbox_read_block
         * @throws #Error on error
         */
        virtual void readIntrMboxBlocking(unsigned int &data) {
            int ret = ::ctk_intr_mbox_read_block(getInternalContext(), &data);
            if (ret != CTK_SUCCESS) 
                throw Error(ret);
        }

        /**
         * Returns the number of available data in mbox
         * @see #ctk_mbox_read_count
         * @throws #Error on error
         */
        virtual int getMboxReadCount() {
            int ret = ::ctk_mbox_read_count(getInternalContext());
            if (ret < 0)
                throw Error(-ret);
            return ret;
        }

        /**
         * Returns the number of empty (writable) data slot in mbox
         * @see #ctk_mbox_write_count
         * @throws #Error on error
         */
        virtual int getMboxWriteCount() {
            int ret = ::ctk_mbox_write_count(getInternalContext());
            if (ret < 0)
                throw Error(-ret);
            return ret;
        }

        /**
         * Returns the number of available data in OUT Interrupt mbox
         * @see #ctk_mbox_read_count
         * @throws #Error on error
         */
        virtual int getIntrMboxReadCount() {
            int ret = ::ctk_intr_mbox_read_count(getInternalContext());
            if (ret < 0)
                throw Error(-ret);
            return ret;
        }

        /**
         * Returns the number of interrupt count in mbox
         * @see #ctk_mbox_interrupt_count
         * @throws #Error on error
         */
        virtual int getMboxInterruptCount() {
            int ret = ::ctk_mbox_interrupt_count(getInternalContext());
            if (ret < 0)
                throw Error(-ret);
            return ret;
        }

        /**
         * Writes the #data to the signal notification register specified by
         * #reg. [This will block until the SPE starts if it has not yet
         *   started]
         * @param[in] reg - specifies the signal notification register to be 
         *   written. The value should be one of either SignalRegister::reg1
         *   or SignalRegister::reg2.
         * @param[in] data - the 32-bit data to be written
         * @see #ctk_signal_write
         * @throws #Error on error
         */
        virtual inline void
        writeSignalRegister(SignalRegister::reg reg, ctk_uint32_t data)
        {
            int ret = ::ctk_signal_write(getInternalContext(), (unsigned int)reg, data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Writes the #data to the signal notification register 1.
         * @param[in] data - the 32-bit data to be written
         * @throws #Error on error
         * @see #ctk_signal_write_reg1
         */
        virtual inline void
        writeSignalRegister1(unsigned int data)
        {
            int ret = ::ctk_signal_write_reg1(getInternalContext(), data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Writes the #data to the signal notification register 2.
         * @param[in] data - the 32-bit data to be written
         * @throws #Error on error
         * @see #ctk_signal_write_reg2
         */
        virtual inline void
        writeSignalRegister2(unsigned int data)
        {
            int ret = ::ctk_signal_write_reg2(getInternalContext(), data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * DMA get operation (non-blocking, returns immediately after
         * get is issued)
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        getMemory(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_get(getInternalContext(), ls_addr, ea_addr, size, tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * DMA get operation with FENCE
         * @note for small-size transfer you should try #getf_small() instead.
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        getfMemory(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_getf(getInternalContext(), ls_addr, ea_addr, size, tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * DMA get operation with BARRIER
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        getbMemory(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_getb(getInternalContext(), ls_addr, ea_addr, size, tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * blocking DMA get operation
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        getMemoryBlocking(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            return ::ctk_dma_get_block(getInternalContext(), ls_addr, ea_addr, size);
        }

        /**
         * blocking DMA get operation with FENCE 
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        getfMemoryBlocking(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            return ::ctk_dma_getf_block(getInternalContext(), ls_addr, ea_addr, size);
        }

        /**
         * blocking DMA get operation with BARRIER 
         * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        getbMemoryBlocking(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            return ::ctk_dma_getb_block(getInternalContext(), ls_addr, ea_addr, size);
        }

        /*-------------------------------------------------------------------*/
        /**
         * DMA put operation
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        putMemory(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_put(getInternalContext(), ls_addr, ea_addr, size, tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * DMA put operation with FENCE
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        putfMemory(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_putf(getInternalContext(), ls_addr, ea_addr, size, tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * DMA put operation with BARRIER
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        putbMemory(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_putb(getInternalContext(), ls_addr, ea_addr, size, tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * blocking DMA put operation
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        putMemoryBlocking(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            return ::ctk_dma_put_block(getInternalContext(), ls_addr, ea_addr, size);
        }

        /**
         * blocking DMA put operation with FENCE 
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        putfMemoryBlocking(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            return ::ctk_dma_putf_block(getInternalContext(), ls_addr, ea_addr, size);
        }

        /**
         * blocking DMA put operation with BARRIER 
         * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
         * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
         * @param[in] size - transfer size (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        inline int
        putbMemoryBlocking(ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
        {
            return ::ctk_dma_putb_block(getInternalContext(), ls_addr, ea_addr, size);
        }

        /*-------------------------------------------------------------------*/
        /**
         * DMA get operation for constant-size transfer
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        getMemory(ls_addr_t ls_addr, _T& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_get(getInternalContext(), ls_addr, (void*)&data,
                            sizeof(_T), tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * DMA get operation with FENCE for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        getfMemory(ls_addr_t ls_addr, _T& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_getf(getInternalContext(), ls_addr, (void*)&data,
                            sizeof(_T), tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * DMA get operation with BARRIER for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        getbMemory(ls_addr_t ls_addr, _T& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_getb(getInternalContext(), ls_addr, (void*)&data,
                            sizeof(_T), tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * blocking DMA get operation for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        getMemoryBlocking(ls_addr_t ls_addr, _T& data)
        {
            return ::ctk_dma_get_block(getInternalContext(), ls_addr, (void*)&data,
                            sizeof(_T));
        }

        /**
         * blocking DMA get operation with FENCE for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        getfMemoryBlocking(ls_addr_t ls_addr, _T& data)
        {
            return ::ctk_dma_getf_block(getInternalContext(), ls_addr, (void*)&data,
                            sizeof(_T));
        }

        /**
         * blocking DMA get operation with BARRIER for constant-size transfer 
         * @param[in] data - the LS data to be filled
         * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        getbMemoryBlocking(ls_addr_t ls_addr, _T& data)
        {
            return ::ctk_dma_getb_block(getInternalContext(), ls_addr, (void*)&data,
                            sizeof(_T));
        }

        /*-------------------------------------------------------------------*/
        /**
         * DMA put operation for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        putMemory(ls_addr_t ls_addr, _T& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_put(getInternalContext(), ls_addr, (void*)&data,
                            sizeof(_T), tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * DMA put operation with FENCE for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        putfMemory(ls_addr_t ls_addr, _T& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_putf(getInternalContext(), ls_addr, (void*)&data,
                            sizeof(_T), tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * DMA put operation with BARRIER for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        putbMemory(ls_addr_t ls_addr, _T& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_putb(getInternalContext(), ls_addr, (void*)&data,
                            sizeof(_T), tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        /**
         * blocking DMA put operation for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        putMemoryBlocking(ls_addr_t ls_addr, _T& data)
        {
            return ::ctk_dma_put_block(getInternalContext(), ls_addr, (void*)&data,
                        sizeof(_T));
        }

        /**
         * blocking DMA put operation with FENCE for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        putfMemoryBlocking(ls_addr_t ls_addr, _T& data)
        {
            return ::ctk_dma_putf_block(getInternalContext(), ls_addr, (void*)&data,
                        sizeof(_T));
        }

        /**
         * blocking DMA put operation with BARRIER for constant-size transfer 
         * @param[in] data - the LS data to be sent
         * @param[in] ea_addr - destination EA addr (must be aligned)
         * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
         */
        template <typename _T> inline int
        putbMemoryBlocking(ls_addr_t ls_addr, _T& data)
        {
            return ::ctk_dma_putb_block(getInternalContext(), ls_addr, (void*)&data,
                        sizeof(_T));
        }

        /*-------------------------------------------------------------------*/
        /* specialized ones */
        template <typename _T, int _ALIGN> inline int
        getMemory(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_get(getInternalContext(), ls_addr, (void*)data,
                            data.size(), tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        template <typename _T, int _ALIGN> inline int
        getfMemory(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_getf(getInternalContext(), ls_addr, (void*)data,
                            data.size(), tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        template <typename _T, int _ALIGN> inline int
        getbMemory(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_getb(getInternalContext(), ls_addr, (void*)data,
                            data.size(), tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        template <typename _T, int _ALIGN> inline int
        getMemoryBlocking(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            return ::ctk_dma_get_block(getInternalContext(), ls_addr, (void*)data,
                            Aligned<_T, _ALIGN>::size);
        }

        template <typename _T, int _ALIGN> inline int
        getfMemoryBlocking(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            return ::ctk_dma_getf_block(getInternalContext(), ls_addr, (void*)data,
                            Aligned<_T, _ALIGN>::size);
        }

        template <typename _T, int _ALIGN> inline int
        getbMemoryBlocking(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            return ::ctk_dma_getb_block(getInternalContext(), ls_addr, (void*)data,
                            Aligned<_T, _ALIGN>::size);
        }

        /*-------------------------------------------------------------------*/
        /* specialized ones */
        template <typename _T, int _ALIGN> inline int
        putMemory(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_put(getInternalContext(), ls_addr, (void*)data,
                            Aligned<_T, _ALIGN>::size, tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        template <typename _T, int _ALIGN> inline int
        putfMemory(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_putf(getInternalContext(), ls_addr, (void*)data,
                            Aligned<_T, _ALIGN>::size, tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        template <typename _T, int _ALIGN> inline int
        putbMemory(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            int tag = ctk_dma_new_tag();
            int ret = ::ctk_dma_putb(getInternalContext(), ls_addr, (void*)data,
                            Aligned<_T, _ALIGN>::size, tag);
            if (ret == CTK_SUCCESS)
                m_tagMask |= (1 << tag);
            return ret;
        }

        template <typename _T, int _ALIGN> inline int
        putMemoryBlocking(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            return ::ctk_dma_put_block(getInternalContext(), ls_addr, (void*)data,
                            Aligned<_T, _ALIGN>::size);
        }

        template <typename _T, int _ALIGN> inline int
        putfMemoryBlocking(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            return ::ctk_dma_putf_block(getInternalContext(), ls_addr, (void*)data,
                            Aligned<_T, _ALIGN>::size);
        }

        template <typename _T, int _ALIGN> inline int
        putbMemoryBlocking(ls_addr_t ls_addr, Aligned<_T, _ALIGN>& data)
        {
            return ::ctk_dma_putb_block(getInternalContext(), ls_addr, (void*)data,
                            Aligned<_T, _ALIGN>::size);
        }

        /*-------------------------------------------------------------------*/
        /**
         * Waits all of DMA transfers issued so far.
         */
        virtual inline int
        waitMemoryAll()
        {
            int ret = ::ctk_dma_wait_all(getInternalContext(), m_tagMask);
            m_tagMask = 0;
            return ret;
        }

        /**
         * Waits any of DMA transfers issued so far.
         */
        virtual inline int
        waitMemoryAny()
        {
            return ::ctk_dma_wait_any(getInternalContext(), m_tagMask);
        }
}; // Spe

/**
 * SPE context class
 */
class SpeContext : public Spe {
    private:
        ctk_spe_context_t m_context;
        ls_addr_t m_pc;
        int m_retStatus;
        int m_stopCode;
        ctk_atomic_ea_t m_counter_ea;

        void release() {
            if (m_counter_ea != 0) {
                if (_ctk_atomic_dec(m_counter_ea) == 1) {
                    // we were the last one
                    ::ctk_spe_context_destroy(m_context);
                }
            }
        }

    protected:
        // implement
        virtual const ctk_spe_context_t& getInternalContext() const { return m_context; }

    public:
        /** Default constructor
         *  @see #ctk_spe_context_create
         *  @throws #Error on error
         */
        SpeContext(unsigned int flags = 0, void *info = NULL)
            : m_context(NULL), m_pc(CTK_SPE_DEFAULT_ENTRY),
              m_retStatus(0), m_stopCode(0)
        {
            int ret = ::ctk_spe_context_create(&m_context, flags, info);
            if (ret != CTK_SUCCESS)
                // ctk_spe_context_create should clean up any memory
                // on failure, so we can just throw an Exception
                throw Error(ret);
            m_counter_ea = CTK_ATOMIC_EA(m_context->m_counter);
            _ctk_atomic_inc(m_counter_ea);
        }

        /** Constructor */
        SpeContext(ctk_spe_context_t &context)
            : m_context(context), m_pc(CTK_SPE_DEFAULT_ENTRY),
              m_retStatus(0), m_stopCode(0), m_counter_ea(0)
        {
        }

        /** Copy constructor */
        SpeContext(const SpeContext &ctx)
            : m_context(ctx.m_context), m_pc(ctx.m_pc),
              m_retStatus(0), m_stopCode(0),
              m_counter_ea(ctx.m_counter_ea)
        {
            if (m_counter_ea != 0)
                _ctk_atomic_inc(m_counter_ea);
        }

        /** Assignment operator */
        SpeContext& operator=(const SpeContext& ctx)
        {
            if (m_context == ctx.m_context)
                return *this;

            release();

            m_context = ctx.m_context;
            m_pc = ctx.m_pc;
            m_retStatus = m_stopCode = 0;
            m_counter_ea = ctx.m_counter_ea;
            if (m_counter_ea != 0)
                _ctk_atomic_inc(m_counter_ea);
        }

        /** Destructor */
        virtual ~SpeContext() {
            release();
        }

        /** Loads the given SPE image
         *  @see #ctk_spe_context_load
         *  @throws #Error on error
         */
        virtual void load(SpeImage &image) {
            int ret = ::ctk_spe_context_load(m_context, image.getProgram());
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /** Loads the given SPE program handle
         *  @see #ctk_spe_context_load
         *  @throws #Error on error
         */
        virtual void load(ctk_spe_program_t &program) {
            int ret = ::ctk_spe_context_load(m_context, &program);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /** Loads the given SPE program file
         *  @see #ctk_spe_context_load
         *  @see #SpeImage
         *  @throws #Error on error
         */
        virtual void load(const char *fname) {
            SpeImage image(fname);
            int ret = ::ctk_spe_context_load(m_context, image.getProgram());
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /** Runs the SPE context
         *  @see #ctk_spe_context_run
         *  @throws #Error on error
         */
        virtual void run(ls_addr_t *entry, unsigned int runflags,
                    void *argp, void *envp, int *status, int *stop_code)
        {
            int ret = ::ctk_spe_context_run(m_context, entry, runflags, 
                            argp, envp, &m_retStatus, &m_stopCode);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_pc = *entry;
            if (status != NULL) *status = m_retStatus;
            if (stop_code != NULL) *stop_code = m_stopCode;
        }

        /** Runs the SPE context
         *  @see #ctk_spe_context_run
         *  @throws #Error on error
         */
        virtual void run(void *argp = NULL, void *envp = NULL)
        {
            int ret = ::ctk_spe_context_run(m_context, &m_pc, 0, 
                            argp, envp, &m_retStatus, &m_stopCode);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /** Runs the SPE context
         *  @see #ctk_spe_context_run
         *  @throws #Error on error
         */
        virtual void run(ls_addr_t &entry, void *argp = NULL, void *envp = NULL)
        {
            int ret = ::ctk_spe_context_run(m_context, &entry, 0, 
                            argp, envp, &m_retStatus, &m_stopCode);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_pc = entry;
        }

        /** Stops the SPE context
         *  @see #ctk_spe_context_stop
         */
        virtual void stop() {
            ::ctk_spe_context_stop(m_context);
        }

        /** Returns the SPE context ID */
        virtual unsigned long long getID() const {
            return CTK_SPE_ID(m_context);
        }

        /** Returns the current program counter */
        ls_addr_t getPC() const { return m_pc; }

        /** Sets a new program counter */
        void setPC(ls_addr_t pc) { m_pc = pc; }

        /** Returns the last stop code */
        int getStopCode() const { return m_stopCode; }

        /** Returns the last exit code */
        int getExitCode() const { return CTK_SPE_EXIT_STATUS(m_retStatus); }

        /** Returns true if the last run exited normally */
        bool isExited() const { return CTK_SPE_IF_EXITED(m_retStatus); }
}; // SpeContext class

}; // namespace ctk

#endif /* _CTK_SPE_CONTEXT_HPP */
