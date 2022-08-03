#ifndef __MQX_TO_FREERTOS_h__
#define __MQX_TO_FREERTOS_h__

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include <stdbool.h>
#include "event_groups.hpp"



typedef int32_t   _mqx_int, * _mqx_int_ptr;
typedef uint32_t  _mqx_uint, * _mqx_uint_ptr;

/*
 *                          MXQ ERROR CODES
 */
#define MQX_OK                              (0)

#define MQX_ERROR_BASE                      (0x00000000ul)
#define POSIX_ERROR_BASE                    (MQX_ERROR_BASE | 0x0400)

#define MQX_OUT_OF_MEMORY                   (MQX_ERROR_BASE|0x04)

#define MQX_EINVAL                          (POSIX_ERROR_BASE|0x10)




/// MESSAGE POOLS :
///================
typedef void   *_pool_id;           /* what a pool_id looks like */
typedef uint16_t _msg_size;          /* what a message size looks like */
/* What a NULL pool id looks like.  Used for error returns from functions */
#define MSGPOOL_NULL_POOL_ID     ((_pool_id)0)

// Creates a private message pool.
// message_size [IN]    Size (in single-addressable units) of the messages (including the message header)
//                      to be created for the message pool
// num_messages [IN]    Initial number of messages to be created for the message pool
// grow_number [IN]     Number of messages to be added if all the messages are allocated
// grow_limit [IN]      If grow_number is not equal to 0; one of the following:
//                          maximum number of messages that the pool can have
//                          0 (unlimited growth)
// Returns
//   Pool ID to access the message pool (success)
//   0 (failure)
_pool_id _msgpool_create(uint16_t message_size, uint16_t num_messages, uint16_t grow_number, uint16_t grow_limit);

// pool_id [IN] � A pool ID from _msgpool_create()
// Returns  Pointer to a message (success)
//          NULL (failure)
void *_msg_alloc(_pool_id pool_id);

// pool_id [IN] A pool ID from _msgpool_create()
//				>>>> NEW parameter introduced to make things work. Code changes for this function therefore required. <<<<
// msg_ptr [IN] — Pointer to the message to be freed
void _msg_free(_pool_id pool_id, void* msg_ptr);
// ORIGINAL Prototype : void _msg_free(void* msg_ptr);



/// MESSAGE QUEUES :
///=================
typedef uint16_t _processor_number;  /* what a processor number is */
typedef uint16_t _queue_number;      /* what a queue number is         */
// typedef uint16_t _queue_id;          /* What a queue_id looks like */
// Need to make _queue_id 32 bits to be able to hold pointers
typedef QueueHandle_t _queue_id;          /* What a queue_id looks like */

/* MESSAGE HEADER STRUCT */
/*!
 * \brief This structure defines the first field (header) of any message.
 *
 * Any pool of messsages must have at least this size and must start with a
 * message header.
 */
typedef struct message_header_struct
{   /*!
    * \brief The size of the DATA field in bytes, including the header. The
    * maximum value is MAX_MESSAGE_SIZE. This field is set by the application.
    */
   _msg_size       SIZE;

#if MQX_USE_32BIT_MESSAGE_QIDS
   uint16_t         PAD;
#endif

   /*!
    * \brief Queue ID of the queue to which MQX is to send the message. This
    * field is set by the application.
    */
   _queue_id       TARGET_QID;

   /*!
    * \brief Queue ID of a message queue that is associated with the sending
    * task.
    *
    * When messages are allocated, this field is initialized to MSGQ_NULL_QUEUE_ID.
    * If the sending task does not have a message queue associated with it, MQX
    * does not use this field.
    */
   _queue_id       SOURCE_QID;

   /*!
    * \brief Indicates the following for the message: endian format, priority
    * and urgency.
    */
   unsigned char           CONTROL;

#if MQX_USE_32BIT_MESSAGE_QIDS
   /*! \brief Not used. */
   unsigned char           RESERVED[3];
#else
   /*! \brief Not used. */
   unsigned char           RESERVED;
#endif
} MESSAGE_HEADER_STRUCT, * MESSAGE_HEADER_STRUCT_PTR;


// processor_number [IN] � One of the following:
//          processor on which the message queue resides
//          0 (indicates the local processor)
// queue_number [IN] � Image-wide unique number that identifies the message queue
// Returns  Queue ID for the queue (success)
//          MSGQ_NULL_QUEUE_ID (failure: _processor_number is not valid)
_queue_id _msgq_get_id(_processor_number processor_number, _queue_number queue_number);

// queue_number [IN] � One of the following:
//          queue number of the message queue to be opened on this processor (min. 8, max. as defined in
//          the MQX RTOS initialization structure)
//          MSGQ_FREE_QUEUE (MQX RTOS opens an unopened message queue)
// max_queue_size [IN] � One of the following:
//          maximum queue size
//          0 (unlimited size)
// Returns  Queue ID (success)
//          MSGQ_NULL_QUEUE_ID (failure)
_queue_id _msgq_open( _queue_number queue_number, uint16_t max_queue_size );

// queue_id Private or system message queue from which to receive a message
// Returns  Pointer to a message (success)
//          NULL (failure)
void *_msgq_poll( _queue_id queue_id );

// queue_id [IN] 	Private message queue from which to receive a message
//                  >>>> NOTE : New addition to prototype, therefore code changes required. <<<<
// msg_ptr IN] � Pointer to the message to be sent
// Returns  TRUE (success: see description)
//          FALSE (failure)
bool _msgq_send(_queue_id queue_id, void *msg_ptr);
// ORIGINAL Prototype : bool _msgq_send(void *msg_ptr);

// queue_id [IN] � One of the following:
//                  private message queue from which to receive a message
//                  MSGQ_ANY_QUEUE (any queue that the task owns)
// ms_timeout [IN] � One of the following:
//                  maximum number of milliseconds to wait. After the timeout elapses without the message, the function returns.
//                  0 (unlimited wait)
// Returns      Pointer to a message (success)
//              NULL (failure)
void*_msgq_receive(_queue_id queue_id, uint32_t ms_timeout);

// queue_id [IN] � One of the following:
// 			queue ID of the queue to be checked
//			MSGQ_ANY_QUEUE (get the number of messages waiting in all message queues that the task has open)
// Returns
//		Number of messages (success)
//		0 (success: queue is empty)
//		0 (failure)
_mqx_uint _msgq_get_count(_queue_id);




/// MUTEX FUNCTIONS :
///==================
// -----------------------------------------------------------------------------------------------
/*!
 * \brief Mutex attributes, which are used to initialize a mutex : Not used by FreeRTOS, so just a dummy structure
 */
typedef struct mutex_attr_struct
{
    _mqx_uint dummy;
} MUTEX_ATTR_STRUCT, * MUTEX_ATTR_STRUCT_PTR;

// Map MQX to FreeRTOS Mutex
#define MUTEX_STRUCT     struct QueueDefinition
#define MUTEX_STRUCT_PTR SemaphoreHandle_t

// attr_ptr [IN]    Pointer to the mutex attributes structure to initialize
// Returns
//      MQX_EOK (success)
//      MQX_EINVAL (failure: attr_ptr is NULL)
_mqx_uint _mutatr_init(MUTEX_ATTR_STRUCT_PTR attr_ptr);

// Returns
//      MQX_OK (success)
//      MQX_OUT_OF_MEMORY (failure)
_mqx_uint _mutex_init(MUTEX_STRUCT_PTR pMutex, MUTEX_ATTR_STRUCT_PTR attr_ptr);

// mutex_ptr [IN] — Pointer to the mutex to be locked
// Returns
//          MQX_EOK
//          Errors
_mqx_uint _mutex_lock(MUTEX_STRUCT_PTR pMutex);

// mutex_ptr [IN] — Pointer to the mutex to be locked
// Returns
//          MQX_EOK
//          Errors
_mqx_uint _mutex_unlock(MUTEX_STRUCT_PTR pMutex);


/// SEMAPHORE FUNCTIONS :
///======================
/*                         LWSEM STRUCTURE */

// Map MQX to FreeRTOS Semaphore
#define LWSEM_STRUCT     struct QueueDefinition
#define LWSEM_STRUCT_PTR SemaphoreHandle_t

_mqx_uint _lwsem_create(LWSEM_STRUCT_PTR sem_ptr, _mqx_int initial_number);
_mqx_uint _lwsem_wait(LWSEM_STRUCT_PTR sem_ptr);
_mqx_uint _lwsem_post(LWSEM_STRUCT_PTR sem_ptr);



/// TIMER FUNCTIONS :
///==================
//typedef _mqx_uint  _timer_id;
typedef TimerHandle_t	_timer_id;

#define MQX_INT_SIZE_IN_BITS (32)
#define MQX_MIN_BITS_IN_TICK_STRUCT (64)
#define MQX_NUM_TICK_FIELDS  ((MQX_MIN_BITS_IN_TICK_STRUCT + MQX_INT_SIZE_IN_BITS-1) / MQX_INT_SIZE_IN_BITS)

/*
 * This mode tells the timer to use the elapsed time when calculating time
 * (_time_get_elapsed)
 */
#define TIMER_ELAPSED_TIME_MODE       (1)

/*!
 * \brief This structure defines how time is maintained in the system.
 *
 * Time is kept internally in the form of ticks. This is a 64 bit
 * field which is maintained in an array whose size is
 * dependent upon the PSP. HW_TICKS is used to track time between
 * ticks (timer interrupts).
 */
typedef struct mqx_tick_struct
{
	TickType_t ticks;		// Simplified to match FreeRTOS - NOTE : Only ms resolution supported
} MQX_TICK_STRUCT, * MQX_TICK_STRUCT_PTR;


//typedef void (* TIMER_NOTIFICATION_TICK_FPTR)( _timer_id, void *, MQX_TICK_STRUCT_PTR);
typedef void (* TIMER_NOTIFICATION_TICK_FPTR)( _timer_id );

_timer_id  _timer_start_periodic_every_ticks(TIMER_NOTIFICATION_TICK_FPTR, void *, _mqx_uint, MQX_TICK_STRUCT_PTR);
_mqx_uint  _timer_cancel(_timer_id);
_timer_id  _timer_start_oneshot_after_ticks(TIMER_NOTIFICATION_TICK_FPTR, void *, _mqx_uint, MQX_TICK_STRUCT_PTR);

_mqx_uint _time_init_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
MQX_TICK_STRUCT_PTR  _time_add_msec_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
MQX_TICK_STRUCT_PTR  _time_add_sec_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
void _time_get_elapsed_ticks(MQX_TICK_STRUCT_PTR);
int32_t _time_diff_milliseconds(MQX_TICK_STRUCT_PTR, MQX_TICK_STRUCT_PTR, bool *);
void _time_delay(uint32_t);

bool _klog_display(void);



/// EVENT FUNCTIONS :
///==================
#if 0
typedef struct queue_element_struct
{
    /*! \brief Pointer to the next element in the queue. */
    struct queue_element_struct      *NEXT;

    /*! \brief Pointer to the previous element in the queue. */
    struct queue_element_struct      *PREV;
} QUEUE_ELEMENT_STRUCT, * QUEUE_ELEMENT_STRUCT_PTR;
typedef struct queue_struct
{
    /*!
     * \brief Pointer to the next element in the queue. If there are no elements
     * in the queue, the field is a pointer to the structure itself.
     */
    struct queue_element_struct      *NEXT;

    /*
     * \brief Pointer to the last element in the queue. If there are no elements
     * in the queue, the field is a pointer to the structure itself.
     */
    struct queue_element_struct      *PREV;

    /*! \brief Number of elements in the queue. */
    uint16_t                           SIZE;

    /*!
     * \brief Maximum number of elements that the queue can hold. If the field
     * is 0, the number is unlimited.
     */
    uint16_t                           MAX;

} QUEUE_STRUCT, * QUEUE_STRUCT_PTR;
#endif

/* Creation flags */
#define LWEVENT_AUTO_CLEAR          (0x00000001)

/* Error code */
#define EVENT_ERROR_BASE       		(MQX_ERROR_BASE | 0x0300)
#define LWEVENT_WAIT_TIMEOUT        (EVENT_ERROR_BASE|0x10)

typedef struct lwevent_struct
{
	cpp_freertos::EventGroup	*pEventGroup;	// Mapping to FreeRTOS equivalent
	_mqx_uint 					flags; 			//	LWEVENT_AUTO_CLEAR - all bits in the lightweight event group are made autoclearing
												//	0 - lightweight event bits are not set as autoclearing by default
}LWEVENT_STRUCT, * LWEVENT_STRUCT_PTR;

_mqx_uint _lwevent_create(LWEVENT_STRUCT_PTR lwevent_group_ptr, _mqx_uint flags);
_mqx_uint _lwevent_wait_ticks(LWEVENT_STRUCT_PTR, _mqx_uint, bool, _mqx_uint);
_mqx_uint _lwevent_get_signalled(void);
_mqx_uint _lwevent_set(LWEVENT_STRUCT_PTR, _mqx_uint);
_mqx_uint _lwevent_clear(LWEVENT_STRUCT_PTR, _mqx_uint);

/* The maximum name size for a name component name */
#define NAME_MAX_NAME_SIZE          (32)

#define NAME_ERROR_BASE        		(MQX_ERROR_BASE | 0x0F00)
#define NAME_NOT_FOUND         		(NAME_ERROR_BASE|0x02)

typedef struct event_struct
{
	char            			name[NAME_MAX_NAME_SIZE];
	cpp_freertos::EventGroup	*pEventGroup;	// Mapping to FreeRTOS equivalent
	bool 						autoClear; 		//	true - all bits in the lightweight event group are made autoclearing
}EVENT_STRUCT, * EVENT_STRUCT_PTR;

_mqx_uint _event_create(char *);
_mqx_uint _event_open(char *, void **);
_mqx_uint _event_wait_any(void *, _mqx_uint, uint32_t);
_mqx_uint _event_get_value(void *, _mqx_uint_ptr);

_mqx_uint _task_set_error(_mqx_uint);



// TASK FUNCTIONS :
//=================
_mqx_uint _task_get_error(void);
_mqx_uint* _task_get_error_ptr(void);
#define _task_errno  (*_task_get_error_ptr())

_mqx_uint _task_set_error(_mqx_uint);
void _task_block(void);


// -----------------------------------------------------------------------------------------------

#endif  // __MQX_TO_FREERTOS_h__
