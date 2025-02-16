/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//
// Parallel ATA (PATA) controller driver
//
// This driver describes a logical PATA Channel. Each channel can connect up to 2
// IDE Hard Disk Drives. The drives themselves can be either the master drive (hd0)
// or the slave drive (hd1).
//
// More information about the ATA spec for PATA can be found here:
//      ftp://ftp.seagate.com/acrobat/reference/111-1c.pdf
//

#pragma once

#include <AK/OwnPtr.h>
#include <AK/RefPtr.h>
#include <Kernel/Devices/Device.h>
#include <Kernel/IO.h>
#include <Kernel/Lock.h>
#include <Kernel/PCI/Access.h>
#include <Kernel/PCI/Device.h>
#include <Kernel/PhysicalAddress.h>
#include <Kernel/Random.h>
#include <Kernel/VM/PhysicalPage.h>
#include <Kernel/WaitQueue.h>

namespace Kernel {

class AsyncBlockDeviceRequest;

struct PhysicalRegionDescriptor {
    PhysicalAddress offset;
    u16 size { 0 };
    u16 end_of_table { 0 };
};

class PATADiskDevice;
class PATAChannel final : public PCI::Device {
    friend class PATADiskDevice;
    AK_MAKE_ETERNAL
public:
    enum class ChannelType : u8 {
        Primary,
        Secondary
    };

public:
    static OwnPtr<PATAChannel> create(ChannelType type, bool force_pio);
    PATAChannel(PCI::Address address, ChannelType type, bool force_pio);
    virtual ~PATAChannel() override;

    RefPtr<PATADiskDevice> master_device() { return m_master; };
    RefPtr<PATADiskDevice> slave_device() { return m_slave; };

    virtual const char* purpose() const override { return "PATA Channel"; }

private:
    //^ IRQHandler
    virtual void handle_irq(const RegisterState&) override;

    void initialize(bool force_pio);
    void detect_disks();

    void start_request(AsyncBlockDeviceRequest&, bool, bool);
    void complete_current_request(AsyncDeviceRequest::RequestResult);

    void ata_read_sectors_with_dma(bool);
    void ata_read_sectors(bool);
    bool ata_do_read_sector();
    void ata_write_sectors_with_dma(bool);
    void ata_write_sectors(bool);
    void ata_do_write_sector();

    // Data members
    u8 m_channel_number { 0 }; // Channel number. 0 = master, 1 = slave
    IOAddress m_io_base;
    IOAddress m_control_base;
    volatile u8 m_device_error { 0 };

    PhysicalRegionDescriptor& prdt() { return *reinterpret_cast<PhysicalRegionDescriptor*>(m_prdt_page->paddr().offset(0xc0000000).as_ptr()); }
    RefPtr<PhysicalPage> m_prdt_page;
    RefPtr<PhysicalPage> m_dma_buffer_page;
    IOAddress m_bus_master_base;
    Lockable<bool> m_dma_enabled;
    EntropySource m_entropy_source;

    RefPtr<PATADiskDevice> m_master;
    RefPtr<PATADiskDevice> m_slave;

    AsyncBlockDeviceRequest* m_current_request { nullptr };
    u32 m_current_request_block_index { 0 };
    bool m_current_request_uses_dma { false };
    bool m_current_request_flushing_cache { false };
    SpinLock<u8> m_request_lock;
};
}
