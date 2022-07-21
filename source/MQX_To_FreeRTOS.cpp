#include "MQX_to_FreeRTOS.h"
#include "mem_pool.hpp"




/// MESSAGE POOLS :
///================

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
static _pool_id s_msgPool = MSGPOOL_NULL_POOL_ID;	// TODO : Supporting only 1 Message Pool just now
static uint16_t	s_messageSize = 0;
_pool_id _msgpool_create(uint16_t message_size, uint16_t num_messages, uint16_t grow_number, uint16_t grow_limit)
{
    // Use MemoryPool from mem_pool.hpp ?
	s_messageSize = message_size;
    int itemSize = message_size;
    int itemCount = num_messages;
    int alignment = 4;
    // TODO : We're only supporting 1 Message Pool just now
    s_msgPool = new cpp_freertos::MemoryPool( itemSize, itemCount, alignment );
    return s_msgPool;
}

// pool_id [IN] A pool ID from _msgpool_create()
// Returns  Pointer to a message (success)
//          NULL (failure)
void *_msg_alloc(_pool_id pool_id)
{
	cpp_freertos::MemoryPool* pMemPool = static_cast<cpp_freertos::MemoryPool*>( pool_id );
    return pMemPool->Allocate();
}

// msg_ptr [IN]	Pointer to the message to be freed
void _msg_free(void* msg_ptr)
{
	// TODO : We're only supporting 1 Message Pool just now
	cpp_freertos::MemoryPool* pMemPool = static_cast<cpp_freertos::MemoryPool*>( s_msgPool );
	pMemPool->Free( msg_ptr );
}

#if 0
// msg_ptr [IN] — Pointer to the message to be freed
void _msg_free( void* msg_ptr )
{
	vPortFree( pvBuffer );
}
#endif


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
static _queue_id s_theOneQueueId = 0;	// TODO : Only 1 Queue supported at present
_queue_id _msgq_open( _queue_number queue_number, uint16_t max_queue_size )
{
    // FreeRTOS equivalent :
    // uxQueueLength    The maximum number of items that the queue being created can hold at any one time.
    // uxItemSize       The size in bytes of each data item that can be stored in the queue.
    // Return Value     If NULL is returned, then the queue cannot be created because there is insufficient heap memory available for FreeRTOS
    //                  to allocate the queue data structures and storage area.
    //                  A non-NULL value being returned indicates that the queue has been created successfully. The returned value should be stored as the handle to the created queue.
	UBaseType_t uxQueueLength = max_queue_size;
	UBaseType_t uxItemSize = s_messageSize;	// TODO : 1 Pool support with fixed message size at the moment
	QueueHandle_t handle = xQueueCreate( uxQueueLength, uxItemSize );	// handle is 32 bit pointer !!
	s_theOneQueueId = handle;	// TODO : ! Queue support only at present
	return s_theOneQueueId;
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
	void * pvBuffer = pvPortMalloc(s_messageSize);	// TODO : 1 Pool support with fixed size at present
	TickType_t xTicksToWait = 0;
    BaseType_t result = xQueueReceive( xQueue, pvBuffer, xTicksToWait );
    if ( result != pdPASS )
    {	// Queue is empty, free buffer and return NULL result
    	vPortFree( pvBuffer );
    	return NULL;
    }
    return pvBuffer;
}

// msg_ptr IN] Pointer to the message to be sent
// Returns  TRUE (success: see description)
//          FALSE (failure)
bool _msgq_send(void *msg_ptr)
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
	QueueHandle_t xQueue = s_theOneQueueId; // TODO : How to know which message Q to send on ?
	const void * pvItemToQueue = msg_ptr;
    TickType_t xTicksToWait = 0;
    BaseType_t result = xQueueSendToBack( xQueue, pvItemToQueue, xTicksToWait );
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
    //               If xTicksToWait is zero, then xQueueReceive() will return immediately if the queue is already empty.
    // RETURNS :  pdPASS    pdPASS will be returned only if data was successfully read from the queue.
    //                      If a block time was specified (xTicksToWait was not zero), then it is possible the calling task was placed into the Blocked state, to wait for data to become available on the queue, but data was successfully read from the queue before the block time expired.
    //            errQUEUE_EMPTY    errQUEUE_EMPTY will be returned if data cannot be read from the queue because the queue is already empty.
	QueueHandle_t xQueue = s_theOneQueueId; // TODO : How to know which message Q to send on ?
	void * pvBuffer = pvPortMalloc(s_messageSize);	// TODO : 1 Pool support with fixed size at present
    TickType_t xTicksToWait = pdMS_TO_TICKS(ms_timeout);
    BaseType_t result = xQueueReceive( xQueue, pvBuffer, xTicksToWait );
    if ( result != pdPASS )
    {	// Queue is empty, free buffer and return NULL result
    	vPortFree( pvBuffer );
    	return NULL;
    }
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
	// TODO
	return 0;
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
_mqx_uint _mutex_init(MUTEX_STRUCT_PTR pMutex, MUTEX_ATTR_STRUCT_PTR attr_ptr)
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

_timer_id  _timer_start_periodic_every_ticks(TIMER_NOTIFICATION_TICK_FPTR, void *, _mqx_uint, MQX_TICK_STRUCT_PTR)
{
}

_mqx_uint  _timer_cancel(_timer_id)
{
}

_timer_id  _timer_start_oneshot_after_ticks(TIMER_NOTIFICATION_TICK_FPTR, void *, _mqx_uint, MQX_TICK_STRUCT_PTR)
{
}

_mqx_uint _time_init_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint)
{
}

MQX_TICK_STRUCT_PTR  _time_add_msec_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint)
{
}

MQX_TICK_STRUCT_PTR  _time_add_sec_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint)
{
}

void _time_get_elapsed_ticks(MQX_TICK_STRUCT_PTR)
{
}

int32_t _time_diff_milliseconds(MQX_TICK_STRUCT_PTR, MQX_TICK_STRUCT_PTR, bool *)
{
}

void _time_delay(uint32_t)
{
}


bool _klog_display(void)
{
}


_mqx_uint _lwevent_wait_ticks(LWEVENT_STRUCT_PTR, _mqx_uint, bool, _mqx_uint)
{
}

_mqx_uint _lwevent_get_signalled(void)
{
}

_mqx_uint _lwevent_set(LWEVENT_STRUCT_PTR, _mqx_uint)
{
}

_mqx_uint _lwevent_clear(LWEVENT_STRUCT_PTR, _mqx_uint)
{
}

_mqx_uint _event_create(char *)
{
}

_mqx_uint _event_open(char *, void **)
{
}

_mqx_uint _event_wait_any(void *, _mqx_uint, uint32_t)
{
}

_mqx_uint _event_get_value(void *, _mqx_uint_ptr)
{
}





_mqx_uint _lwsem_wait(LWSEM_STRUCT_PTR)
{
}

_mqx_uint _lwsem_post(LWSEM_STRUCT_PTR)
{
}



_mqx_uint _task_get_error(void)
{

}

_mqx_uint* _task_get_error_ptr(void)
{

}

_mqx_uint _task_set_error(_mqx_uint)
{
}

void _task_block(void)
{
}

// -----------------------------------------------------------------------------------------------


