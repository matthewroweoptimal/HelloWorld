/*
 * MQX_To_FreeRTOS.cpp
 *
 *  This file provides a basic translation of MQX RTOS services used by the CDDLive project
 *  onto equivalent FreeRTOS calls. This allows for minimal code chanegs to the CDDLive project
 *  in order to reduce risks with too much re-coding.
 *
 *  Most MQX calls are unmodified, but there are a couple of instances where slight modification
 *  has been made in order to work with FreeRTOS :
 *      _msg_free					 : Added parameter to reference associated message pool
 *      Timer notification callbacks : Prototype simplified to take 1 parameter
 *
 *  It has been necessary to code in Some knowledge of the CDDLive system for message pools,
 *  however this could be simplified if only a single message pool is ever used (i.e. no IRDA pools)
 */
#include "MQX_to_FreeRTOS.h"
#include "mem_pool.hpp"
#include "string.h"


//#define DEBUG_RTOS_TRANSLATION		1       // For Debug output on translation functions
#define POINTERS_PASSED_IN_MSG_QS	1		// Much more efficient for memory usage and speed (avoids copying of data)
#if DEBUG_RTOS_TRANSLATION
	static TaskStatus_t xTaskDetails;
#endif

// -----------------------------------------------------------------------------------------------
/// MESSAGE POOLS :
///================
// We support 5 Message POOLs : Mandolin MSG Pool + 4 x IRDA message pools
#define MAX_MSG_POOLS			5
#define POOL_MANDOLIN			0
#define POOL_IRDA_A				1
#define POOL_IRDA_B				2
#define POOL_IRDA_C				3
#define POOL_IRDA_D				4

static uint16_t s_poolIndex = 0;
static struct PoolDetails
{	// Remember details of message POOLs created. (First will be Mandolin Pool, followed by 4 x IRDA pools)
	// TODO : Scope for simplification if the 4 IRDA pools aren't required, so then just 1 pool in the system (MANDOLIN_POOL).
	_pool_id	msgPoolId;
	uint16_t	msgPoolItemSize;
	uint16_t	msgPoolItemCount;
} s_msgPoolDetails[MAX_MSG_POOLS] = {0};


// We support 9 Q's : TCP_QUEUE, CONFIG_QUEUE, NET_TX_QUEUE, DSP_QUEUE, IRDA_A_QUEUE, IRDA_B_QUEUE, IRDA_C_QUEUE, IRDA_D_QUEUE, UART_QUEUE
#define MAX_MSG_QUEUES			9
/* Message Queues */
#define X_TCP_QUEUE			8		// Redefine from CommMandolin.h (renamed so as not to clash)
#define X_CONFIG_QUEUE		9
#define X_NET_TX_QUEUE		10
#define X_DSP_QUEUE			11
#define X_IRDA_A_QUEUE		12
#define X_IRDA_B_QUEUE		13
#define X_IRDA_C_QUEUE		14
#define X_IRDA_D_QUEUE		15
#define X_UART_QUEUE		16

#define MAX_Q_SIZE_SUPPORTED	10	// TODO : Review what Q size we can handle
static struct
{	// Remember details of message Qs created
	_queue_id	msgQId;
	_pool_id	msgPoolId;
} s_msgQDetails[MAX_MSG_QUEUES] = {0};


static struct PoolDetails* getPoolFromQIndex( uint16_t qIndex )
{
	for ( uint32_t pool = 0; pool < MAX_MSG_POOLS; pool++ )
	{
		if ( s_msgPoolDetails[pool].msgPoolId == s_msgQDetails[qIndex].msgPoolId )
			return &s_msgPoolDetails[pool];
	}
	configASSERT( 0 );	// Not found
	return NULL;
}

static struct PoolDetails* getPoolFromQId( _queue_id msgQId )
{
	for ( uint32_t q = 0; q < MAX_MSG_QUEUES; q++ )
	{
		if ( s_msgQDetails[q].msgQId == msgQId )
		{
			return getPoolFromQIndex( q );
		}
	}
	configASSERT( 0 );	// Not found
	return NULL;
}


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
_pool_id _msgpool_create(uint16_t message_size, uint16_t num_messages, uint16_t grow_number, uint16_t grow_limit)
{
    // Use MemoryPool from mem_pool.hpp ?
    int itemSize = message_size;
    int itemCount = num_messages;
    int alignment = 4;
    s_msgPoolDetails[s_poolIndex].msgPoolId = new cpp_freertos::MemoryPool( itemSize, itemCount, alignment );
    s_msgPoolDetails[s_poolIndex].msgPoolItemSize = itemSize;
    s_msgPoolDetails[s_poolIndex].msgPoolItemCount = itemCount;
#if DEBUG_RTOS_TRANSLATION
	// Use the handle to obtain further information about the task.
	vTaskGetInfo( NULL, &xTaskDetails, pdTRUE, eInvalid );
	printf("Task '%s' (HWM %d): MSG POOL %p CREATE, size %d, count %d\n", xTaskDetails.pcTaskName, xTaskDetails.usStackHighWaterMark, s_msgPoolDetails[s_poolIndex].msgPoolId, itemSize, itemCount );
#endif
    return s_msgPoolDetails[s_poolIndex++].msgPoolId;
}

// pool_id [IN] A pool ID from _msgpool_create()
// Returns  Pointer to a message (success)
//          NULL (failure)
void *_msg_alloc(_pool_id pool_id)
{
	cpp_freertos::MemoryPool* pMemPool = static_cast<cpp_freertos::MemoryPool*>( pool_id );
    void *pMem = pMemPool->Allocate();
#if 0 // DEBUG_RTOS_TRANSLATION
	// Use the handle to obtain further information about the task.
	vTaskGetInfo( NULL, &xTaskDetails, pdTRUE, eInvalid );
	printf("Task '%s' (HWM %d): MSG POOL %p ALLOC %p\n", xTaskDetails.pcTaskName, xTaskDetails.usStackHighWaterMark, pool_id, pMem );
#endif
	return pMem;
}

// msgQId [IN] A message Q ID which can be used to identify the message pool
//				>>>> NEW parameter introduced to make things work. Code changes for this function therefore required. <<<<
// msg_ptr [IN]	Pointer to the message to be freed
void _msg_free(_queue_id msgQId, void* msg_ptr)
{
	struct PoolDetails *pPoolDetails = getPoolFromQId( msgQId );
	_pool_id pool_id = pPoolDetails->msgPoolId;
	cpp_freertos::MemoryPool* pMemPool = static_cast<cpp_freertos::MemoryPool*>( pool_id );
	pMemPool->Free( msg_ptr );
#if 0 // DEBUG_RTOS_TRANSLATION
	// Use the handle to obtain further information about the task.
	vTaskGetInfo( NULL, &xTaskDetails, pdTRUE, eInvalid );
	printf("Task '%s' (HWM %d): MSG POOL %p FREE %p\n", xTaskDetails.pcTaskName, xTaskDetails.usStackHighWaterMark, pool_id, msg_ptr );
#endif
}



// -----------------------------------------------------------------------------------------------
/// MESSAGE QUEUES :
///=================

// processor_number [IN] One of the following:
//          processor on which the message queue resides
//          0 (indicates the local processor)
// queue_number [IN] Image-wide unique number that identifies the message queue
// Returns  Queue ID for the queue (success)
//          MSGQ_NULL_QUEUE_ID (failure: _processor_number is not valid)
_queue_id _msgq_get_id(_processor_number processor_number, _queue_number queue_number)
{
	return s_msgQDetails[queue_number-X_TCP_QUEUE].msgQId;
}

// queue_number [IN] One of the following:
//          queue number of the message queue to be opened on this processor (min. 8, max. as defined in
//          the MQX RTOS initialization structure)
//          MSGQ_FREE_QUEUE (MQX RTOS opens an unopened message queue)
// max_queue_size [IN] One of the following:
//          maximum queue size
//          0 (unlimited size)
// Returns  Queue ID (success)
//          MSGQ_NULL_QUEUE_ID (failure)
_queue_id _msgq_open( _queue_number queue_number, uint16_t max_queue_size )
{
    // FreeRTOS equivalent :
    // uxQueueLength    The maximum number of items that the queue being created can hold at any one time.
    // uxItemSize       The size in bytes of each data item that can be stored in the queue.
    // Return Value     If NULL is returned, then the queue cannot be created because there is insufficient heap memory available for FreeRTOS
    //                  to allocate the queue data structures and storage area.
    //                  A non-NULL value being returned indicates that the queue has been created successfully. The returned value should be stored as the handle to the created queue.
	if ( max_queue_size == 0 )
	{	// TODO : 0 is unlimited size. FreeRTOS has no equivalent, so set to Max we support
		max_queue_size = MAX_Q_SIZE_SUPPORTED;
	}

	struct PoolDetails* pPoolDetails = NULL;
	switch ( queue_number )
	{	// Manually matching MQX message queues to their associated Message pools
	case X_TCP_QUEUE :
	case X_CONFIG_QUEUE :
	case X_NET_TX_QUEUE :
	case X_DSP_QUEUE :
	case X_UART_QUEUE :
		pPoolDetails = &s_msgPoolDetails[POOL_MANDOLIN];
		break;

	case X_IRDA_A_QUEUE :
		pPoolDetails = &s_msgPoolDetails[POOL_IRDA_A];
		break;
	case X_IRDA_B_QUEUE :
		pPoolDetails = &s_msgPoolDetails[POOL_IRDA_B];
		break;
	case X_IRDA_C_QUEUE :
		pPoolDetails = &s_msgPoolDetails[POOL_IRDA_C];
		break;
	case X_IRDA_D_QUEUE :
		pPoolDetails = &s_msgPoolDetails[POOL_IRDA_D];
		break;

	default:
		configASSERT( 0 );
	}

	uint16_t msgQIndex = queue_number - X_TCP_QUEUE;
	UBaseType_t uxQueueLength = pPoolDetails->msgPoolItemCount;			// Q length matches associated Memory pool
#if POINTERS_PASSED_IN_MSG_QS
	UBaseType_t uxItemSize = sizeof (void*);							// Q Item Size is a pointer
#else
	UBaseType_t uxItemSize = pPoolDetails->msgPoolItemSize;				// Q Item Size matches associated Memory pool
#endif
	QueueHandle_t handle = xQueueCreate( uxQueueLength, uxItemSize );	// handle is 32 bit pointer !!
	s_msgQDetails[msgQIndex].msgQId = handle;
	s_msgQDetails[msgQIndex].msgPoolId = pPoolDetails->msgPoolId;

#if DEBUG_RTOS_TRANSLATION
	// Use the handle to obtain further information about the task.
	vTaskGetInfo( NULL, &xTaskDetails, pdTRUE, eInvalid );
	printf("Task '%s' (HWM %d): MSGQ %p OPEN, size %d, count %d\n", xTaskDetails.pcTaskName, xTaskDetails.usStackHighWaterMark, handle, uxItemSize, uxQueueLength );
#endif
	return handle;
}

// queue_id Private or system message queue from which to receive a message
// Returns  Pointer to a message (success)
//          NULL (failure)
void *_msgq_poll( _queue_id queue_id )
{
    // FreeRTOS equivalent :
    // xQueue    The handle of the queue from which the data is being received (read). The queue handle will have been returned from the call to xQueueCreate() used to create the queue.
    // pvBuffer  A pointer to the memory into which the received data will be copied.
    //           The size of each data item that the queue holds is set when the queue is created.
	//			 The memory pointed to by pvBuffer must be at least large enough to hold that many bytes.
    // xTicksToWait  The maximum amount of time the task should remain in the Blocked state to wait for data to become available on the queue, should the queue already be empty.
    //               If xTicksToWait is zero, then xQueueReceive() will return immediately if the queue is already empty.
    // RETURNS :  pdPASS    pdPASS will be returned only if data was successfully read from the queue.
    //                      If a block time was specified (xTicksToWait was not zero), then it is possible the calling task was placed into the Blocked state, to wait for data to become available on the queue, but data was successfully read from the queue before the block time expired.
    //            errQUEUE_EMPTY    errQUEUE_EMPTY will be returned if data cannot be read from the queue because the queue is already empty.
	QueueHandle_t xQueue = queue_id;
	struct PoolDetails *pPoolDetails = getPoolFromQId( queue_id );
	_pool_id poolId = pPoolDetails->msgPoolId;
	TickType_t xTicksToWait = 0;
#if POINTERS_PASSED_IN_MSG_QS
	void * pvBuffer;
    BaseType_t result = xQueueReceive( xQueue, &pvBuffer, xTicksToWait );
#else
	void * pvBuffer = _msg_alloc( poolId );
    BaseType_t result = xQueueReceive( xQueue, pvBuffer, xTicksToWait );
#endif


    if ( result != pdPASS )
    {	// Queue is empty, free buffer and return NULL result
	#if !POINTERS_PASSED_IN_MSG_QS
	_msg_free( queue_id, pvBuffer );
	#endif
    	return NULL;
    }
#if DEBUG_RTOS_TRANSLATION
	// Use the handle to obtain further information about the task.
	vTaskGetInfo( NULL, &xTaskDetails, pdTRUE, eInvalid );
	printf("Task '%s' (HWM %d): MSGQ %p POLL, item %p\n", xTaskDetails.pcTaskName, xTaskDetails.usStackHighWaterMark, xQueue, pvBuffer );
#endif
    return pvBuffer;
}

// queue_id [IN] 	Private message queue from which to receive a message
//                  >>>> NOTE : New addition to prototype, therefore code changes required. <<<<
// msg_ptr IN] Pointer to the message to be sent
// Returns  TRUE (success: see description)
//          FALSE (failure)
bool _msgq_send(_queue_id queue_id, void *msg_ptr)
{
    // FreeRTOS equivalent :
    // xQueue   The handle of the queue to which the data is being sent (written). The queue handle will have been returned from the call to xQueueCreate() used to create the queue.
    // pvItemToQueue    A pointer to the data to be copied into the queue.
    //                  The size of each item that the queue can hold is set when the queue is created, so this many bytes will be copied from pvItemToQueue into the queue storage area.
    // xTicksToWait     The maximum amount of time the task should remain in the Blocked state to wait for space to become available on the queue, should the queue already be full.
    //                  Will return immediately if xTicksToWait is zero and the queue is already full.
    // RETURN :         pdPASS  pdPASS will be returned only if data was successfully sent to the queue.
    //                  If a block time was specified (xTicksToWait was not zero), then it is possible the calling task was placed into the Blocked state, to wait for space to become available in the queue, before the function returned, but data was successfully written to the queue before the block time expired.
    //                  errQUEUE_FULL   errQUEUE_FULL will be returned if data could not be written to the queue because the queue was already full.
	QueueHandle_t xQueue = queue_id;
#if POINTERS_PASSED_IN_MSG_QS
	const void * pvItemToQueue = &msg_ptr;
#else
	const void * pvItemToQueue = msg_ptr;
#endif
    TickType_t xTicksToWait = 0;
    BaseType_t result = xQueueSendToBack( xQueue, pvItemToQueue, xTicksToWait );
#if DEBUG_RTOS_TRANSLATION
	// Use the handle to obtain further information about the task.
	vTaskGetInfo( NULL, &xTaskDetails, pdTRUE, eInvalid );
	printf("Task '%s' (HWM %d): MSGQ %p SEND, '%s', item %p\n", xTaskDetails.pcTaskName, xTaskDetails.usStackHighWaterMark, xQueue, (result == pdPASS)?"PASS":"FAIL", pvItemToQueue );
#endif
    return (result == pdPASS) ? TRUE : FALSE;
}

// queue_id [IN] ï¿½ One of the following:
//                  private message queue from which to receive a message
//                  MSGQ_ANY_QUEUE (any queue that the task owns)
// ms_timeout [IN] ï¿½ One of the following:
//                  maximum number of milliseconds to wait. After the timeout elapses without the message, the function returns.
//                  0 (unlimited wait)
// Returns      Pointer to a message (success)
//              NULL (failure)
void*_msgq_receive(_queue_id queue_id, uint32_t ms_timeout)
{
    // FreeRTOS equivalent :
    // xQueue    The handle of the queue from which the data is being received (read). The queue handle will have been returned from the call to xQueueCreate() used to create the queue.
    // pvBuffer  A pointer to the memory into which the received data will be copied.
    //           The size of each data item that the queue holds is set when the queue is created. The memory pointed to by pvBuffer must be at least large enough to hold that many bytes.
    // xTicksToWait  The maximum amount of time the task should remain in the Blocked state to wait for data to become available on the queue, should the queue already be empty.
    //               If xTicksToWait is zer1.10o, then xQueueReceive() will return immediately if the queue is already empty.
    // RETURNS :  pdPASS    pdPASS will be returned only if data was successfully read from the queue.
    //                      If a block time was specified (xTicksToWait was not zero), then it is possible the calling task was placed into the Blocked state, to wait for data to become available on the queue, but data was successfully read from the queue before the block time expired.
    //            errQUEUE_EMPTY    errQUEUE_EMPTY will be returned if data cannot be read from the queue because the queue is already empty.
	QueueHandle_t xQueue = queue_id;
	struct PoolDetails *pPoolDetails = getPoolFromQId( queue_id );
	_pool_id poolId = pPoolDetails->msgPoolId;
    TickType_t xTicksToWait = pdMS_TO_TICKS(ms_timeout);
#if POINTERS_PASSED_IN_MSG_QS
	void * pvBuffer;
    BaseType_t result = xQueueReceive( xQueue, &pvBuffer, xTicksToWait );
#else
	void * pvBuffer = _msg_alloc( poolId );
    BaseType_t result = xQueueReceive( xQueue, pvBuffer, xTicksToWait );
#endif

    if ( result != pdPASS )
    {	// Queue is empty, free buffer and return NULL result
	#if !POINTERS_PASSED_IN_MSG_QS
    	_msg_free( queue_id, pvBuffer );
	#endif
    	return NULL;
    }
#if DEBUG_RTOS_TRANSLATION
	// Use the handle to obtain further information about the task.
	vTaskGetInfo( NULL, &xTaskDetails, pdTRUE, eInvalid );
	printf("Task '%s' (HWM %d): MSGQ %p RECEIVE, '%s', item %p\n", xTaskDetails.pcTaskName, xTaskDetails.usStackHighWaterMark, xQueue, (result == pdPASS)?"PASS":"FAIL", pvBuffer );
#endif
    return pvBuffer;
}

// queue_id [IN] — One of the following:
// 			queue ID of the queue to be checked
//			MSGQ_ANY_QUEUE (get the number of messages waiting in all message queues that the task has open)
// Returns
//		Number of messages (success)
//		0 (success: queue is empty)
//		0 (failure)
_mqx_uint _msgq_get_count(_queue_id queue_id)
{
	// Returns the number of items that are currently held in a queue.
	// Parameters : 	xQueue	The handle of the queue being queried.
	return uxQueueMessagesWaiting( queue_id );
}



// -----------------------------------------------------------------------------------------------
/// MUTEX FUNCTIONS :
///==================

// attr_ptr [IN]    Pointer to the mutex attributes structure to initialize
// Returns
//      MQX_EOK (success)
//      MQX_EINVAL (failure: attr_ptr is NULL)
_mqx_uint _mutatr_init(MUTEX_ATTR_STRUCT_PTR attr_ptr)
{
    return MQX_OK;  // We just support standard Mutex at present.
}

// Returns
//      MQX_OK (success)
//      MQX_OUT_OF_MEMORY (failure)
_mqx_uint _mutex_init(MUTEX_STRUCT_PTR& pMutex, MUTEX_ATTR_STRUCT_PTR attr_ptr)
{
    pMutex = xSemaphoreCreateMutex();
    if (pMutex == NULL)
    {
        configASSERT(!"Mutex Constructor Failed");
        return MQX_OUT_OF_MEMORY;
    }
    return MQX_OK;
}

// mutex_ptr [IN] â€” Pointer to the mutex to be locked
// Returns
//          MQX_EOK
//          Errors
_mqx_uint _mutex_lock(MUTEX_STRUCT_PTR pMutex)
{
    TickType_t xTicksToWait = portMAX_DELAY;    // Waits indefinitely until Sem available
    BaseType_t success = xSemaphoreTake(pMutex, xTicksToWait);
    return success == pdTRUE ? MQX_OK : MQX_EINVAL;
}

// mutex_ptr [IN] â€” Pointer to the mutex to be locked
// Returns
//          MQX_EOK
//          Errors
_mqx_uint _mutex_unlock(MUTEX_STRUCT_PTR pMutex)
{
    BaseType_t success = xSemaphoreGive(pMutex);
    return success == pdTRUE ? MQX_OK : MQX_EINVAL;
}


// -----------------------------------------------------------------------------------------------
/// SEMAPHORE FUNCTIONS :
///======================

// Parameters
//		lwsem_ptr [IN] — Pointer to the lightweight semaphore to create
//		initial_count [IN] — Initial semaphore counter
// Returns
//		MQX_OK
//		MQX_EINVAL (lwsem is already initialized)
//		MQX_INVALID_LWSEM (If, when in user mode, MQX RTOS tries to access a lwsem with inappropriate access rights)
_mqx_uint _lwsem_create(LWSEM_STRUCT_PTR sem_ptr, _mqx_int initial_number)
{
	sem_ptr = xSemaphoreCreateBinary();
    if (sem_ptr == NULL)
    {
        configASSERT(!"xSemaphoreCreateBinary() Failed");
        return MQX_OUT_OF_MEMORY;
    }
    return MQX_OK;
}

// Parameters
//		sem_ptr [IN] — Pointer to the lightweight semaphore
// Returns
//		MQX_OK
//		Errors
_mqx_uint _lwsem_wait(LWSEM_STRUCT_PTR sem_ptr)
{
    TickType_t xTicksToWait = portMAX_DELAY;    // Waits indefinitely until Sem available
    BaseType_t success = xSemaphoreTake(sem_ptr, xTicksToWait);
    return success == pdTRUE ? MQX_OK : MQX_EINVAL;
}

// Parameters
//		lwsem_ptr [IN] — Pointer to the created lightweight semaphore
// Returns
//		MQX_OK (success)
//		MQX_INVALID_LWSEM (failure: lwsem_ptr does not point to a valid lightweight semaphore)
_mqx_uint _lwsem_post(LWSEM_STRUCT_PTR sem_ptr)
{
    BaseType_t success = xSemaphoreGive(sem_ptr);
    return success == pdTRUE ? MQX_OK : MQX_EINVAL;
}


// -----------------------------------------------------------------------------------------------
/// TIMER FUNCTIONS :
///==================

// Start a periodic timer every number of ticks
// Parameters
//		notification_ function [IN] — Notification function that MQX RTOS calls when the timer expires
//		notification_ data_ptr [IN] — Data that MQX RTOS passes to the notification function
//		mode [IN] — Time to use when calculating the time to expire; one of the following:
//				TIMER_ELAPSED_TIME_MODE (use _time_get_elapsed() or _time_get_elapsed_ticks(), which are not affected by _time_set() or _time_set_ticks())
//				TIMER_KERNEL_TIME_MODE (use _time_get() or _time_get_ticks())
//		ms_wait [IN] — Milliseconds that MQX RTOS waits before it first calls the notification function and between subsequent
//					   calls to the notification function
//		tick_time_wait_ptr [IN] — Ticks (in tick time) that MQX RTOS waits before it first calls the notification function and between
//								  subsequent calls to the notification function
// Returns
//		Timer ID (success)
//		TIMER_NULL_ID (failure)
_timer_id  _timer_start_periodic_every_ticks(TIMER_NOTIFICATION_TICK_FPTR notification_function, void *notification_data_ptr,
												_mqx_uint mode, MQX_TICK_STRUCT_PTR tick_time_wait_ptr)
{
	// Notes : 1 use in code (ConfigManager.cpp) : With notification_data_ptr = NULL and mode = TIMER_ELAPSED_TIME_MODE
	TimerCallbackFunction_t pxCallbackFunction = notification_function;	// TODO : Callback prototypes are different between MQX and FreeRTOS
	TimerHandle_t handle = xTimerCreate( "TMR",			// Just a text name not used by kernel
								 tick_time_wait_ptr->ticks,
								 pdTRUE,		// Timer will Auto-Reload after expiry
								 ( void * ) 0,	// The ID is not used, so can be set to anything.
								 pxCallbackFunction );
	configASSERT( handle != NULL );
	BaseType_t result = xTimerStart( handle, portMAX_DELAY );
	configASSERT( result != pdFALSE );
}

// Cancels an outstanding timer request
// Parameters
//		id [IN] — ID of the timer to be cancelled, from calling a function from the _timer_start family of functions
// Returns
//		MQX_OK
//		Errors
_mqx_uint  _timer_cancel(_timer_id id)
{
	BaseType_t result = xTimerStop(id, portMAX_DELAY);
	configASSERT( result != pdFALSE );
}

// Start a timer that expires after the number of ticks
// Parameters
//		notification_ function [IN] — Notification function that MQX RTOS calls when the timer expires
//		notification_ data_ptr [IN] — Data that MQX RTOS passes to the notification function
//		mode [IN] — Time to use when calculating the time to expire; one of the following:
//				TIMER_ELAPSED_TIME_MODE (use _time_get_elapsed() or _time_get_elapsed_ticks(), which are not affected by _time_set() or _time_set_ticks())
//				TIMER_KERNEL_TIME_MODE (use _time_get() or _time_get_ticks())
//		milliseconds [IN] — Milliseconds to wait before MQX RTOS calls the notification function and cancels the timer
//		tick_time_ptr [IN] — Ticks (in tick time) to wait before MQX RTOS calls the notification function and cancels the timer
// Returns
//		Timer ID (success)
//		TIMER_NULL_ID (failure)
_timer_id  _timer_start_oneshot_after_ticks(TIMER_NOTIFICATION_TICK_FPTR notification_function, void *notification_data_ptr,
												_mqx_uint mode, MQX_TICK_STRUCT_PTR tick_time_ptr)
{
	// Notes : 2 uses in code (ConfigManager.cpp) : With notification_data_ptr = NULL and mode = TIMER_ELAPSED_TIME_MODE
	TimerCallbackFunction_t pxCallbackFunction = notification_function;	// TODO : Callback prototypes are different between MQX and FreeRTOS
	TimerHandle_t handle = xTimerCreate( "TMR",			// Just a text name not used by kernel
								 tick_time_ptr->ticks,
								 pdFALSE,		// Timer will be ONE-SHOT
								 ( void * ) 0,	// The ID is not used, so can be set to anything.
								 pxCallbackFunction );
	configASSERT( handle != NULL );
	BaseType_t result = xTimerStart( handle, portMAX_DELAY );
	configASSERT( result != pdFALSE );
}


// Initializes a tick time structure with the number of ticks
// Parameters
//		tick_time_ptr [OUT] — Pointer to the tick time structure to initialize
//		ticks [IN] — Number of ticks with which to initialize the structure
// Returns
//		TRUE (success)
//		FALSE (failure: input year is earlier than 1970 or output year is later than 2481)
_mqx_uint _time_init_ticks(MQX_TICK_STRUCT_PTR tick_time_ptr, _mqx_uint ticks)
{
    tick_time_ptr->ticks = ticks;
    return MQX_OK;
}

// Add time in msec units to tick time
// Parameters
//		tick_ptr [IN] — Tick time to add to
// Returns
//		Tick time
MQX_TICK_STRUCT_PTR  _time_add_msec_to_ticks(MQX_TICK_STRUCT_PTR tick_ptr, _mqx_uint msecs)
{
    tick_ptr->ticks += pdMS_TO_TICKS(msecs);
    return tick_ptr;
}

// Add time in sec units to tick time
// Parameters
//		tick_ptr [IN] — Tick time to add to
// Returns
//		Tick time
MQX_TICK_STRUCT_PTR  _time_add_sec_to_ticks(MQX_TICK_STRUCT_PTR tick_ptr, _mqx_uint secs)
{
    tick_ptr->ticks += pdMS_TO_TICKS(secs * 1000);
    return tick_ptr;
}

// Get the time in this format since RTOS started
// Parameters :
//		timetick_time_ptr [OUT] — Where to store the elapsed tick time
void _time_get_elapsed_ticks(MQX_TICK_STRUCT_PTR tick_time_ptr)
{
	tick_time_ptr->ticks = xTaskGetTickCount();
}

// Get the difference in this unit between two tick times in msecs
// Parameters
//		end_tick_ptr [IN] — Pointer to the ending tick time, which must be greater than the starting tick time
//		start_tick_ptr [IN] — Pointer to the starting tick time
//		overflow_ptr [OUT] — TRUE if overflow occurs.  In this case needs to use hours, days version of function etc
// Returns
//		Difference in msecs
int32_t _time_diff_milliseconds(MQX_TICK_STRUCT_PTR end_tick_ptr, MQX_TICK_STRUCT_PTR start_tick_ptr, bool *overflow_ptr)
{
	TickType_t diff = end_tick_ptr->ticks - start_tick_ptr->ticks;
	return diff * 1000 / configTICK_RATE_HZ;
}

// Suspend the active task for specified number of msec
// Parameters :
//		milliseconds	msec delay to suspend task for
void _time_delay(uint32_t milliseconds)
{
	vTaskDelay( pdMS_TO_TICKS(milliseconds) );
}

// The function prints the oldest entry in kernel log to the default output stream of the current task and deletes the entry.
// Returns
//		TRUE (entry is found and displayed)
//		FALSE (entry is not found)
bool _klog_display(void)
{
}


// -----------------------------------------------------------------------------------------------
/// EVENT FUNCTIONS :
///==================

// Initializes the lightweight event group.
// Parameters
//		lwevent_group_ptr [IN] — Pointer to the lightweight event group to initialize
//		flags[IN] — Creation flag; one of the following:
//			LWEVENT_AUTO_CLEAR - all bits in the lightweight event group are made autoclearing
//			0 - lightweight event bits are not set as autoclearing by default  [NOTE : CDD Code always uses this option]
//			note: the autoclearing bits can be changed any time later by calling _lwevent_set_auto_clear.
//Returns
//		MQX_OK
//		MQX_EINVAL (lwevent is already initialized)
//		MQX_LWEVENT_INVALID (If, when in user mode, MQX RTOS tries to access a lwevent with inappropriate access rights.)
_mqx_uint _lwevent_create(LWEVENT_STRUCT_PTR lwevent_group_ptr, _mqx_uint flags)
{
	lwevent_group_ptr->pEventGroup = new cpp_freertos::EventGroup();
	lwevent_group_ptr->flags = flags;
	return MQX_OK;
}


// Wait for the specified lightweight event bits to be set in the lightweight event group
// Blocks until the event combination is set or until the timeout expires
// Parameters
//		event_ptr [IN] — Pointer to the lightweight event
//		bit_mask [IN] — Each set bit represents an event bit to wait for
//		all — One of the following:
//			TRUE (wait for all bits in bit_mask to be set)
//			FALSE (wait for any bit in bit_mask to be set)
//		tick_ptr [IN] — One of the following:
//			pointer to the maximum number of ticks to wait
//			NULL (unlimited wait)
//		timeout_in_ticks [IN] — One of the following:
//			maximum number of ticks to wait
//			Wait for the specified lightweight event bits to be set in the lightweight event group:
//				For the number of ticks
//				0 (unlimited wait)
// Returns
//		MQX_OK
//		LWEVENT_WAIT_TIMEOUT (the time elapsed before an event signalled)
//		Errors
_mqx_uint _lwevent_wait_ticks(LWEVENT_STRUCT_PTR event_ptr, _mqx_uint bit_mask, bool all, _mqx_uint timeout_in_ticks)
{
	EventBits_t uxBitsToWaitFor = bit_mask;
	bool xClearOnExit = (event_ptr->flags == 0) ? false : true;
	TickType_t xTicksToWait = (timeout_in_ticks == 0) ? portMAX_DELAY : timeout_in_ticks;
	EventBits_t bits = event_ptr->pEventGroup->WaitBits(uxBitsToWaitFor, xClearOnExit, all, xTicksToWait);

	// Write the 32-bit bit-mask value directly into index 0 of the thread local storage array.
	vTaskSetThreadLocalStoragePointer( NULL,  /* Task handle. */
	                                   0,     /* Index into the array. */
	                                   ( void * ) bits );
	if ( (bits & uxBitsToWaitFor) == 0 )
		return LWEVENT_WAIT_TIMEOUT;
	else
		return MQX_OK;
}

// Gets which particular bit(s) in the lwevent unblocked recent wait command
// Returns
//		lwevent mask from last task's lwevent_wait_xxx call that unblocked the task
_mqx_uint _lwevent_get_signalled(void)
{
	// Read the bit-mask value stored in index 0 of the calling task's thread local storage array.
	return (_mqx_uint) pvTaskGetThreadLocalStoragePointer( NULL, 0 );
}

// Sets the specified event bits in the lightweight event group.
// Parameters
//		lwevent_group_ptr [IN] — Pointer to the lightweight event group to set bits in
//		flags [IN] — Each bit represents an event bit to be set
// Returns
//		MQX_OK (success)
//		MQX_LWEVENT_INVALID (failure: lightweight event group was invalid)
_mqx_uint _lwevent_set(LWEVENT_STRUCT_PTR lwevent_group_ptr, _mqx_uint flags)
{
	EventBits_t uxBitsToSet = flags;
	EventBits_t bits = lwevent_group_ptr->pEventGroup->SetBits( uxBitsToSet );
	return MQX_OK;
}

// Clears the specified event bits in the lightweight event group
// Parameters
//		event_group_ptr [IN] — Pointer to the event group
//		bit_mask [IN] — Each set bit represents an event bit to clear
// Returns
//		MQX_OK (success)
//		LWEVENT_INVALID_EVENT (failure: lightweight event group is not valid)
_mqx_uint _lwevent_clear(LWEVENT_STRUCT_PTR event_group_ptr, _mqx_uint bit_mask)
{
	EventBits_t uxBitsToClear = bit_mask;
	EventBits_t bits = event_group_ptr->pEventGroup->ClearBits( uxBitsToClear );
	return MQX_OK;
}



#define MAX_EVENT_GROUPS	1	// Only 1 Event Group required to be supported in the CDD code ("event_spkr_cnfg")
static uint32_t s_currentNumOfEventGroups = 0;
static struct eventGroupDetail
{	// Remember details of Event Groups
	char            			name[NAME_MAX_NAME_SIZE];
	bool 						autoClear; 		//	true - all bits in the lightweight event group are made autoclearing
	cpp_freertos::EventGroup	*pEventGroup;	// Mapping to FreeRTOS equivalent
} s_eventGroupDetails[MAX_EVENT_GROUPS] = {0};

static struct eventGroupDetail* getEventGroupByName( char *pName )
{
	for ( uint32_t i = 0; i < MAX_EVENT_GROUPS; i++ )
	{
		if ( strcmp(pName, s_eventGroupDetails[i].name) == 0 )
			return &s_eventGroupDetails[i];
	}
	return NULL;
}

// Creates the named event group
// Parameters
//		name_ptr [IN] — Name of the event group
// Returns
//		MQX_OK
//		Errors
_mqx_uint _event_create(char *name_ptr)
{
	configASSERT( s_currentNumOfEventGroups < MAX_EVENT_GROUPS );
	struct eventGroupDetail *pEvent = &s_eventGroupDetails[s_currentNumOfEventGroups++];
	strncpy(pEvent->name, name_ptr, (_mqx_uint) NAME_MAX_NAME_SIZE - 1);
	pEvent->pEventGroup = new cpp_freertos::EventGroup();
	pEvent->autoClear = false;
	return MQX_OK;
}

// Opens a connection to the named event group.
// Parameters
//		name_ptr [IN] — Pointer to the name of the event group (see description)
//		event_ptr [OUT] — Where to write the event group handle (NULL is written if an error occurred)
// Returns
//		MQX_OK
//		Errors
_mqx_uint _event_open(char *name_ptr, void **event_ptr)
{
	struct eventGroupDetail *pEvent = getEventGroupByName(name_ptr);
	*event_ptr = (void *) pEvent;
	return (pEvent != NULL) ? MQX_OK : NAME_NOT_FOUND;
}

// Wait for any of the specified event bits to be set in the event group (for the number of ms)
// Parameters
//		event_group_ptr [IN] — Event group handle returned by _event_open() or _event_open_fast()
//		bit_mask [IN] — Each set bit represents an event bit to wait for
//		ms_timeout [IN] — One of the following:
//			maximum number of milliseconds to wait
//			0 (unlimited wait)
// Returns
//		MQX_OK
//		Errors
_mqx_uint _event_wait_any(void *event_group_ptr, _mqx_uint bit_mask, uint32_t ms_timeout)
{
	struct eventGroupDetail *pEvent = (struct eventGroupDetail *) event_group_ptr;
	EventBits_t uxBitsToWaitFor = bit_mask;
	bool xClearOnExit = pEvent->autoClear;
	TickType_t xTicksToWait = (ms_timeout == 0) ? portMAX_DELAY : pdMS_TO_TICKS(ms_timeout);
	EventBits_t bits = pEvent->pEventGroup->WaitBits(uxBitsToWaitFor, xClearOnExit, false, xTicksToWait);
	return MQX_OK;
}

// Gets the event bits for the event group
// Parameters
//		event_group_ptr [IN] — Event group handle returned by _event_open()
//		event_group_value_ptr [OUT] — Where to write the value of the event bits (on error, 0 is written)
// Returns
//		MQX_OK
//		Errors
_mqx_uint _event_get_value(void *event_group_ptr, _mqx_uint_ptr event_group_value_ptr)
{
	struct eventGroupDetail *pEvent = (struct eventGroupDetail *) event_group_ptr;
	EventBits_t bits = pEvent->pEventGroup->GetBits();
	*event_group_value_ptr = bits;
	return MQX_OK;
}








// -----------------------------------------------------------------------------------------------
/// TASK FUNCTIONS :
///=================
_mqx_uint _task_get_error(void)
{
	return MQX_OK;	// Dummy implementation, Don't think we need any FreeRTOS equivalent
}

static _mqx_uint s_errno = MQX_OK;
_mqx_uint* _task_get_error_ptr(void)
{	// Dummy implementation, Don't think we need any FreeRTOS equivalent
	return &s_errno;
}

_mqx_uint _task_set_error(_mqx_uint)
{
	// Don't think we need any FreeRTOS equivalent
}

// Blocks the active task.
void _task_block(void)
{
	vTaskSuspend( NULL );
}

// -----------------------------------------------------------------------------------------------


