#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_
#include "stdlib.h"
#include "stm32f1xx.h"

#define TIMER_CYCLE 1

typedef struct sTask {
// Pointer to the task (must be a ’ void ( void ) ’ function )
	void ( * pTask) ( void ) ;
	// Delay ( ticks ) until the function will ( next ) be run
	int Delay ;
	// Interval ( tickse ) between subsequent runs .
	uint32_t Period ;
	// Incremented ( by scheduler ) when task is due to execute
	uint8_t RunMe;
	// This is a hint to solve the question below .
	uint32_t TaskID ;
	struct sTask * next;
} sTask;

extern sTask * SCH_Tasks;

extern unsigned int taskID;

void SCH_Init ( void );
unsigned int SCH_Add_Task ( void (* Task) ( void ), uint32_t Delay, uint32_t Period );
void SCH_Add_Task_Dispatcher ( void (* Task) ( void ), uint32_t Delay, uint32_t Period, int TaskID );

void Delete_Task_Dispatcher ( void );
void Delete_Task ( unsigned int TaskID );

void SCH_Update ( void );
void SCH_Dispatcher ( void );
//void SCH_Display();

#endif /* INC_SCHEDULER_H_ */
