#include "lis3dsh_usage.h"
#include "lis3dsh_addresses.h"
#include <miosix.h>
#include <util/software_spi.h>

using namespace miosix;

/*!< SPI clock enable on PE.3*/
typedef Gpio<GPIOE_BASE, 3> spi_ce;
/*!< SPI clock on PA.5*/
typedef Gpio<GPIOA_BASE, 5> spi_sck;
/*!< SPI input on PA.7*/
typedef Gpio<GPIOA_BASE, 7> spi_si;
/*!< SPI output on PA.6*/
typedef Gpio<GPIOA_BASE, 6> spi_so;
/*!< SoftwareSPI miosix class*/
typedef SoftwareSPI<spi_si, spi_so, spi_sck, spi_ce, 10> spi_c;

/**
 *  \brief Writes 1 byte to the accelerometer through SPI
 *
 *  \param data the data to be written
 *  \param writeAddrr the address to write into
 *  \return void
 */
void LIS3DSH_write(unsigned char data, unsigned char writeAddr) {
  spi_c::sendRecvChar(writeAddr);
  spi_c::sendRecvChar(data);
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
  // Initialize accelerometer SPI
  spi_c::init();

  /*
    Output data rate = 0b0110= 100Hz
    Block data update: continuous update
    Enable zen, yen, xen (z, y, x axes)
    unction description
   */
  LIS3DSH_write(0b01100111, LIS3DSH_CTRL_REG4_ADDR);

  /* Write value to MEMS CTRL_REG5 register
    All defaults:
    Antialiasing 800 Hz
    Self-test disabled
    +- 2g
  */
  LIS3DSH_write(0b0, LIS3DSH_CTRL_REG5_ADDR);
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
  //spi_int2::init();

  EXTI->IMR |= EXTI_IMR_MR1; //
  // listen to raising edge trigger
  EXTI->RTSR |= EXTI_RTSR_TR1;
  // configure the interrupt controller in order to pass the interrupt request
  // up to the CPU
  NVIC_EnableIRQ(EXTI1_IRQn);
  // Set the interrupt priority to 15 (low)
  NVIC_SetPriority(EXTI1_IRQn, 15);
  /* Write value to MEMS CTRL_REG3 register
     Interrupt signal active high
     Interrupt signal latched
     Interrupt1 enabled
     Interrupt2 disabled
  */
  LIS3DSH_write(0b01001000, LIS3DSH_CTRL_REG3_ADDR);

  /* Configure State Machine 1 */
  /* Write value to MEMS CTRL_REG1 register
     SM1 Enable; SM1 interrupt routed (by default) to INT1; hysteris 0
  */
  LIS3DSH_write(0b1001, LIS3DSH_CTRL_REG1_ADDR);
}
/**
 *  \brief Configure click interrupt
 *
 *  Configure the states of the State Machine 1 (SM1) of the LIS3DSH in order to
 *  recognize single click events.
 *
 *  \return void
 */
void LIS3DSH_click_int_config() {
  LIS3DSH_interrupt_config();

  /* Set LIS3DSH State Machines configuration */
  // configure general timer TC1 for SM1
  // Register TIMx_1 (x=1..4) define the initial value of the timer counter TC1
  LIS3DSH_write(0x02, LIS3DSH_TIM4_1_ADDR);
  LIS3DSH_write(0x01, LIS3DSH_TIM3_1_ADDR);
  LIS3DSH_write(0x32, LIS3DSH_TIM2_1_L_ADDR);
  LIS3DSH_write(0x07, LIS3DSH_TIM1_1_L_ADDR);

  // threshold values for sm1
  LIS3DSH_write(0x55, LIS3DSH_THRS2_1_ADDR);
  LIS3DSH_write(0x55, LIS3DSH_THRS1_1_ADDR);

  // SM1 code registers
  // ST1_x are the different status of the state machine 1

  /* 0b 0101 0001
     GNTH1 - TI1
   */
  LIS3DSH_write(0x51, LIS3DSH_ST1_1_ADDR);
  // 0b 0101 0001
  // as before
  LIS3DSH_write(0x51, LIS3DSH_ST1_2_ADDR);
  // 0b 0000 0110
  /* NOP - GNTH2
   */
  LIS3DSH_write(0x06, LIS3DSH_ST1_3_ADDR);
  /* 0b 0011 1000
     TI3 - LNTH2
     GNTH1
   */
  LIS3DSH_write(0x38, LIS3DSH_ST1_4_ADDR);
  /*
    The following was used for double-click
   */
  // /* 0b 0000 0100
  //    NOP - TI4
  //  */
  // LIS3DSH_write(0x04, LIS3DSH_ST1_5_ADDR);
  // /* 0b 1001 0001
  //    GTTH1 - TI1
  //  */
  // LIS3DSH_write(0x91, LIS3DSH_ST1_6_ADDR);
  // /* 0b 0010 0110
  //    TI2 - GNTH2
  //  */
  // LIS3DSH_write(0x26, LIS3DSH_ST1_7_ADDR);
  // /* 0b 0011 1000
  //    TI3 - LNTH2
  //  */
  // LIS3DSH_write(0x38, LIS3DSH_ST1_8_ADDR);
  // /* 0b 0000 0100
  //    NOP - TI4
  //  */
  // LIS3DSH_write(0x04, LIS3DSH_ST1_9_ADDR);
  // /* 0b 1001 0001
  //    GTTH1 - TI1
  //  */
  // LIS3DSH_write(0x91, LIS3DSH_ST1_10_ADDR);
  /* 0b 0001 0001
     TI1 - TI1
   */
  // LIS3DSH_write(0x11, LIS3DSH_ST1_11_ADDR);
  LIS3DSH_write(0x11, LIS3DSH_ST1_5_ADDR);
  // machine 1: listen to xyz axis
  LIS3DSH_write(0b11111100, LIS3DSH_MASK1_A_ADDR);
  LIS3DSH_write(0b10100001, LIS3DSH_SETT1_ADDR);
}
