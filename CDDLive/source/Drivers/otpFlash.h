#ifndef __OTP_FLASH
#define __OTP_FLASH

#include <stdint.h>
#include "LOUD_types.h"


bool setMACAddressIntoOTP( const uint8_t *pMac );
void readMACAddressFromOTP( uint8_t *pMac );
bool ValidMAC( uint8_t *pMac );


#endif // __OTP_FLASH

