#include "lis3dsh_manager.h"
#include "lis3dsh_addresses.h"
#include <miosix.h>
#include <stdio.h>

using namespace miosix;

/*! SPI clock enable (CE or CS) on PE.3 */
typedef Gpio<GPIOE_BASE, 3> spi_ce;
/*! SPI clock on PA.5 */
typedef Gpio<GPIOA_BASE, 5> spi_sck;
/*! SPI input on PA.7 */
typedef Gpio<GPIOA_BASE, 7> spi_si;
/*! SPI output on PA.6 */
typedef Gpio<GPIOA_BASE, 6> spi_so;

unsigned char spi_send_recv(unsigned char byte) {
  // Wait for TXE
  while (!(SPI1->SR & 0b10))
    ;
  SPI1->DR = byte;
  // Wait for RXNE
  while (!(SPI1->SR & 0b01))
    ;
  return (SPI1->DR);
}

void spi_send_data(unsigned char data, unsigned char address) {
  spi_ce::low();
  spi_send_recv(address);
  spi_send_recv(data);
  spi_ce::high();
}

unsigned char spi_get_data(unsigned char address) {
  unsigned char c;
  spi_ce::low();
  address = 0x80 | address;
  spi_send_recv(address);
  c = spi_send_recv(0x00);
  spi_ce::high();
  return c;
}

/**
 - Enable spi clock to PE3, PA5, PA6 and PA7.
 - Set PE3 mode to output
 - Set PA5, PA6, PA7 to alternate function mode.
 - Enable clock to SPI1
 - Set the following configuration parameters (all the others to default):
     -# CPHA: 1
     -# CPOL: 1,
     -# Master Mode
     -# Baud Rate: maximum frequency/32 (i.e. 10 Mhz/32)
     -# SSI enabled
     -# SSM enabled
     -# SPI enabled
 */
void spi_init() {
  // Enable spi clock to PE3, PA5, PA6 and PA7.
  // Set PE3 mode to output
  spi_ce::mode(Mode::OUTPUT);
  spi_ce::high();

  // Set PA5, PA6, PA7 to alternate function mode.
  spi_si::mode(Mode::ALTERNATE);
  spi_si::alternateFunction(5);
  spi_so::mode(Mode::ALTERNATE);
  spi_so::alternateFunction(5);
  spi_sck::mode(Mode::ALTERNATE);
  spi_sck::alternateFunction(5);

  // Enable clock to SPI1
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  SPI1->CR1 = 1;           // CPHA=1
  SPI1->CR1 |= 1 << 1;     // CPOL=1,
  SPI1->CR1 |= 1 << 2;     // Master Mode
  SPI1->CR1 |= 0b001 << 5; // Baud Rate = maximum frequency/32 (i.e. 10 Mhz/32)
  SPI1->CR1 |= 1 << 8;     // SSI enabled
  SPI1->CR1 |= 1 << 9;     // SSM enabled
  SPI1->CR1 |= 1 << 6;     //	SPI enabled
}

/**
 * Write:
 *  -  to MEMS CTRL_REG4 register:
 *    -# Output data rate: 100 Hz
 *    -# Block data update: Continuous update
 *    -# Enable xen, yen, zen (x, y, z) axes
 *  - to MEMS CTRL_REG5 register
 *    -# Anti-aliasing filter bandwidth: 800Hz
 *    -# Full scale selection: +- 2g
 *    -# Self-test: disabled
 * All other values are left to default ones.
 *
 */
void LIS3DSH_init() {
  spi_send_data(0b01100111, LIS3DSH_CTRL_REG4_ADDR);
  spi_send_data(0b0, LIS3DSH_CTRL_REG5_ADDR);
}

/* Write
   - to MEMS CTRL_REG3 register:
   -# Interrupt signal active high
   -# Interrupt signal latched
   -# Interrupt1 disabled
   -# Interrupt2 enabled
   - to MEMS CTRL_REG1 register:
   -# State machine 1 enabled
   -# State machine 1 interrupt routed to INT2
   -# Hysteresis: 0
*/
void LIS3DSH_interrupt_config() {
  spi_send_data(0b01010000, LIS3DSH_CTRL_REG3_ADDR);
  spi_send_data(0b00001001, LIS3DSH_CTRL_REG1_ADDR);
}

/**
 Configure the states of the State Machine 1 (SM1) of the LIS3DSH in order to
 recognise single click (tap) events.
 The state machine was configured starting from the application note provided
 by ST for the LIS3DSH 3d-axial accelerometer. The original configuration was
 thought for a double tap; we have adapted it to be used for a single tap.

 The first two states (GNTH1 | TI1) are used to create a “pre-silence” time
 window, the NEXT condition is the Timer1. If one or more axes become greater
 than threshold 1, the RESET condition is satisfied and the program pointer is
 reset.

 After that there is the real first tap detection composed of two states, the
 first one is composed of (NOP | GNTH2), where the next condition is satisfied
 when one or more triggered axes exceed threshold 2. In the second state (TI3 |
 LNTH2), it is checked whether the acceleration value on the axis becomes lower
 than threshold 2 within a time defined by Timer 3. The result of these states
 is to create a time window (TI3) where the acceleration is first higher, and
 then lower, than threshold 2.

 \image html single_click_output.png "Single click output"
 */
void LIS3DSH_click_sm_config() {

  // configure general timer TC1 for SM1
  // Register TIMx_1 (x=1..4) define the initial value of the timer counter TC1
  spi_send_data(0x02, LIS3DSH_TIM4_1_ADDR);
  spi_send_data(0x01, LIS3DSH_TIM3_1_ADDR);
  spi_send_data(0x32, LIS3DSH_TIM2_1_L_ADDR);
  spi_send_data(0x07, LIS3DSH_TIM1_1_L_ADDR);

  // threshold values for sm1
  spi_send_data(0x55, LIS3DSH_THRS2_1_ADDR);
  spi_send_data(0x55, LIS3DSH_THRS1_1_ADDR);
  printf("Sent Thr\n");

  // SM1 code registers
  // ST1_x are the different status of the state machine 1

  /* 0b 0101 0001
     GNTH1 - TI1
   */
  spi_send_data(0x51, LIS3DSH_ST1_1_ADDR);

  // 0b 0101 0001
  // as before
  spi_send_data(0x51, LIS3DSH_ST1_2_ADDR);

  // 0b 0000 0110
  /* NOP - GNTH2
   */
  spi_send_data(0x06, LIS3DSH_ST1_3_ADDR);

  /* 0b 0011 1000
     TI3 - LNTH2
     GNTH1
   */

  spi_send_data(0x38, LIS3DSH_ST1_4_ADDR);

  /* 0b 0000 0011
     NOP - TI4
   */
  spi_send_data(0x11, LIS3DSH_ST1_5_ADDR);

  // machine 1: listen to xyz axis
  spi_send_data(0b11111100, LIS3DSH_MASK1_A_ADDR);
  spi_send_data(0b10100001, LIS3DSH_SETT1_ADDR);
}
