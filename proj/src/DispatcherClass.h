#ifndef _DISPATCHER_CLASS_H_
#define _DISPATCHER_CLASS_H_

// DEFINITIONS
//

#if (1)
#define BIT(n) 			(0x01<<(n))

#define IRQ_TM			0
#define IRQ_KB			1
#define IRQ_COM2		3
#define IRQ_COM1		4
#define IRQ_RTC			8
#define IRQ_MS			12

#define SP_ADDR_COM2	0x2F8
#define SP_ADDR_COM1	0x3F8
#endif

// LOCAL INCLUDES
//
#include "InputClass.h"
#include "ClockClass.h"
#include "GraphicsClass.h"
#include "GameLogicClass.h"
#include "SerialPortClass.h"

typedef struct dispatcher_class DispatcherClass;
typedef enum
{
	KB = 1,
	MS = 2,
	RTC = 3,
	SP = 4,
	TM = 5
} devices_t;


/* Default constructor
 *
 */
DispatcherClass* newDispatcher();

/* Destructor
 *
 * @param Dispatcher A pointer to the "class"
 */
void deleteDispatcher(DispatcherClass* Dispatcher);


/* Initializer
 *
 * @param Dispatcher A pointer to the "class"
 * @param Clock A pointer to the ClockClass
 * @param Input A pointer to the InputClass
 * @param Graphics A pointer to the GraphicsClass
 *
 * @return False if a error occurs. True otherwise
 */
bool DispatcherInitialize(DispatcherClass* Dispatcher, ClockClass* Clock, InputClass* Input, GameLogicClass* GameLogic, GraphicsClass* Graphics);

/* Shutdown
 *
 * @param Dispatcher A pointer to the "class"
 */
void DispatcherShutdown(DispatcherClass* Dispatcher);

bool DispatcherInitializeSerialPort(DispatcherClass* Dispatcher, SerialPortClass* SerialPort);
void DispatcherShutdownSerialPort(DispatcherClass* Dispatcher);


/* Dispatch a message
 *
 * @param Dispatcher A pointer to the "class"
 */
void DispatcherRun(DispatcherClass* Dispatcher);


/* Initialize Clock
 *
 * @param Dispatcher A pointer to the "class"
 *
 * @return False if a error occurs. True otherwise
 */
static bool DispatcherInitializeClock(DispatcherClass* Dispatcher, ClockClass* Clock);

/* Shutdown Clock
 *
 * @param Dispatcher A pointer to the "class"
 */
static void DispatcherShutdownClock(DispatcherClass* Dispatcher);


/*
 *
 * @param Dispatcher A pointer to the "class"
 *
 * @return False if a error occurs. True otherwise
 */
static bool DispatcherInitializeInput(DispatcherClass* Dispatcher, InputClass* Input);

/*
 *
 * @param Dispatcher A pointer to the "class"
 */
static void DispatcherShutdownInput(DispatcherClass* Dispatcher);


/*
 * @param Dispatcher A pointer to the "class"
 *
 * @return False if a error occurs. True otherwise
 */
static bool DispatcherSubscribeInt(DispatcherClass* Dispatcher, devices_t device, unsigned long flags);

/*
 *
 * @return False if a error occurs. True otherwise
 */
static bool DispatcherUnsubscribeInt(DispatcherClass* Dispatcher, devices_t device);

/*
 *
 * @param Dispatcher A pointer to the "class"
 *
 * @return False if a error occurs. True otherwise
 */
static bool DispatcherHandleInt(DispatcherClass* Dispatcher, devices_t device);

/*
 *
 * @param Dispatcher A pointer to the "class"
 */
static int DispatcherGetIRQLine(DispatcherClass* Dispatcher, devices_t device);

/*
 *
 * @param Dispatcher A pointer to the "class"
 */
static int DispatcherGetHookID(DispatcherClass* Dispatcher, devices_t device);

/*
 *
 * @param Dispatcher A pointer to the "class"
 */
static int DispatcherGetIRQSet(DispatcherClass* Dispatcher, devices_t device);


/*
 *
 * @param Dispatcher A pointer to the "class"
 */
static void DispatcherSetHookID(DispatcherClass* Dispatcher, devices_t device, int hookID);

/*
 * @param Dispatcher A pointer to the "class"
 *
 * @return True if interrupt subscription is enabled
 */
static void DispatcherSetIRQSet(DispatcherClass* Dispatcher, devices_t device, bool enable);

#endif
