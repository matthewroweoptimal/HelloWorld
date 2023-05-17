// LOUD_defines.h

#ifndef __LOUD_DEFINES__
#define __LOUD_DEFINES__

//
// BRANDS - Hardware Info:  4 bits
//

typedef enum {
    LOUD_BRAND_GENERIC = 0x0,
    LOUD_BRAND_MACKIE,
    LOUD_BRAND_EAW,
    LOUD_BRAND_MARTIN
} LOUD_brand;


//
// DEVICE MODELS (Firmware) - Hardware Info:  12 bits
//

//
// Generic Models
//

typedef enum {
    GENERIC_MODEL_GENERIC = 0x000
} GENERIC_model;


//
// Mackie Models
//

typedef enum {
    MACKIE_MODEL_GENERIC        = 0x000,
/*  Mixers */
    MACKIE_MODEL_DL1608,
    MACKIE_MODEL_DL806,
    MACKIE_MODEL_DL1608_DL806,  // WhiteApp doesn't know...
    MACKIE_MODEL_DL3214,
    MACKIE_MODEL_DL32R,
    MACKIE_MODEL_DL24XX,
    MACKIE_MODEL_DL24XX_RACK,
    MACKIE_MODEL_DC16,
/*  Loudspeakers */
    MACKIE_MODEL_R8_AA        = 0x020,      // Radius 8" Articulated Array Module
    MACKIE_MODEL_R8_64,                     // Radius 8" Full-Range, 60x40 Horn
    MACKIE_MODEL_R8_96,                     // Radius 8" Full-Range, 90x60 Horn
    MACKIE_MODEL_R12_64,                    // Radius 12" Full-Range, 60x40 Horn
    MACKIE_MODEL_R12_96,                    // Radius 12" Full-Range, 90x60 Horn
    MACKIE_MODEL_R12_SM,                    // Radius 12" Stage Monitor
    MACKIE_MODEL_R12_SW,                    // Radius 12" Subwoofer
    MACKIE_MODEL_R18_SW,                    // Radius 18" Subwoofer
/*  Software Products */
    MACKIE_MODEL_MASTER_FADER   = 0x040,
    MACKIE_MODEL_MY_FADER,
    MACKIE_MODEL_MASTER_RIG
} MACKIE_model;

//
// Martin Models
//

typedef enum {
    MARTIN_MODEL_GENERIC        = 0x000,
    MARTIN_MODEL_CDDL8,
    MARTIN_MODEL_CDDL12,
    MARTIN_MODEL_CDDL15,
    MARTIN_MODEL_CSXL118,
    MARTIN_MODEL_CSXL218,
    MARTIN_MODEL_GENERIC_NU,    // New equivalents of above Martin models, but running on Nuvoton hardware.
    MARTIN_MODEL_CDDL8_NU,
    MARTIN_MODEL_CDDL12_NU,
    MARTIN_MODEL_CDDL15_NU,
    MARTIN_MODEL_CSXL118_NU,
    MARTIN_MODEL_CSXL218_NU
} MARTIN_model;


// DL1608 Specific

// DL1608 Application IDs
typedef enum {
    DL1608_APPID_UNDEFINED = 0,
    DL1608_APPID_WHITEAPP,
    DL1608_APPID_BLACKAPP
} DL1608_application_id;

// DL1608 Package IDs
typedef enum {
    DL1608_PACKID_UNDEFINED = 0,
    DL1608_PACKID_MINI,
    DL1608_PACKID_MAIN
} DL1608_package_id;

// DL1608 Firmware Components
typedef enum {
    DL1608_FIRM_UNDEFINED = 0,
    DL1608_FIRM_MINI_OS,        // Blackfin 'mini' uClinux
    DL1608_FIRM_MINI_APP,       // Blackfin 'White' app
    DL1608_FIRM_MINI_INTERFACE, // Cypress 'Bonsai'
    DL1608_FIRM_OS,             // Blackfin 'main' uClinux
    DL1608_FIRM_APP,            // Blackfin 'Black' app
    DL1608_FIRM_INTERFACE,      // Cypress
    DL1608_FIRM_DSP             // SHARC
} DL1608_firmware_component;

#define DL1608_FIRST_MINI_COMPONENT DL1608_FIRM_MINI_OS
#define DL1608_LAST_MINI_COMPONENT  DL1608_FIRM_MINI_INTERFACE
#define DL1608_FIRST_MAIN_COMPONENT DL1608_FIRM_OS
#define DL1608_LAST_MAIN_COMPONENT  DL1608_FIRM_DSP
#define DL1608_COMPONENTS           DL1608_LAST_MAIN_COMPONENT


// DL806 Specific
//
// NOTE:  The DL806 is generally identical to the DL1608 (for the purposes of these defines).
//        Thus, these defines are redundant, but included for completeness!

// DL806 Application IDs
typedef enum {
    DL806_APPID_UNDEFINED = 0,
    DL806_APPID_WHITEAPP,
    DL806_APPID_BLACKAPP
} DL806_application_id;

// DL806 Package IDs
typedef enum {
    DL806_PACKID_UNDEFINED = 0,
    DL806_PACKID_MINI,
    DL806_PACKID_MAIN
} DL806_package_id;

// DL806 Firmware Components
typedef enum {
    DL806_FIRM_UNDEFINED = 0,
    DL806_FIRM_MINI_OS,        // Blackfin 'mini' uClinux
    DL806_FIRM_MINI_APP,       // Blackfin 'White' app
    DL806_FIRM_MINI_INTERFACE, // Cypress 'Bonsai'
    DL806_FIRM_OS,             // Blackfin 'main' uClinux
    DL806_FIRM_APP,            // Blackfin 'Black' app
    DL806_FIRM_INTERFACE,      // Cypress
    DL806_FIRM_DSP             // SHARC
} DL806_firmware_component;

#define DL806_FIRST_MINI_COMPONENT DL806_FIRM_MINI_OS
#define DL806_LAST_MINI_COMPONENT  DL806_FIRM_MINI_INTERFACE
#define DL806_FIRST_MAIN_COMPONENT DL806_FIRM_OS
#define DL806_LAST_MAIN_COMPONENT  DL806_FIRM_DSP
#define DL806_COMPONENTS           DL806_LAST_MAIN_COMPONENT


// DL32R Specific

// DL32R Application IDs
typedef enum {
    DL32R_APPID_UNDEFINED = 0,
    DL32R_APPID_WHITEAPP,
    DL32R_APPID_BLACKAPP
} DL32R_application_id;

// DL32R Package IDs
typedef enum {
    DL32R_PACKID_UNDEFINED = 0,
    DL32R_PACKID_MINI,
    DL32R_PACKID_MAIN
} DL32R_package_id;

// DL32R Hardware Components
typedef enum {
    DL32R_HARD_UNDEFINED = 0,
    DL32R_HARD_OPTION         // Blackfin option card slot
} DL32R_hardware_component;

// DL32R Option Cards
typedef enum {
    DL32R_OPTION_UNDEFINED = 0,
    DL32R_OPTION_NONE,
    DL32R_OPTION_ETHERNET,
    DL32R_OPTION_DANTE
} DL32R_option_card;

// DL32R Firmware Components
typedef enum {
    DL32R_FIRM_UNDEFINED = 0,
    DL32R_FIRM_MINI_OS,       // Blackfin 'mini' uClinux
    DL32R_FIRM_MINI_APP,      // Blackfin 'White' app
    DL32R_FIRM_OS,            // Blackfin 'main' uClinux
    DL32R_FIRM_APP,           // Blackfin 'Black' app
    DL32R_FIRM_DSP,           // SHARC
    DL32R_FIRM_FPGA,          // FPGA/router
    DL32R_FIRM_DANTE_OS,      // DANTE Brooklyn OS
    DL32R_FIRM_DANTE_APP,     // DANTE Brooklyn app
    DL32R_FIRM_RECORD,        // Archwave recording interface
    DL32R_FIRM_DANTE_CAP,     // DANTE Brooklyn capabilities
    DL32R_FIRM_DANTE_BUILD    // DANTE Brooklyn build
} DL32R_firmware_component;

#define DL32R_FIRST_MINI_COMPONENT DL32R_FIRM_MINI_OS
#define DL32R_LAST_MINI_COMPONENT  DL32R_FIRM_MINI_APP
#define DL32R_FIRST_MAIN_COMPONENT DL32R_FIRM_OS
#define DL32R_LAST_MAIN_COMPONENT  DL32R_FIRM_DANTE_BUILD
#define DL32R_COMPONENTS           DL32R_LAST_MAIN_COMPONENT


// DC16 Specific

// DC16 Application IDs
typedef enum {
    DC16_APPID_UNDEFINED = 0,
    DC16_APPID_WHITEAPP,
    DC16_APPID_ORANGEAPP
} DC16_application_id;

// DC16 Package IDs
typedef enum {
    DC16_PACKID_UNDEFINED = 0,
    DC16_PACKID_MINI,
    DC16_PACKID_MAIN
} DC16_package_id;

// DC16 Firmware Components
typedef enum {
    DC16_FIRM_UNDEFINED = 0,
    DC16_FIRM_MINI_OS,        // Blackfin 'mini' uClinux
    DC16_FIRM_MINI_APP,       // Blackfin 'White' app
    DC16_FIRM_MINI_INTERFACE, // IPIF controller Freescale MK60 MFi

    DC16_FIRM_OS        = 8,  // Blackfin 'main' uClinux
    DC16_FIRM_APP,            // Blackfin 'Orange' app
    DC16_FIRM_DSP,            // Audio controller SHARC
    DC16_FIRM_INTERFACE,      // IPIF controller Freescale MK60 MFi
    DC16_FIRM_HUI_APP,        // Blackfin 'HUI' app
    DC16_FIRM_GRAPHICS,       // GPU controller STM32F429I
    DC16_FIRM_SURFACE,        // Front panel controller Freescale MK20F512 MCU
    DC16_FIRM_DANTE_APP,      // DANTE network controller Audinate Ultimo4 app
    DC16_FIRM_DANTE_CAP,      // DANTE network controller Audinate Ultimo4 capabilities
    DC16_FIRM_IMAGE           // png2bmp image conversion utility
} DC16_firmware_component;

#define DC16_FIRST_MINI_COMPONENT DC16_FIRM_MINI_OS
#define DC16_LAST_MINI_COMPONENT  DC16_FIRM_MINI_INTERFACE
#define DC16_FIRST_MAIN_COMPONENT DC16_FIRM_OS
#define DC16_LAST_MAIN_COMPONENT  DC16_FIRM_IMAGE
#define DC16_COMPONENTS           DC16_LAST_MAIN_COMPONENT


// OLY Specific

// OLY Application IDs
typedef enum {
    OLY_APPID_UNDEFINED = 0,
    OLY_APPID_BOOTLOADER,
    OLY_APPID_MAIN,
} OLY_application_id;



//
// EAW Models
//

typedef enum {
    EAW_MODEL_GENERIC = 0x000,
    EAW_MODEL_DX1208,
    /*  Loudspeakers */
    EAW_MODEL_ANYA = 0x002,
    EAW_MODEL_OTTO,
    EAW_MODEL_HALARCH,
    EAW_MODEL_ANNA,
	/*  Radius */
    EAW_MODEL_RSX208L = 0x020,    // Radius 8" Articulated Array Module
    EAW_MODEL_RSX86,              // Radius 8" Full-Range, 60x40 Horn
    EAW_MODEL_RSX89,              // Radius 8" Full-Range, 90x60 Horn
    EAW_MODEL_RSX126,             // Radius 12" Full-Range, 60x40 Horn
    EAW_MODEL_RSX129,             // Radius 12" Full-Range, 90x60 Horn
    EAW_MODEL_RSX12M,             // Radius 12" Stage Monitor
    EAW_MODEL_RSX12,              // Radius 12" Subwoofer
    EAW_MODEL_RSX18,              // Radius 18" Subwoofer
    EAW_MODEL_RSX218,              // Radius 18" Subwoofer
    EAW_MODEL_RSX18F,              //  Radius 18" Subwoofer
    EAW_MODEL_RSX212L,              // Radius 12" Articulated Array Module
} EAW_model;


// Generic (Software)
typedef enum {
    EAW_APPID_UNDEFINED = 0,
    EAW_APPID_RESOLUTION
} EAW_application_id;


// ANYA Specific

// ANYA Application IDs
typedef enum {
	ANYA_APPID_XML = -1,
    ANYA_APPID_UNDEFINED = 0,
    ANYA_APPID_WHITEAPP,
    ANYA_APPID_GOVNAPP,
    ANYA_APPID_BROOKLYNAPP,
	ANYA_APPID_DSPBOOT,
	ANYA_APPID_DSPOS,
	ANYA_APPID_DSPTST
} ANYA_application_id;

// ANYA Package IDs
typedef enum {
    ANYA_PACKID_UNDEFINED = 0,
    ANYA_PACKID_MINI,
    ANYA_PACKID_MAIN
} ANYA_package_id;

// ANYA Firmware Components
typedef enum {
    ANYA_FIRM_UNDEFINED = 0,
    ANYA_FIRM_MINI_OS,        // Blackfin 'mini' uClinux
    ANYA_FIRM_MINI_APP,       // Blackfin 'White' app
    ANYA_FIRM_MINI_INTERFACE, // Brooklyn 'Bonsai'
    ANYA_FIRM_OS,             // Blackfin 'main' uClinux
    ANYA_FIRM_APP,            // Blackfin 'Black' app
    ANYA_FIRM_INTERFACE,      // Brooklyn
    ANYA_FIRM_DSP             // SHARC
} ANYA_firmware_component;

#define ANYA_FIRST_MINI_COMPONENT ANYA_FIRM_MINI_OS
#define ANYA_LAST_MINI_COMPONENT  ANYA_FIRM_MINI_INTERFACE
#define ANYA_FIRST_MAIN_COMPONENT ANYA_FIRM_OS
#define ANYA_LAST_MAIN_COMPONENT  ANYA_FIRM_DSP
#define ANYA_COMPONENTS           ANYA_LAST_MAIN_COMPONENT


typedef enum {
    ANYA_REBOOT_HARDWARE = 0,
    ANYA_REBOOT_TO_BOOT,
    ANYA_REBOOT_TO_OS,
	ANYA_REBOOT_TO_MTST,
	ANYA_REBOOT_TO_SAFE_OS,
} ANYA_reboot_type;


typedef enum {
    ANYA_TESTID_SET_FUNC_GEN_OUTPUT = 0x0001,
    ANYA_TESTID_SET_FUNC_GEN,
    ANYA_TESTID_SET_RMS_METER_INPUT,
    ANYA_TESTID_GET_RMS_METER_LEVEL,
    ANYA_TESTID_GET_RMS_METER_RANGE,
    ANYA_TESTID_WRITEIO,
    ANYA_TESTID_READIO,
    ANYA_TESTID_READ_AMP_TEMPERATURE,
    ANYA_TESTID_READ_AMP_IMPEDANCE,
	ANYA_TESTID_READ_SD_RAM_TEST_STATUS,

    ANYA_TESTID_GOVN_KERNEL = 0x41,
    ANYA_TESTID_GOVN_RAM,
    ANYA_TESTID_GOVN_SD_CARD,
    ANYA_TESTID_GOVN_NAND_FLASH,
    ANYA_TESTID_GOVN_RTC,
    ANYA_TESTID_GOVN_DEBUG_UART,
    ANYA_TESTID_GOVN_IRDA,
    ANYA_TESTID_GOVN_TEMPERATURE_DYE,
    ANYA_TESTID_GOVN_TEMPERATURE_ON_BOARD,
	ANYA_TESTID_GOVN_READ_FP_BUTTONS,		// 0 test off, 1=put in test, 2=read button when in test
	ANYA_TESTID_GOVN_SET_FP_LEDS,
    ANYA_TESTID_GOVN_SET_FP_STATUSLED,
    ANYA_TESTID_GOVN_ACCELEROMETER,
    ANYA_TESTID_GOVN_BROOKLYN_COMMS,
    ANYA_TESTID_GOVN_DSP_COMMS,
    ANYA_TESTID_GOVN_USB_HOST,
    ANYA_TESTID_GOVN_USB_DEVICE,
    ANYA_TESTID_GOVN_BATTERY,
	ANYA_TESTID_GOVN_IRDAINTERNAL,

} ANYA_TESTID;

// irda EQUATES
typedef enum
{
	ANYA_IRDA_PORT_TOP = 0,
	ANYA_IRDA_PORT_BOTTOM,
	ANYA_IRDA_PORT_LEFT,
	ANYA_IRDA_PORT_RIGHT,
} ANYA_IRDA_PORTID;

#define ANYA_IRDA_PORTBITMASK_TOP	0x01
#define ANYA_IRDA_PORTBITMASK_BOTTOM 0x02
#define ANYA_IRDA_PORTBITMASK_LEFT	0x04
#define ANYA_IRDA_PORTBITMASK_RIGHT 0x08
#define ANYA_IRDA_PORTBITMASK_FRONT		0x10
#define ANYA_IRDA_PORTBITMASK_BACK 		0x20
#define ANYA_IRDA_PORTBITMASK_TOP2		0x40	// top - back left
#define ANYA_IRDA_PORTBITMASK_BOTTOM2	0x80	// bottom - back right

//
// HOST App IDs (3rd-party PC/Tablet/Phone Software Applications) - Software Info:  32 bits
//

//
// Mackie App IDs
//

typedef enum {
    MACKIE_APPID_UNDEFINED = 0x00000000,
    MACKIE_APPID_MASTER_FADER,
    MACKIE_APPID_MY_FADER,
    MACKIE_APPID_MASTER_RIG
} MACKIE_application_id;

#endif // __LOUD_DEFINES__
