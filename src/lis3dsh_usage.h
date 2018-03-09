#ifndef _LIS3DSH_USAGE_H
#define _LIS3DSH_USAGE_H

// init values CTRL_REG4 and CTRL_REG5
#define LIS3DSH_DATARATE_100 ((unsigned char)0x60) /* 100   Hz Normal Mode */
#define LIS3DSH_XYZ_ENABLE ((unsigned char)0x07)   /* zen=1, yen=1, xen=1 */
#define LIS3DSH_SERIALINTERFACE_4WIRE ((unsigned char)0x00)
#define LIS3DSH_SELFTEST_NORMAL ((unsigned char)0x00)
#define LIS3DSH_FULLSCALE_2 ((unsigned char)0x00)   /* 2 g  */
#define LIS3DSH_FILTER_BW_800 ((unsigned char)0x00) /* 800 Hz */

// init values CTRL_REG1, CTRL_REG2 and CTRL_REG5
#define LIS3DSH_INTERRUPT_2_ENABLE ((unsigned char)0x00)
#define LIS3DSH_INTERRUPT_REQUEST_LATCHED ((unsigned char)0x00)
#define LIS3DSH_INTERRUPT_SIGNAL_HIGH ((unsigned char)0x40)
#define LIS3DSH_SM_ENABLE ((unsigned char)0x01)
#define LIS3DSH_SM_DISABLE ((unsigned char)0x00)
#define LIS3DSH_SM_INT1 ((unsigned char)0x00)
#define LIS3DSH_SM_INT2 ((unsigned char)0x08)

void LIS3DSH_init();
void LIS3DSH_write(unsigned char data, unsigned char writeAddr);
void LIS3DSH_interrupt_config();
void LIS3DSH_click_int_config();

#endif
