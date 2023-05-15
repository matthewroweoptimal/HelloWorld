// mandolin.h - A parser for the MANDOLIN audio control protocol.
//              ASSUMES LITTLE-ENDIAN HOST BYTE ORDER!

#ifndef __MANDOLIN_H__
#define __MANDOLIN_H__

// Includes
#include "LOUD_types.h"
#include "mandolin_config.h"


// Defines
#define MANDOLIN_MAJOR_VERSION 1
#define MANDOLIN_MINOR_VERSION 4
//      MANDOLIN BUILD # 2          // This is only intended for development use!


//
// Protocol
//

// General
#define MANDOLIN_FILL_BYTE 0x00

// Message ID
typedef enum {
    MANDOLIN_MSG_INVALID                            = 0x00,

    // Core
    MANDOLIN_MSG_PING                               = 0x01,
    MANDOLIN_MSG_ADVERTIZE_HARDWARE_INFO,
    MANDOLIN_MSG_GET_HARDWARE_INFO,
    MANDOLIN_MSG_GET_SOFTWARE_INFO,
    MANDOLIN_MSG_REBOOT,

    // Event
    MANDOLIN_MSG_TRIGGER_SYSTEM_EVENT               = 0x06,
    MANDOLIN_MSG_TRIGGER_APPLICATION_EVENT,

    // Test
    MANDOLIN_MSG_TEST                               = 0x08,

    // Info
    MANDOLIN_MSG_GET_INFO                           = 0x09,

    // File
    MANDOLIN_MSG_FILE_OPEN                          = 0x0A,
    MANDOLIN_MSG_FILE_CLOSE,
    MANDOLIN_MSG_GET_FILE,
    MANDOLIN_MSG_POST_FILE,

    // State
    MANDOLIN_MSG_GET_SYSTEM_STATUS                  = 0x0E,
    MANDOLIN_MSG_SET_SYSTEM_STATE,
    MANDOLIN_MSG_RESET_SYSTEM_STATE,
    MANDOLIN_MSG_SUBSCRIBE_SYSTEM_STATE,

    // Parameter
    MANDOLIN_MSG_GET_APPLICATION_PARAMETER          = 0x12,
    MANDOLIN_MSG_SET_APPLICATION_PARAMETER,
    MANDOLIN_MSG_RESET_APPLICATION_PARAMETER,
    MANDOLIN_MSG_SUBSCRIBE_APPLICATION_PARAMETER,
    MANDOLIN_MSG_CREATE_PARAMETER_LIST,

    // String
    MANDOLIN_MSG_GET_APPLICATION_STRING             = 0x17,
    MANDOLIN_MSG_SET_APPLICATION_STRING,
    MANDOLIN_MSG_RESET_APPLICATION_STRING,

    MANDOLIN_MESSAGES                               = 26    // including '0' (==INVALID)
} mandolin_message_id;

// Progress
typedef enum {
    MANDOLIN_PROGRESS_FAIL,
    MANDOLIN_PROGRESS_SUCCESS,
    MANDOLIN_PROGRESS_BUSY
} mandolin_progress_code;

// Subscription
typedef enum {
    MANDOLIN_UNSUBSCRIBE,
    MANDOLIN_SUBSCRIBE
} mandolin_subscription_code;

// Error
typedef enum {
    MANDOLIN_NO_ERROR_NO_FURTHER_REPLY = -2,    // Return from handler - indicates a 'real' (non-ACK/NACK) reply has been generated.
    MANDOLIN_NO_ERROR_FORWARD_IN_PLACE = -1,    // Return from handler - indicates the original message needs to be forwarded 'in place'.

    MANDOLIN_NO_ERROR                  =  0,

    MANDOLIN_ERROR                     =  1,
    MANDOLIN_ERROR_BUSY,
    MANDOLIN_ERROR_LOCKED,
    MANDOLIN_ERROR_OUT_OF_MEMORY,
    MANDOLIN_ERROR_TIMEOUT,
    MANDOLIN_ERROR_CHECKSUM_MISMATCH,
    MANDOLIN_ERROR_LENGTH_MISMATCH,
    MANDOLIN_ERROR_UNIMPLEMENTED_MSGID =  8,

    MANDOLIN_ERROR_INVALID_MSGID       =  9,
    MANDOLIN_ERROR_INVALID_FLAGS,
    MANDOLIN_ERROR_INVALID_ID,
    MANDOLIN_ERROR_INVALID_DATA        = 12,

    MANDOLIN_ERROR_FLASH_ERASE         = 13,
    MANDOLIN_ERROR_FLASH_WRITE,
    MANDOLIN_ERROR_FLASH_READ          = 15,

    MANDOLIN_ERRORS
} mandolin_error_code;


// Message

// Header

// Sync
#if defined(__ADSP21000__)
#define MANDOLIN_SYNC_WORD ((uint32) 0x000000AB)
#else
#define MANDOLIN_SYNC_WORD ((uint8) 0xAB)
#endif

// Sequence
#define MANDOLIN_SEQUENCE_NULL 0x00

// Mask & shift vs. transport flags byte
#define MANDOLIN_TRANSPORT_NULL         0x00
#define MANDOLIN_TRANSPORT_REPLY        0x01
#define MANDOLIN_TRANSPORT_REPLY_MASK   ((uint8) 0x01)
#define MANDOLIN_TRANSPORT_REPLY_SHIFT  0
#define MANDOLIN_TRANSPORT_PEND         0x02
#define MANDOLIN_TRANSPORT_PEND_MASK    ((uint8) 0x02)
#define MANDOLIN_TRANSPORT_PEND_SHIFT   1
#define MANDOLIN_TRANSPORT_NACK         0x04
#define MANDOLIN_TRANSPORT_NACK_MASK    ((uint8) 0x04)
#define MANDOLIN_TRANSPORT_NACK_SHIFT   2
#define MANDOLIN_TRANSPORT_NOACK        0x08
#define MANDOLIN_TRANSPORT_NOACK_MASK   ((uint8) 0x08)
#define MANDOLIN_TRANSPORT_NOACK_SHIFT  3
#define MANDOLIN_TRANSPORT_ENDIAN       0x80
#define MANDOLIN_TRANSPORT_ENDIAN_MASK  ((uint8) 0x80)
#define MANDOLIN_TRANSPORT_ENDIAN_SHIFT 7

#define IS_MANDOLIN_REPLY(transport_flags)          (0!=((transport_flags) & (MANDOLIN_TRANSPORT_REPLY_MASK)))
#define IS_MANDOLIN_ACK(transport_flags)            (0==((transport_flags) & (MANDOLIN_TRANSPORT_NACK_MASK)))
#define IS_MANDOLIN_NACK(transport_flags)           (0!=((transport_flags) & (MANDOLIN_TRANSPORT_NACK_MASK)))
#define IS_MANDOLIN_PEND(transport_flags)           (0!=((transport_flags) & (MANDOLIN_TRANSPORT_PEND_MASK)))
#define ISNT_MANDOLIN_NOACK(transport_flags)        (0==((transport_flags) & (MANDOLIN_TRANSPORT_NOACK_MASK)))
#define IS_MANDOLIN_HOST_ENDIAN(transport_flags)    (0!=((transport_flags) & (MANDOLIN_TRANSPORT_ENDIAN_MASK)))
#define IS_MANDOLIN_NETWORK_ENDIAN(transport_flags) (0==((transport_flags) & (MANDOLIN_TRANSPORT_ENDIAN_MASK)))

// Checksum
#define MANDOLIN_HEADER_CHECKSUM_BYTES 6u

typedef enum {
    MANDOLIN_BYTE_SYNC,        // 0
    MANDOLIN_BYTE_SEQUENCE,    // 1
    MANDOLIN_BYTE_LENGTH_HI,   // 2
    MANDOLIN_BYTE_LENGTH_LO,   // 3
    MANDOLIN_BYTE_TRANSPORT,   // 4
    MANDOLIN_BYTE_MSG_ID,      // 5
    MANDOLIN_BYTE_CHECKSUM_HI, // 6
    MANDOLIN_BYTE_CHECKSUM_LO, // 7
    MANDOLIN_BYTE_PAYLOAD      // 8
} mandolin_message_byte;

typedef struct {
#if defined(__ADSP21000__)
    uint32 sync;
    uint32 sequence;
    uint32 length;
    uint32 transport;
    uint32 id;
    uint32 checksum;
    void  *payload;
#else
    uint8  sync;
    uint8  sequence;
    uint16 length;
    uint8  transport;
    uint8  id;
    uint16 checksum;
    void  *payload;
#endif
} mandolin_message;

// Payload

// Advertize Hardware Info
typedef enum {
    MANDOLIN_ADVERTIZE_TERMINATE,
    MANDOLIN_ADVERTIZE_CONTINUE
} mandolin_advertize_response;

// Get Hardware Info
#define MANDOLIN_HARDINFO_BRAND_MASK   ((uint32) 0xF0000000)
#define MANDOLIN_HARDINFO_BRAND_SHIFT  28
#define MANDOLIN_HARDINFO_MODEL_MASK   ((uint32) 0x0FFF0000)
#define MANDOLIN_HARDINFO_MODEL_SHIFT  16
#define MANDOLIN_HARDINFO_MAC_HI_MASK  ((uint32) 0x0000FFFF)
#define MANDOLIN_HARDINFO_MAC_HI_SHIFT 0

#define MANDOLIN_HARDWARE_BRAND(version_word)  (((version_word) >> MANDOLIN_HARDINFO_BRAND_SHIFT) & 0x0000000F)     // first version word
#define MANDOLIN_HARDWARE_MODEL(version_word)  (((version_word) >> MANDOLIN_HARDINFO_MODEL_SHIFT) & 0x00000FFF)     // first version word
#define MANDOLIN_HARDWARE_MAC_HI(version_word) ((version_word) & MANDOLIN_HARDINFO_MAC_HI_MASK)                     // first version word
#define MANDOLIN_HARDWARE_MAC_LO(version_word) (version_word)                                                       // second version word

// Get Software Info
#define MANDOLIN_SOFTINFO_MANDOLIN_MASK  ((uint32) 0xFFFF0000)
#define MANDOLIN_SOFTINFO_MANDOLIN_SHIFT 16
#define MANDOLIN_SOFTINFO_XML_MASK       ((uint32) 0x0000FFFF)
#define MANDOLIN_SOFTINFO_XML_SHIFT      0

#define MANDOLIN_PROTOCOL_VERSION(version_word) (((version_word) >> MANDOLIN_SOFTINFO_MANDOLIN_SHIFT) & 0x0000FFFF) // first version word
#define MANDOLIN_XML_VERSION(version_word)      ((version_word) & MANDOLIN_SOFTINFO_XML_MASK)                       // first version word
#define MANDOLIN_APPLICATION_ID(version_word)   (version_word)                                                      // second version word
#define MANDOLIN_FIRMWARE_VERSION(version_word) (version_word)                                                      // third version word

// Reboot
#define MANDOLIN_BOOT_TARGET_DEFAULT 0

// System Event
typedef enum {
    MANDOLIN_SE_CONNECT             = 0,
    MANDOLIN_SE_DISCONNECT,
    MANDOLIN_SE_LOCK_CHANGES,
    MANDOLIN_SE_UNLOCK_CHANGES,
    MANDOLIN_SE_IDENTIFY,

    MANDOLIN_SE_SYNC_CURRENT_STATE  = 6,
    MANDOLIN_SE_SYNC_OK,
    MANDOLIN_SE_BEGIN_UPDATE,
    MANDOLIN_SE_END_UPDATE,
    MANDOLIN_SE_PROCESS_FILES,
    MANDOLIN_SE_CODEC_RESET,

    MANDOLIN_SE_ENABLE_TUNNEL       = 16
} mandolin_system_event;

// Connect
typedef enum {
    MANDOLIN_CONNECT_FULL,
    MANDOLIN_CONNECT_FAST
} mandolin_connect_code;

// Disconnect
typedef enum {
    MANDOLIN_DISCONNECT_NO_REASON,
    MANDOLIN_DISCONNECT_USER_REQUEST,
    MANDOLIN_DISCONNECT_DEVICE_MISMATCH,
    MANDOLIN_DISCONNECT_VERSION_MISMATCH,
    MANDOLIN_DISCONNECT_UPDATING,
    MANDOLIN_DISCONNECT_SNAPSHOT_SYNCHING,
    MANDOLIN_DISCONNECT_SHOW_MISMATCH,
    MANDOLIN_DISCONNECT_SHOW_SYNCHING
} mandolin_disconnect_code;

// Lock Changes
typedef enum {
    MANDOLIN_LOCK_SHOW_CREATE_1 = 1,
    MANDOLIN_LOCK_SHOW_WRITE_2,
    MANDOLIN_LOCK_SHOW_PUSH_3,
    MANDOLIN_LOCK_SNAPSHOT_SYNC_4,
    MANDOLIN_LOCK_DISCONNECTED_5
} mandolin_lock_code;

// Unlock Changes
typedef enum {
    MANDOLIN_UNLOCK_SHOW_CREATED_1 = 1,
    MANDOLIN_UNLOCK_SHOW_WRITTEN_2,
    MANDOLIN_UNLOCK_SHOW_PUSHED_3,
    MANDOLIN_UNLOCK_SNAPSHOT_SYNCED_4,
    MANDOLIN_UNLOCK_CONNECTED_5
} mandolin_unlock_code;

// Identify
#define MANDOLIN_DURATION_INFINITY 0xFFFFFFFF

// Application Event
typedef enum {
    MANDOLIN_AE_LOAD_SNAPSHOT       = 1,
    MANDOLIN_AE_STORE_SNAPSHOT,
    MANDOLIN_AE_DELETE_SNAPSHOT,
    MANDOLIN_AE_MOVE_SNAPSHOT,
    MANDOLIN_AE_NAME_SNAPSHOT,

    MANDOLIN_AE_SHOW_READY          = 6,
    MANDOLIN_AE_PUSH_SHOW,
    MANDOLIN_AE_WRITE_SHOW,
    MANDOLIN_AE_SET_SHOW,
    MANDOLIN_AE_SHOW_OK,

    MANDOLIN_AE_TRANSPORT_COMMAND   = 11
} mandolin_application_event;

// Timestamp
typedef uint32 mandolin_timestamp;  // the least significant 32-bits of a Unix time_t seconds value

#define MANDOLIN_TIMESTAMP_UNKNOWN 0

// Get Info
typedef enum {
    MANDOLIN_GI_FILE                = 2,
    MANDOLIN_GI_LIST,
    MANDOLIN_GI_SNAPSHOT,
    MANDOLIN_GI_SHOW
} mandolin_info_type;

#define MANDOLIN_SHOW_CHANGE_IN_PROGRESS 0

// File
typedef uint32 mandolin_file_id;

// List
typedef uint32 mandolin_list_id;

#define MANDOLIN_NULL_LIST_ID 0

// Snapshot
typedef uint32 mandolin_snapshot_id;

// Show
typedef uint32 mandolin_show_id;

// File Transfer
typedef enum {
    MANDOLIN_FILE_ERROR_NONE = -1,
    MANDOLIN_FILE_ERROR_NO_REASON,
    MANDOLIN_FILE_ERROR_LOCKED,
    MANDOLIN_FILE_ERROR_BAD_TYPE,
    MANDOLIN_FILE_ERROR_BAD_NAME
} mandolin_file_error;

typedef enum {
    MANDOLIN_FILE_DELETE,
    MANDOLIN_FILE_POST
} mandolin_file_code;

#define MANDOLIN_FILE_ACCESS_MASK  ((uint32) 0x80000000)
#define MANDOLIN_FILE_ACCESS_SHIFT 31
#define MANDOLIN_FILE_TYPE_MASK    ((uint32) 0x00FFFFFF)
#define MANDOLIN_FILE_TYPE_SHIFT   0

typedef enum {
    MANDOLIN_FILE_ACCESS_WRITE,
    MANDOLIN_FILE_ACCESS_READ
} mandolin_file_access;

typedef enum {
    MANDOLIN_FILE_TYPE_GENERIC,
    MANDOLIN_FILE_TYPE_UPDATE,
    MANDOLIN_FILE_TYPE_SNAPSHOT,
    MANDOLIN_FILE_TYPE_IMAGE,
    MANDOLIN_FILE_TYPE_SHOW,
    MANDOLIN_FILE_TYPE_STATE,
    MANDOLIN_FILE_TYPE_NOTES,
    MANDOLIN_FILE_TYPE_BROWSE
} mandolin_file_type;

#define MANDOLIN_FILE_ACCESS(flags_word)   (((flags_word) >> MANDOLIN_FILE_ACCESS_SHIFT) & 0x00000001)
#define MANDOLIN_FILE_TYPE(flags_word)     ((flags_word) & MANDOLIN_FILE_TYPE_MASK)
#define IS_MANDOLIN_FILE_WRITE(flags_word) (MANDOLIN_FILE_ACCESS_WRITE==MANDOLIN_FILE_ACCESS(flags_word))
#define IS_MANDOLIN_FILE_READ(flags_word)  (MANDOLIN_FILE_ACCESS_READ==MANDOLIN_FILE_ACCESS(flags_word))

// System State
typedef enum {
    MANDOLIN_SS_ERROR_CODE          =  0,
    MANDOLIN_SS_BOOT_CODE           =  1,
    MANDOLIN_SS_MAX_MSG_WORDS,
    MANDOLIN_SS_SYSTEM_LABEL,
    MANDOLIN_SS_SYSTEM_IP_ADDRESS,
    MANDOLIN_SS_LOCK_STATUS,
    MANDOLIN_SS_STATIC_IP_CONFIG,
    MANDOLIN_SS_SUBSYSTEM_LABEL,
    MANDOLIN_SS_REC_PLAY_TRANSPORT  =  8,
    MANDOLIN_SS_REC_PLAY_MULTITRACK,
    MANDOLIN_SS_REC_PLAY_MUSIC,
    MANDOLIN_SS_DANTE_CONFIG        = 11,

    MANDOLIN_SS_MIXER               = 16,
    MANDOLIN_SS_MIXERLIST
} mandolin_query_id;

typedef enum {
    MANDOLIN_LOCK_LEVEL_NONE,
    MANDOLIN_LOCK_LEVEL_ONE,
    MANDOLIN_LOCK_LEVEL_TWO,
    MANDOLIN_LOCK_LEVEL_THREE,
    MANDOLIN_LOCK_LEVEL_FOUR
} mandolin_lock_level;

// Boot Code
typedef enum {
    MANDOLIN_BOOT_CODE_NO_REASON,
    MANDOLIN_BOOT_CODE_UPDATE,
    MANDOLIN_BOOT_CODE_PANIC,
    MANDOLIN_BOOT_CODE_CORRUPT,
    MANDOLIN_BOOT_CODE_CORRUPT_OS,
    MANDOLIN_BOOT_CODE_CORRUPT_FILESYSTEM,
    MANDOLIN_BOOT_CODE_HARDWARE_FAULT
} mandolin_boot_code;

// System Label
#define MANDOLIN_MAX_LABEL_OCTETS 63

// Scheduling
#define MANDOLIN_RATE_UNSUBSCRIBE 0

// Mask & shift vs. scheduling flags word
#define MANDOLIN_SCHEDULE_RATE_MASK   ((uint32) 0xFFFF0000)
#define MANDOLIN_SCHEDULE_RATE_SHIFT  16
#define MANDOLIN_SCHEDULE_NOACK_MASK  ((uint32) 0x00000001)
#define MANDOLIN_SCHEDULE_NOACK_SHIFT  0

#define IS_MANDOLIN_UNSUBSCRIBE(flags_word)     (((flags_word) >> MANDOLIN_SCHEDULE_RATE_SHIFT) & 0x0000FFFF)
#define IS_MANDOLIN_NOACK_SUBSCRIBE(flags_word) (1==((flags_word) & MANDOLIN_SCHEDULE_NOACK_MASK))

typedef struct {
#if defined(__ADSP21000__)
    uint32 rate;
    uint32 reserved;
    uint32 noack;
#else
    uint16 rate;
    uint8  reserved;
    uint8  noack;
#endif
} mandolin_scheduling_flags;

// Application Parameter
typedef int32 mandolin_parameter_id;    // valid ID values are positive
typedef int32 mandolin_meter_id;

typedef union {
    int32   i;
    uint32  u;
    bool32  b;
    float32 f;
} mandolin_parameter_value;

// Mask & shift vs. wildcard flags word
#define MANDOLIN_WILDCARD_TYPE_MASK       ((uint32) 0xF0000000)
#define MANDOLIN_WILDCARD_TYPE_SHIFT      28
#define MANDOLIN_WILDCARD_MAP_MASK        ((uint32) 0x0F000000)
#define MANDOLIN_WILDCARD_MAP_SHIFT       24
#define MANDOLIN_WILDCARD_DIRECTION_MASK  ((uint32) 0x00800000)
#define MANDOLIN_WILDCARD_DIRECTION_SHIFT 23
#define MANDOLIN_WILDCARD_COUNT_MASK      ((uint32) 0x007F0000)
#define MANDOLIN_WILDCARD_COUNT_SHIFT     16

typedef enum {
    MANDOLIN_TYPE_NEIGHBORHOOD = 0,
    MANDOLIN_TYPE_LIST,
    MANDOLIN_TYPE_MULTIPARAMETER
} mandolin_wildcard_type;

typedef enum {
    MANDOLIN_DIRECTION_NEXT = 0,
    MANDOLIN_DIRECTION_PREVIOUS
} mandolin_wildcard_direction;

#define MANDOLIN_WILDCARD_MAX_COUNT 0x7F
#define MANDOLIN_WILDCARD_COUNT_ALL 0

#define MANDOLIN_WILD_TYPE(flags_word)         (((flags_word) >> MANDOLIN_WILDCARD_TYPE_SHIFT)      & 0x0000000F)
#define MANDOLIN_WILD_MAP(flags_word)          (((flags_word) >> MANDOLIN_WILDCARD_MAP_SHIFT)       & 0x0000000F)
#define MANDOLIN_WILD_DIRECTION(flags_word)    (((flags_word) >> MANDOLIN_WILDCARD_DIRECTION_SHIFT) & 0x00000001)
#define MANDOLIN_WILD_COUNT(flags_word)        (((flags_word) >> MANDOLIN_WILDCARD_COUNT_SHIFT)     & 0x0000007F)
#define IS_MANDOLIN_NEIGHBORHOOD(flags_word)   (MANDOLIN_TYPE_NEIGHBORHOOD==MANDOLIN_WILD_TYPE(flags_word))
#define IS_MANDOLIN_PARAMETER_LIST(flags_word) (MANDOLIN_TYPE_LIST==MANDOLIN_WILD_TYPE(flags_word))
#define IS_MANDOLIN_MULTIPARAMETER(flags_word) (MANDOLIN_TYPE_MULTIPARAMETER==MANDOLIN_WILD_TYPE(flags_word))
#define IS_MANDOLIN_WILDCARD(flags_word)       (1!=MANDOLIN_WILD_COUNT(flags_word))

#define MANDOLIN_EDIT_TARGET_MASK  ((uint32) 0x0000FF00)
#define MANDOLIN_EDIT_TARGET_SHIFT 8

#define MANDOLIN_EDIT_TARGET(flags_word) (((flags_word) >> MANDOLIN_EDIT_TARGET_SHIFT) & 0x000000FF)

#define MANDOLIN_EXTENDED_FLAGS_FORMAT_MASK       ((uint32) 0x00000001)
#define MANDOLIN_EXTENDED_FLAGS_FORMAT_SHIFT      0
#define MANDOLIN_EXTENDED_FLAGS_START_MASK        ((uint32) 0x00000002)
#define MANDOLIN_EXTENDED_FLAGS_START_SHIFT       1
#define MANDOLIN_EXTENDED_FLAGS_STOP_MASK         ((uint32) 0x00000004)
#define MANDOLIN_EXTENDED_FLAGS_STOP_SHIFT        2
#define MANDOLIN_EXTENDED_FLAGS_INTERPOLATE_MASK  ((uint32) 0x00000008)
#define MANDOLIN_EXTENDED_FLAGS_INTERPOLATE_SHIFT 3

#define ARE_MANDOLIN_EXTENDED_FLAGS(flags_word) (0!=((flags_word) & 0x0000000F))

typedef struct {
#if defined(__ADSP21000__)
    uint32 wildcard;
    uint32 target;
    uint32 extended;
#else
    uint16 wildcard;
    uint8  target;
    uint8  extended;
#endif
} mandolin_parameter_flags;

// List
typedef enum {
    MANDOLIN_LIST_DESTROY,
    MANDOLIN_LIST_CREATE
} mandolin_list_op;

// String
typedef uint32 mandolin_string_id;


//
// Parser
//

// Defines
#define MANDOLIN_HEADER_WORDS             2u
#define MANDOLIN_PAYLOAD_CHECK_WORDS      1u
#define MANDOLIN_MAX_MESSAGE_WORDS        (MANDOLIN_HEADER_WORDS + MANDOLIN_MAX_PAYLOAD_DATA_WORDS + MANDOLIN_PAYLOAD_CHECK_WORDS)
#define MANDOLIN_MIN_PAYLOAD_DATA_WORDS 125u
#define MANDOLIN_MIN_MESSAGE_WORDS        (MANDOLIN_HEADER_WORDS + MANDOLIN_MIN_PAYLOAD_DATA_WORDS + MANDOLIN_PAYLOAD_CHECK_WORDS)
#define MANDOLIN_BYTES_PER_WORD           4u
#define MANDOLIN_HEADER_BYTES             (MANDOLIN_BYTES_PER_WORD*MANDOLIN_HEADER_WORDS)
#define MANDOLIN_MAX_MESSAGE_BYTES        (MANDOLIN_BYTES_PER_WORD*MANDOLIN_MAX_MESSAGE_WORDS)
#define MANDOLIN_FIFO_BYTES               (MANDOLIN_MAX_MESSAGE_BYTES + MANDOLIN_BUFFER_SLOP_BYTES) // Should be smaller than FIFO_TYPE_MAX/2 to avoid overflow!


// Types & Structures

// Error
typedef struct {
    mandolin_error_code error;
#if defined(__ADSP21000__)
    uint32              sequence;
    uint32              transport;
    uint32              id;
#else
    uint8               sequence;
    uint8               transport;
    uint8               id;
#endif
} mandolin_parse_error;

// FIFO Types
#if defined(__ADSP21000__)
typedef uint32 mandolin_byte_type;          // In this case it is up to the user to insure 8-bit bytes!
#else
typedef uint8  mandolin_byte_type;
#endif
typedef uint32 mandolin_fifo_type;          // All indices and buffer sizes must use the same data type.
#define MANDOLIN_FIFO_TYPE_MAX 4294967295   // max 32-bit unsigned number

// FIFO
typedef struct {
    mandolin_fifo_type  write_i;
    mandolin_fifo_type  read_i;
    mandolin_fifo_type  buffer_bytes;
#if MANDOLIN_FIFO_IS_POWER_OF_TWO
    mandolin_fifo_type  buffer_mask;
#endif
#if DEBUG_FIFO_BUFFER
    mandolin_fifo_type  max_buffer_bytes;
#endif
    uint32              valid_header;
    mandolin_byte_type *buffer;
} mandolin_fifo;


// Macros
#if defined(__ADSP21000__)
#define PACK_SHORT(hi,lo) (((hi) << 8) | (lo))
#define PACK_LONG(hihi,hilo,lohi,lolo) (((hihi) << 24) | ((hilo) << 16) | ((lohi) << 8) | (lolo))

#define SHORT_LO(n) ( (n)       & 0x00FF)
#define SHORT_HI(n) (((n) >> 8) & 0x00FF)
#else
#define PACK_SHORT(hi,lo) ((((uint16) (hi)) << 8) | (uint16) (lo))
#define PACK_LONG(hihi,hilo,lohi,lolo) ((((uint32) (hihi)) << 24) | (((uint32) (hilo)) << 16) | (((uint32) (lohi)) << 8) | ((uint32) (lolo)))

#define SHORT_LO(n) ((uint8) ( (n)       & 0x00FF))
#define SHORT_HI(n) ((uint8) (((n) >> 8) & 0x00FF))
#endif


// Prototypes
#if ENABLE_MANDOLIN_TIMEOUT
uint32             MANDOLIN_FIFO_valid_stamp(mandolin_fifo *);
#endif
const char        *MANDOLIN_MSG_id_string(mandolin_message_id msgid);
void               MANDOLIN_MSG_print(mandolin_message *);
mandolin_fifo_type MANDOLIN_FIFO_max_bytes(mandolin_fifo *);
bool               MANDOLIN_FIFO_is_empty(mandolin_fifo *);
bool               MANDOLIN_FIFO_is_full(mandolin_fifo *);
mandolin_fifo_type MANDOLIN_FIFO_bytes_full(mandolin_fifo *);
mandolin_fifo_type MANDOLIN_FIFO_bytes_empty(mandolin_fifo *);
#if DEBUG_FIFO_BUFFER
mandolin_fifo_type MANDOLIN_FIFO_high_watermark(mandolin_fifo *);
#endif
bool               MANDOLIN_FIFO_init(mandolin_fifo *, mandolin_fifo_type, mandolin_byte_type *);
void               MANDOLIN_FIFO_reset(mandolin_fifo *);
#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
bool               MANDOLIN_FIFO_alloc(mandolin_fifo *, mandolin_fifo_type);
void               MANDOLIN_FIFO_free(mandolin_fifo *);
#endif
void               MANDOLIN_FIFO_enqueue_byte(mandolin_fifo *, mandolin_byte_type);
void               MANDOLIN_FIFO_enqueue_bytes(mandolin_fifo *, mandolin_byte_type *, mandolin_fifo_type);
mandolin_byte_type MANDOLIN_FIFO_dequeue_byte(mandolin_fifo *);
mandolin_fifo_type MANDOLIN_FIFO_dequeue_bytes(mandolin_fifo *, mandolin_byte_type *, mandolin_fifo_type);
mandolin_byte_type MANDOLIN_FIFO_get_byte(mandolin_fifo *, mandolin_fifo_type);
mandolin_fifo_type MANDOLIN_FIFO_dump_bytes(mandolin_fifo *, mandolin_fifo_type);

mandolin_fifo_type MANDOLIN_FIFO_parse(mandolin_fifo *, mandolin_parse_error *);

mandolin_fifo_type MANDOLIN_MSG_bytes(mandolin_message *);
void               MANDOLIN_MSG_erase(mandolin_fifo *);
mandolin_fifo_type MANDOLIN_MSG_read(mandolin_fifo *, mandolin_message *, mandolin_byte_type *);
#if DYNAMIC_MEMORY_ALLOCATION_AVAILABLE
bool               MANDOLIN_MSG_copy(mandolin_fifo *, mandolin_message **);
bool               MANDOLIN_MSG_alloc(mandolin_message **, mandolin_fifo_type);
void               MANDOLIN_MSG_free(mandolin_message *);
#endif
void               MANDOLIN_MSG_pack(mandolin_message *, mandolin_byte_type);
void               MANDOLIN_MSG_stuff_sequence(mandolin_message *, mandolin_byte_type);
mandolin_fifo_type MANDOLIN_MSG_write(mandolin_byte_type *, mandolin_message *);
bool               MANDOLIN_MSG_enqueue(bool (*fifo_enqueue)(mandolin_byte_type), mandolin_message *);

#endif // __MANDOLIN_H__
