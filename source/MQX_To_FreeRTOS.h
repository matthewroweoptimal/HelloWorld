#ifndef __MQX_TO_FREERTOS_h__
#define __MQX_TO_FREERTOS_h__

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"



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

// msg_ptr [IN] — Pointer to the message to be freed
void _msg_free(void* msg_ptr);



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

// msg_ptr IN] � Pointer to the message to be sent
// Returns  TRUE (success: see description)
//          FALSE (failure)
bool _msgq_send(void *msg_ptr);

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


typedef _mqx_uint  _timer_id;
#define MQX_INT_SIZE_IN_BITS (32)
#define MQX_MIN_BITS_IN_TICK_STRUCT (64)
#define MQX_NUM_TICK_FIELDS  ((MQX_MIN_BITS_IN_TICK_STRUCT + MQX_INT_SIZE_IN_BITS-1) / MQX_INT_SIZE_IN_BITS)

/*!
 * \brief This structure defines how time is maintained in the system.
 *
 * Time is kept internally in the form of ticks. This is a 64 bit
 * field which is maintained in an array whose size is
 * dependent upon the PSP. HW_TICKS is used to track time between
 * ticks (timer interrupts).
 */
typedef struct mqx_tick_struct
{    /*!
     * \brief Ticks since MQX started.
     *
     * The field is a minimum of 64 bits; the exact size depends on the PSP.
     */
    _mqx_uint TICKS[MQX_NUM_TICK_FIELDS];

    /*!
     * \brief Hardware ticks (timer counter increments) between ticks.
     *
     * The field increases the accuracy over counting the time simply in ticks.
     */
    uint32_t   HW_TICKS;
} MQX_TICK_STRUCT, * MQX_TICK_STRUCT_PTR;
typedef void (* TIMER_NOTIFICATION_TICK_FPTR)( _timer_id, void *, MQX_TICK_STRUCT_PTR);

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

/*!
 * \brief This structure defines a lightweight event.
 *
 * Tasks can wait on and set event bits.
 *
 * \see _lwevent_clear
 * \see _lwevent_create
 * \see _lwevent_destroy
 * \see _lwevent_set
 * \see _lwevent_set_auto_clear
 * \see _lwevent_usr_check
 * \see _lwevent_wait_for
 * \see _lwevent_wait_ticks
 * \see _lwevent_wait_until
 * \see _usr_lwevent_clear
 * \see _usr_lwevent_create
 * \see _usr_lwevent_destroy
 * \see _usr_lwevent_set
 * \see _usr_lwevent_set_auto_clear
 * \see _usr_lwevent_wait_for
 * \see _usr_lwevent_wait_ticks
 * \see _usr_lwevent_wait_until
 */
typedef struct lwevent_struct
{    /*! \brief Queue data structures. */
    QUEUE_ELEMENT_STRUCT LINK;

    /*! \brief Queue of tasks waiting for event bits to be set. */
    QUEUE_STRUCT WAITING_TASKS;

    /*! \brief Validation stamp. */
    _mqx_uint VALID;

    /*! \brief Current bit value of the lightweight event. */
    _mqx_uint VALUE;

    /*! \brief Flags associated with the light weight event. */
    _mqx_uint FLAGS;

    /*! \brief Mask specifying lightweight event bits that are configured as auto-clear. */
    _mqx_uint AUTO;
}LWEVENT_STRUCT, * LWEVENT_STRUCT_PTR;

_mqx_uint _lwevent_wait_ticks(LWEVENT_STRUCT_PTR, _mqx_uint, bool, _mqx_uint);
_mqx_uint _lwevent_get_signalled(void);
_mqx_uint _lwevent_set(LWEVENT_STRUCT_PTR, _mqx_uint);
_mqx_uint _lwevent_clear(LWEVENT_STRUCT_PTR, _mqx_uint);

_mqx_uint _event_create(char *);
_mqx_uint _event_open(char *, void **);
_mqx_uint _event_wait_any(void *, _mqx_uint, uint32_t);
_mqx_uint _event_get_value(void *, _mqx_uint_ptr);

_mqx_uint _task_set_error(_mqx_uint);


/*                         LWSEM STRUCTURE */
/*!
 * \brief Lightweight semaphore.
 *
 * This structure defines a lightweight semaphore.
 * \n These sempahores implement a simple counting semaphore.
 * \n Tasks wait on these semaphores in a FIFO manner.
 * \n Priority inheritance is NOT implemented for these semaphores.
 * \n The semaphores can be embedded into data structures similarly to mutexes.
 *
 * \see _lwsem_create
 * \see _lwsem_create_hidden
 * \see _lwsem_destroy
 * \see _lwsem_poll
 * \see _lwsem_post
 * \see _lwsem_usr_check
 * \see _lwsem_wait
 * \see _lwsem_wait_for
 * \see _lwsem_wait_ticks
 * \see _lwsem_wait_until
 * \see _usr_lwsem_create
 * \see _usr_lwsem_destroy
 * \see _usr_lwsem_poll
 * \see _usr_lwsem_post
 * \see _usr_lwsem_wait
 * \see _usr_lwsem_wait_for
 * \see _usr_lwsem_wait_ticks
 * \see _usr_lwsem_wait_until
 */
typedef struct lwsem_struct
{    /* The next two fields are used to maintain a list of all LWSEMS */

    /*! \brief Pointer to the next lightweight semaphore in the list of lightweight
     *  semaphores. */
    struct lwsem_struct       *NEXT;

    /*! \brief Pointer to the previous lightweight semaphore in the list of
     *  lightweight semaphores. */
    struct lwsem_struct       *PREV;

    /*! \brief  Manages the queue of tasks that are waiting for the lightweight
     *  semaphore. The NEXT and PREV fields in the task descriptors link the tasks. */
    QUEUE_STRUCT               TD_QUEUE;

    /*! \brief When MQX creates the lightweight semaphore, it initializes the field.
     *  When MQX destroys the lightweight semaphore, it clears the field. */
    _mqx_uint                  VALID;

    /*! \brief Count of the semaphore. MQX decrements the field when a task waits
     *  for the semaphore. If the field is not 0, the task gets the semaphore. If
     *  the field is 0, MQX puts the task in the lightweight semaphore queue until
     *  the count is a non-zero value.the semaphore value. */
    _mqx_int                   VALUE;
} LWSEM_STRUCT, * LWSEM_STRUCT_PTR;

_mqx_uint _lwsem_wait(LWSEM_STRUCT_PTR);
_mqx_uint _lwsem_post(LWSEM_STRUCT_PTR);


// TASK FUNCTIONS :
//=================
_mqx_uint _task_get_error(void);
_mqx_uint* _task_get_error_ptr(void);
#define _task_errno  (*_task_get_error_ptr())

_mqx_uint _task_set_error(_mqx_uint);
void _task_block(void);


// -----------------------------------------------------------------------------------------------

#endif  // __MQX_TO_FREERTOS_h__
