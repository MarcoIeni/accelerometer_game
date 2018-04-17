#ifndef _LIS3DSH_MANAGER_H
#define _LIS3DSH_MANAGER_H

void LIS3DSH_init();
void LIS3DSH_interrupt_config();
void LIS3DSH_click_sm_config();

void spi_init();
unsigned char spi_get_data(unsigned char address);
void spi_send_data(unsigned char byte, unsigned char address);
unsigned char spi_send_recv(unsigned char byte);
#endif
