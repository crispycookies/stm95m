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

#ifndef _STM95M_H
#define _STM95M_H

#include <stdint.h>
#include <stddef.h>

#define MAX_BURST_SIZE 62

/**
 * Return values
 */
typedef enum {
    MEMORY_STATUS_OK = 0,
    MEMORY_STATUS_NOK,
    MEMORY_STATUS_INVALID_HANDLE
} memory_status_t;

/**
 * Provides abstraction for SPI communication with ST95M memory
 */
typedef struct {
    void *low_level_handle;

    memory_status_t (*read)(void *handle, uint8_t *data, uint32_t length);

    memory_status_t (*write)(void *handle, const uint8_t *data, uint32_t length);

    memory_status_t (*cs_enable)(void *handle, size_t cs);

    memory_status_t (*cs_disable)(void *handle, size_t cs);
} stm95m_handle_t;

/**
 * @brief Reads data from the ST95M memory.
 * @param handle The stm95m_handle_t to use
 * @param address The address to read from
 * @param data The data buffer to read into
 * @param length The length of the data buffer
 * @param cs The chip select to use
 * @return MEMORY_STATUS_OK on success, MEMORY_STATUS_NOK on failure
 */
memory_status_t stm95m_read(stm95m_handle_t *handle, uint32_t address, uint8_t *data, uint32_t length, size_t cs);

/**
 * @brief Writes data to the ST95M memory.
 * @param handle The stm95m_handle_t to use
 * @param address The address to write to
 * @param data The data buffer to write
 * @param length The length of the data buffer
 * @param cs The chip select to use
 * @return MEMORY_STATUS_OK on success, MEMORY_STATUS_NOK on failure
 */
memory_status_t stm95m_write(stm95m_handle_t *handle, uint32_t address, const uint8_t *data, uint32_t length, size_t cs);

memory_status_t stm95m_write_safe(stm95m_handle_t *handle, uint32_t address, const uint8_t *data, uint32_t length, size_t cs);

/**
 * @brief Reads the status register of the ST95M memory.
 * @param handle The handle to use
 * @param data The data to read into
 * @param cs The chip select to use
 * @return MEMORY_STATUS_OK on success, MEMORY_STATUS_NOK on failure
 */
memory_status_t stm95m_read_register(stm95m_handle_t *handle, uint8_t *data, size_t cs);

/**
 * @brief Writes into the status register of the ST95M memory.
 * @param handle The handle to use
 * @param data The data to write
 * @param cs The chip select to use
 * @return MEMORY_STATUS_OK on success, MEMORY_STATUS_NOK on failure
 */
memory_status_t stm95m_write_register(stm95m_handle_t *handle, uint8_t data, size_t cs);

#endif //_STM95M_H
