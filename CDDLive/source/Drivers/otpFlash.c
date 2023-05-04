/**************************************************************************//**
 * @file     otpFlash.c
 * @version  V1.00
 * @brief    Handles Read and Write for the OTP (One Time Programmable) Flash
 *           region. 
 *           Currently we just make use of the first OTP Flash lockable area (8 bytes),
 *           which we put the MAC address into.
 *
*****************************************************************************/
#include "otpFlash.h"
#include "M480.h"
#include <string.h>
#include "Upgrade.h"

const uint32_t OTP_MAC_ADDRESS_LOC = 0;	// First location in OTP used for MAC (8 bytes)


//--------------------------------------------------------------
// @brief   Program MAC address into OTP Flash area.
// @param   mac     - MAC address to program into OTP Flash
// @return  true = OTP programmed ok (or value already set),  false = OTP programming not possible.
//--------------------------------------------------------------
bool setMACAddressIntoOTP( const uint8_t *pMac )
{
	bool retCode = true;
	uint32_t lowWord, highWord;
    SYS_UnlockReg();                   /* Unlock protected registers */
    FMC_Open();                        /* Enable FMC ISP function */

    if (FMC_Read_OTP(OTP_MAC_ADDRESS_LOC, &lowWord, &highWord) != 0)
    {
        printf("Read OTP%d failed!\n");
        retCode = false;
    }
    else if ( (lowWord == 0xFFFFFFFF) && (highWord == 0xFFFFFFFF))
    {	// OTP Available for programming
    	uint32_t lsWord = ((pMac[2]&0xFF) << 24) | ((pMac[3]&0xFF) << 16) | ((pMac[4]&0xFF) << 8) | (pMac[5] & 0xFF);
    	uint32_t msWord = ((pMac[0]&0xFF) << 8) | (pMac[1] & 0xFF);
        printf("Writing OTP %08X, %08X\n", lsWord, msWord);
        if (FMC_Write_OTP(OTP_MAC_ADDRESS_LOC, lsWord, msWord) != 0)
        {
            printf("Failed to program MAC address into OTP Flash\n");
            retCode = false;
        }
        else if (FMC_Read_OTP(OTP_MAC_ADDRESS_LOC, &lowWord, &highWord) != 0)
        {	// Verify read failed
            printf("Read OTP failed after programming\n");
            retCode = false;
        }
        else if ( (lowWord != lsWord) || (highWord != msWord) )
        {	// Verify Failed
            printf("OTP Verify Failed\n");
            retCode = false;
        }
        else if (FMC_Lock_OTP(OTP_MAC_ADDRESS_LOC) != 0)
        {	// Locking OTP failed
            printf("Failed to lock OTP\n");
            retCode = false;
        }
        else if (FMC_Read_OTP(OTP_MAC_ADDRESS_LOC, &lowWord, &highWord) != 0)
        {   // Verify read Failed after Lock
            printf("Read OTP failed after locked\n");
            retCode = false;
        }
        else if ( (lowWord != lsWord) || (highWord != msWord) )
        {	// Verify Failed after Locking
            retCode = false;
        }
    }
    else
    {	// OTP MAC is already programmed.
    	uint32_t lsWord = ((pMac[2]&0xFF) << 24) | ((pMac[3]&0xFF) << 16) | ((pMac[4]&0xFF) << 8) | (pMac[5] & 0xFF);
    	uint32_t msWord = ((pMac[0]&0xFF) << 8) | (pMac[1] & 0xFF);
    	if ( (lowWord == lsWord) && (highWord == msWord) )
    		retCode = true;		// OTP MAC matches what we desire.
    	else
    		retCode = false;	// OTP MAC not set to what we want
    }

    FMC_Close();					 /* Disable FMC ISP function */
    SYS_LockReg();                   /* Unlock protected registers */
    return retCode;
}

//--------------------------------------------------------------
// @brief   Read MAC address from OTP Flash area.
//          If not able or invalid then will use Data Flash MAC
//          or Default MAC (in that order)
// @param   mac     - Storage for MAC address read from OTP Flash
//--------------------------------------------------------------
void readMACAddressFromOTP( uint8_t *pMac )
{
	uint8_t mac[6];
	bool retCode = true;
	uint32_t lowWord, highWord;
    SYS_UnlockReg();                   /* Unlock protected registers */
    FMC_Open();                        /* Enable FMC ISP function */

#if 0 //	IQ - NOT CHECKING OTP
    if (FMC_Read_OTP(OTP_MAC_ADDRESS_LOC, &lowWord, &highWord) != 0)
    {
        printf("Read OTP failed!\n");
        retCode = false;
    }
    else if ( (lowWord == 0xFFFFFFFF) && (highWord == 0xFFFFFFFF))
    {	// OTP MAC has not been programmed yet
    	retCode = false;
    }
    else
    {
    	mac[0] = (highWord >> 8) & 0xFF;
    	mac[1] = highWord & 0xFF;
    	mac[2] = (lowWord >> 24) & 0xFF;
    	mac[3] = (lowWord >> 16) & 0xFF;
    	mac[4] = (lowWord >> 8) & 0xFF;
    	mac[5] = lowWord & 0xFF;
		if ( ValidMAC(mac) )
		{
    		printf( "Valid MAC in OTP Flash : " );
		}
		else
		{
			retCode = false;
		}
	}
	
	if ( retCode == false )
    {   // No valid MAC in OTP Flash, so next try OLY_IDENTITY block
#endif//	IQ - NOT CHECKING OTP
    	memcpy(mac, OLY_IDENTITY_LOCATION->mac, sizeof(mac));

    	//	Needs to  move inside Config object
    	if ( ValidMAC(mac) )
    	{
        	printf( "Valid MAC in DATA Flash : " );
    	}
    	else
    	{   // Use DEFAULT MAC
    	    mac[0] = OLY_DEFAULT_MAC_ADDR0;
    	    mac[1] = OLY_DEFAULT_MAC_ADDR1;
    	    mac[2] = OLY_DEFAULT_MAC_ADDR2;
    	    mac[3] = OLY_DEFAULT_MAC_ADDR3;
    	    mac[4] = OLY_DEFAULT_MAC_ADDR4;
    	    mac[5] = OLY_DEFAULT_MAC_ADDR5;
    		printf( "Using DEFAULT MAC : " );
    	}
	//}	IQ - NOT CHECKING OTP

	// Return the MAC to the user
	printf("%02x:%02x:%02x:%02x:%02x:%02x\r\n",	mac[0],	mac[1], mac[2], mac[3], mac[4], mac[5] );
	memcpy( pMac, mac, sizeof(mac) );

    FMC_Close();					 /* Disable FMC ISP function */
    SYS_LockReg();                   /* Lock protected registers */
}

bool ValidMAC(uint8_t *pMac)
{
	uint32 upper,lower;

	upper = ((pMac[0]&0xFF) << 8) | (pMac[1] & 0xFF);
	lower = ((pMac[2]&0xFF) << 24) | ((pMac[3]&0xFF) << 16) | ((pMac[4]&0xFF) << 8) | (pMac[5] & 0xFF);
	if (upper != 0x0000000F)
		return false;
	if ((lower < 0xF2080000) | (lower > 0xF208FFFF))
		return false;

	return true;
}
