# STM M95Mxx-Series Driver

### How to use

* Declare a config struct of type ``stm95m_handle_t``.

  ```c
  stm95m_handle_t config;
  ```

* Create callback-functions for read, write, chip-select enable and disable. These callbacks are just wrappers to your platform-depended SPI-driver.

  An example used in one of my projects is given below:

  ```c
  memory_status_t read(void *handle, uint8_t *data, uint32_t length) {
      SPI_Init_Struct *spi = (SPI_Init_Struct *) handle; // When using STM32 Hal, this can also be SPI_HandleTypeDef
      return SPI_ReadData(spi, data, length, 200) == SPI_RET_OK ? MEMORY_STATUS_OK : MEMORY_STATUS_NOK;
  }
  
  memory_status_t write(void *handle, uint8_t *data, uint32_t length) {
      SPI_Init_Struct *spi = (SPI_Init_Struct *) handle; // When using STM32 Hal, this can also be SPI_HandleTypeDef
      return SPI_WriteData(spi, data, length, 200) == SPI_RET_OK ? MEMORY_STATUS_OK : MEMORY_STATUS_NOK;
  }
  
  memory_status_t cs_enable(void *handle, size_t cs) {
      SPI_Init_Struct *spi = (SPI_Init_Struct *) handle; // When using STM32 Hal, this can also be SPI_HandleTypeDef
      return SPI_CS_Enable(&spi->CS[cs]) == SPI_RET_OK ? MEMORY_STATUS_OK : MEMORY_STATUS_NOK;
  }
  
  memory_status_t cs_disable(void *handle, size_t cs) {
      SPI_Init_Struct *spi = (SPI_Init_Struct *) handle; // When using STM32 Hal, this can also be SPI_HandleTypeDef
      return SPI_CS_Disable(&spi->CS[cs]) == SPI_RET_OK ? MEMORY_STATUS_OK : MEMORY_STATUS_NOK;
  }
  ```

* Add these callbacks to the `config`

  The ``low_level_handle`` is a pointer to any data you need for your platform-depended SPI-driver. In case your platform-depended SPI-driver is 	   STM32HAL based, this would be a ptr to ``SPI_HandleTypeDef``.

  ```c
  config.write = write;
  config.read = read;
  config.cs_enable = cs_enable;
  config.cs_disable = cs_disable;
  config.low_level_handle = (void *) &spi;
  ```

* Now you can use the following functions:

  ```c
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
  memory_status_t stm95m_write(stm95m_handle_t *handle, uint32_t address, uint8_t *data, uint32_t length, size_t cs);
  
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
  ```

  
