#ifndef _LIS3DSH_MANAGER_H
#define _LIS3DSH_MANAGER_H

/**
 *  \brief Initialize LIS3DSH
 */
void LIS3DSH_init();
/**
 *  \brief Configure interrupt
 */
void LIS3DSH_interrupt_config();
/**
 *  \brief Configure the LIS3DSH state machine.
 */
void LIS3DSH_click_sm_config();

/**
 * \brief Initialize the SPI peripheral.
 */
void spi_init();
/**
 *  \brief Read the data from the specified address.
 *  \param address The address to read from
 *  \return The data contained at the specific address.
 */
unsigned char spi_get_data(unsigned char address);
/**
 *  \brief Writes 1 byte to the accelerometer through SPI
 *  \param byte The data to be written.
 *  \param address The address to write into
 */
void spi_send_data(unsigned char byte, unsigned char address);
/**
 *  \brief Sends 1 byte to the accelerometer through SPI and returns the value
 *  \param byte The data to be written, as a char
 *  \return a char containing the corresponding byte received during the same
 * full-duplex communication.
 */
unsigned char spi_send_recv(unsigned char byte);
#endif
