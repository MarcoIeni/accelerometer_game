#include "lis3dsh_manager.h"
#include "lis3dsh_addresses.h"
#include <miosix.h>
#include <stdio.h>

using namespace miosix;

/*!< SPI clock enable (CE or CS) on PE.3*/
typedef Gpio<GPIOE_BASE, 3> spi_ce;
/*!< SPI clock on PA.5*/
typedef Gpio<GPIOA_BASE, 5> spi_sck;
/*!< SPI input on PA.7*/
typedef Gpio<GPIOA_BASE, 7> spi_si;
/*!< SPI output on PA.6*/
typedef Gpio<GPIOA_BASE, 6> spi_so;

/**
 *  \brief Sends 1 byte to the accelerometer through SPI and returns the value
 *
 *  \param data the data to be written
 *  \param address the address to write into
 *  \return void
 */
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

/**
 *  \brief Writes 1 byte to the accelerometer through SPI
 *
 *  \param data the data to be written
 *  \param address the address to write into
 *  \return void
 */
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
  SPI1->CR1 = 1;           //  CPHA=1
  SPI1->CR1 |= 1 << 1;     // CPOL=1,
  SPI1->CR1 |= 1 << 2;     // Master Mode
  SPI1->CR1 |= 0b001 << 5; // Baud Rate = maximum frequency/32 (i.e. 10 Mhz/32)
  SPI1->CR1 |= 1 << 8;     // SSI enabled
  SPI1->CR1 |= 1 << 9;     // SSM enabled
  SPI1->CR1 |= 1 << 6;     //	SPI enabled
}

/**
 *  \brief Initialize LIS3DSH
 *
 *  First initialize accelerometer SPI.
 *  Then write to MEMS CTRL_REG4 and MEMS CTRL_REG5 registers
 *
 *  \return return void
 */
void LIS3DSH_init() {
  /*
    Addr: 0x20
    Output data rate = 0b0110= 100Hz
    Block data update: continuous update
    Enable zen, yen, xen (z, y, x axes)
   */
  spi_send_data(0b01100111, LIS3DSH_CTRL_REG4_ADDR);

  /* Write value to MEMS CTRL_REG5 register
    All defaults:
    Antialiasing 800 Hz
    Self-test disabled
    +- 2g
  */
  spi_send_data(0b0, LIS3DSH_CTRL_REG5_ADDR);
}

/**
 *  \brief Configure interrupt
 *
 *  \return void
 */
void LIS3DSH_interrupt_config() {
  /*
    Configure accelerometer INT2
  */
  // TODO: move to interrupt manager

  EXTI->IMR |= EXTI_IMR_MR1; //
  // listen to raising edge trigger
  EXTI->RTSR |= EXTI_RTSR_TR1;
  // configure the interrupt controller in order to pass the interrupt request
  // up to the CPU
  NVIC_EnableIRQ(EXTI1_IRQn);
  // Set the interrupt priority to 14 (low)
  NVIC_SetPriority(EXTI1_IRQn, 14);
  /* Write value to MEMS CTRL_REG3 register (Address 0x23)
     Interrupt signal active high
     Interrupt signal latched
     Interrupt1 disabled
     Interrupt2 enabled
  */
  spi_send_data(0b01010000, LIS3DSH_CTRL_REG3_ADDR);

  /* Configure State Machine 1 */
  /* Write value to MEMS CTRL_REG1 register
     SM1 Enable; SM1 interrupt routed (by default) to INT1; hysteris 0
  */
  spi_send_data(0b00001001, LIS3DSH_CTRL_REG1_ADDR);
}
/**
 *  \brief Configure click interrupt
 *
 *  Configure the states of the State Machine 1 (SM1) of the LIS3DSH in order to
 *  recognize single click events.
 *
 *  \return void
 */
void LIS3DSH_click_sm_config() {

  /* Set LIS3DSH State Machines configuration */
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
  /*
    The following was used for double-click
   */
  /* 0b 0000 0100
     NOP - TI4
   */
  spi_send_data(0x04, LIS3DSH_ST1_5_ADDR);
  /* 0b 1001 0001
     GTTH1 - TI1
   */
  spi_send_data(0x91, LIS3DSH_ST1_6_ADDR);
  /* 0b 0010 0110
     TI2 - GNTH2
   */
  spi_send_data(0x26, LIS3DSH_ST1_7_ADDR);
  /* 0b 0011 1000
     TI3 - LNTH2
   */
  spi_send_data(0x38, LIS3DSH_ST1_8_ADDR);
  /* 0b 0000 0100
     NOP - TI4
   */
  spi_send_data(0x04, LIS3DSH_ST1_9_ADDR);
  /* 0b 1001 0001
     GTTH1 - TI1
   */
  spi_send_data(0x91, LIS3DSH_ST1_10_ADDR);
  /*
    0b 0001 0001
    TI1 - TI1
  */
  spi_send_data(0x11, LIS3DSH_ST1_11_ADDR);
  // machine 1: listen to xyz axis
  spi_send_data(0b11111100, LIS3DSH_MASK1_A_ADDR);
  spi_send_data(0b10100001, LIS3DSH_SETT1_ADDR);
}
