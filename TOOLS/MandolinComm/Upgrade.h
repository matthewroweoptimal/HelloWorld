/*
 * Upgrade.h
 *
 *  Created on: Jan 21, 2016
 *      Author: Ted.Wolfe
 *
 *	This file details common structures and defines used for upgrading 
 *	that must be shared between the primary and secondary bootloaders and host application.
*/


#pragma once

// Match Nuvoton Definitions :
#define OLY_MAGIC_WORD					0x57102957	//	word to identify  contents is a valid structure
#define OLY_BLOCK_VERSION				0			//	Increment to force structure to initialize on boot

// We allow 4kB at top of Flash for the Data Flash Region, comprised of :
//      0x0007F000 :  OLY_BLOCK         (3840 bytes)
//      0x0007FF00 :  OLY_IDENTITY      (256 bytes)
//
#define OLY_BLOCK_LOCATION				((P_OLY_BLOCK)0x0007F000)	//	Data Flash location of where Shared Upgrade region structure OLY_BLOCK is located.
#define OLY_IDENTITY_OFFSET				(0x0007FF00)                //	location of where Shared identity (mac, rev, brand, model) is located.
#define OLY_IDENTITY_LOCATION			((P_OLY_IDENTITY)OLY_IDENTITY_OFFSET)	//	location of where Shared identity (mac, rev, brand, model) is located.

#define OLY_DEFAULT_VECTOR_TABLE		0x0000	    //	location of vector table to use if there is not valid option from application selection algorithm, typically, the secondary bootloader
#define OLY_DEFAULT_STACK_PTR			0x00028000	//	location of stack pointer to use if there is not valid option from application selection algorithm, typically, the secondary bootloader
#define OLY_MAX_REGIONS					1			//	maximum number of flash regions defined

#define FW_UPGRADE_CHUNK_SIZE           1024            // 1kB chunks due to Mandolin message size support limitations on CDDLive


typedef	union ip_address_union_t {
	struct {
		unsigned int ip;			//	IPV4	IP address, 0 for DHCP
		unsigned int reserved;		//	undefined
	} ipv4;
	struct {
		unsigned char ip[6];		//	IPV6	IP address for IPV6
		unsigned char reserved[2];	//	undefined
	} ipv6;
} OLY_IP_ADDRESS, *P_OLY_IP_ADDRESS;

typedef union {
    struct {
    unsigned char major;
    unsigned char minor;
    unsigned char sub;
    unsigned char build;      // Not used by application, build versioning only
    };
    unsigned int u32;
} OLY_VERSION, *P_OLY_VERSION;

typedef enum {
	OLY_REGION_UNUSED,			//	region is not defined
	OLY_REGION_INFO,			//	????-Not sure what this is
	OLY_REGION_SECONDARY_BOOT,	//	Region contains secondary boot loader
	OLY_REGION_APPLICATION,		//	Region conatins upgrade application
	OLY_REGION_SHARC_BOOT,		//	Region contains SHARC code
	OLY_REGION_DEVICE_INFO		//	????-Not sure what this is 
} OLY_REGION_TYPE;

typedef struct {
	OLY_REGION_TYPE type;		//	what does this region contain
	OLY_VERSION version;		//	expected version of what the region
	unsigned int address;		//	absolute address of start of flash region
	unsigned int length;		//	number of bytes used in the flash region
	unsigned int vectorTable;	//	location of vector table in flash (OLY_REGION_SECONDARY_BOOT & OLY_REGION_APPLICATION type only).
	unsigned int stackPtr;		//	location of initial system stack (OLY_REGION_SECONDARY_BOOT & OLY_REGION_APPLICATION type only).
	unsigned short crc;			//	expected value for CRC calculation
	unsigned char align[2];		//	for 32 bit alignment -  can be used for future expansion
	unsigned int reserved[9];	//	for future expansion
} OLY_REGION, *P_OLY_REGION;	//	size: 64 bytes

typedef struct SharedSystemParameters_t {	 
	size_t size;				//	Size of this structure
	int generation;				//	number of times structure has been written to same flash block
	unsigned int magicWord;		//	See OLY_MAGIC_WORD
	int version;				//  SC : This was missing, added in to match structure in Nuvoton project
	int isIpv6;					//	non-zero for ipv6
	OLY_IP_ADDRESS staticIp;	//	static IP Address to use or dhcp indication
	OLY_IP_ADDRESS gatewayIp;	//	static Gateway IP address to use 
	OLY_IP_ADDRESS mask;		//	static mask to use
	OLY_REGION_TYPE launchType;	//	the type of application that the primary bootloader should try to launch.
	unsigned int reserved[21];	//	future expansion
	OLY_REGION rgn[OLY_MAX_REGIONS];//	flash region descriptors
} OLY_BLOCK, *P_OLY_BLOCK;		//	size: 128 + (8 * 64) = 640 bytes

