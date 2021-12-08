/**
 *  Copyright (C) 2021  Tobias Egger
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include "stm95m.h"

#define WREN    0b00000110
#define WRDI    0b00000100
#define RDSR    0b00000101
#define WRSR    0b00000001
#define READ    0b00000011
#define WRITE   0b00000010
#define RDID    0b10000011
#define WRID    0b10000010
#define RDLS    RDID
#define LID     0b10000010

#define WIP     0x01

memory_status_t stm95m_check_handle(const stm95m_handle_t *const handle) {
    if (handle == NULL) {
        return MEMORY_STATUS_INVALID_HANDLE;
    }
    if (handle->read == NULL) {
        return MEMORY_STATUS_INVALID_HANDLE;
    }
    if (handle->cs_disable == NULL) {
        return MEMORY_STATUS_INVALID_HANDLE;
    }
    if (handle->cs_enable == NULL) {
        return MEMORY_STATUS_INVALID_HANDLE;
    }
    if (handle->write == NULL) {
        return MEMORY_STATUS_INVALID_HANDLE;
    }
    return MEMORY_STATUS_OK;
}

memory_status_t stm95m_read(stm95m_handle_t *handle, uint32_t address, uint8_t *data, uint32_t length, size_t cs) {
    memory_status_t rc = stm95m_check_handle(handle);
    if (rc != MEMORY_STATUS_OK) {
        return rc;
    }

    uint8_t header[4] = {0};
    header[0] = READ;
    header[1] = address >> 16;
    header[2] = address >> 8;
    header[3] = address;

    handle->cs_enable(handle->low_level_handle, cs);
    if (handle->write(handle->low_level_handle, header, sizeof header) != MEMORY_STATUS_OK) {
        handle->cs_disable(handle->low_level_handle, cs);
        return MEMORY_STATUS_NOK;
    }
    rc = handle->read(handle->low_level_handle, data, length);
    handle->cs_disable(handle->low_level_handle, cs);

    return rc;
}

memory_status_t stm95m_write_latch_enable(stm95m_handle_t *handle, size_t cs) {
    uint8_t enable = WREN;

    handle->cs_enable(handle->low_level_handle, cs);
    memory_status_t rc = handle->write(handle->low_level_handle, &enable, 1);
    handle->cs_disable(handle->low_level_handle, cs);

    return rc;
}

memory_status_t stm95m_write_latch_disable(stm95m_handle_t *handle, size_t cs) {
    uint8_t disable = WRDI;

    handle->cs_enable(handle->low_level_handle, cs);
    memory_status_t rc = handle->write(handle->low_level_handle, &disable, 1);
    handle->cs_disable(handle->low_level_handle, cs);

    return rc;
}

memory_status_t stm95m_wait_wip_completed(stm95m_handle_t *handle, size_t cs) {
    uint8_t wip = WIP;
    while (wip & WIP) {
        if (stm95m_read_register(handle, &wip, cs) != MEMORY_STATUS_OK) {
            return MEMORY_STATUS_NOK;
        }
    }
    return MEMORY_STATUS_OK;
}

memory_status_t stm95m_write(stm95m_handle_t *handle, uint32_t address, const uint8_t *data, uint32_t length, size_t cs) {
    memory_status_t rc = stm95m_check_handle(handle);
    if (rc != MEMORY_STATUS_OK) {
        return rc;
    }

    uint8_t header[4] = {0};
    header[0] = WRITE;
    header[1] = address >> 16;
    header[2] = address >> 8;
    header[3] = address;

    // Reset WIP-bit in status register
    uint8_t wip;
    stm95m_read_register(handle, &wip, cs);
    wip &= ~ WIP;
    stm95m_write_register(handle, wip, cs);

    if (stm95m_write_latch_enable(handle, cs) != MEMORY_STATUS_OK) {
        return MEMORY_STATUS_NOK;
    }

    handle->cs_enable(handle->low_level_handle, cs);

    // Send the header
    if (handle->write(handle->low_level_handle, header, sizeof header) != MEMORY_STATUS_OK) {
        // If sending the header fails, we need to disable the latch and chip select
        handle->cs_disable(handle->low_level_handle, cs);
        stm95m_write_latch_disable(handle, cs);
        return MEMORY_STATUS_NOK;
    }

    // Send the data
    rc = handle->write(handle->low_level_handle, data, length);
    handle->cs_disable(handle->low_level_handle, cs);

    if (stm95m_write_latch_disable(handle, cs) != MEMORY_STATUS_OK) {
        return MEMORY_STATUS_NOK;
    }

    // wait for the write to complete
    if (stm95m_wait_wip_completed(handle, cs) != MEMORY_STATUS_OK) {
        return MEMORY_STATUS_NOK;
    }

    return rc;
}

memory_status_t stm95m_read_register(stm95m_handle_t *handle, uint8_t *data, size_t cs) {
    memory_status_t rc = stm95m_check_handle(handle);
    if (rc != MEMORY_STATUS_OK) {
        return rc;
    }

    uint8_t read_reg = RDSR;

    handle->cs_enable(handle->low_level_handle, cs);
    if (handle->write(handle->low_level_handle, &read_reg, 1) != MEMORY_STATUS_OK) { ;
        handle->cs_disable(handle->low_level_handle, cs);
        return MEMORY_STATUS_NOK;
    }
    rc = handle->read(handle->low_level_handle, data, 1);
    handle->cs_disable(handle->low_level_handle, cs);

    return rc;
}

memory_status_t stm95m_write_register(stm95m_handle_t *handle, uint8_t data, size_t cs) {
    uint8_t write_reg[] = {WRSR, data};

    memory_status_t rc = stm95m_check_handle(handle);
    if (rc != MEMORY_STATUS_OK) {
        return rc;
    }

    handle->cs_enable(handle->low_level_handle, cs);
    rc = handle->write(handle->low_level_handle, write_reg, sizeof write_reg);
    handle->cs_disable(handle->low_level_handle, cs);

    return rc;
}

