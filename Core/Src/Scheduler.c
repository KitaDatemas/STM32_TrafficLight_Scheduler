#include "Scheduler.h"

sTask * SCH_Tasks;
unsigned int taskID = 0;

void SCH_Init ( void ) {
	SCH_Tasks = NULL;
}

unsigned int SCH_Add_Task (void (* Task) (void), uint32_t Delay, uint32_t Period) {
    sTask * NewTask = (sTask *) malloc (sizeof( sTask ));
    NewTask->pTask = Task;
	NewTask->Delay = Delay;
    NewTask->Period = Period;
    NewTask->RunMe = 0;
    NewTask->TaskID = taskID++;

    //Find the suitable position for Task
    sTask **pointer = &SCH_Tasks;
    while (*pointer != NULL && NewTask->Delay >= (*pointer)->Delay) {
        NewTask->Delay -= (*pointer)->Delay;
        pointer = &((*pointer)->next);
    }
    NewTask->next = *pointer;
    *pointer = NewTask;

    //Update the rest of the linked list
    if (NewTask->next != NULL) {
    	(NewTask->next)->Delay -= NewTask->Delay;
    }
    return NewTask->TaskID;
}

void SCH_Add_Task_Dispatcher (void (* Task) (void), uint32_t Delay, uint32_t Period, int TaskID) {
    sTask * NewTask = (sTask *) malloc (sizeof(sTask));
    NewTask->pTask = Task;
    NewTask->Delay = Delay;
    NewTask->Period = Period;
    NewTask->RunMe = 0;
    NewTask->TaskID = TaskID;

    //Find the suitable position for Task
    sTask **pointer = &SCH_Tasks;
    while (*pointer != NULL && NewTask->Delay >= (*pointer)->Delay) {
        NewTask->Delay -= (*pointer)->Delay;
        pointer = &((*pointer)->next);
    }
    NewTask->next = *pointer;
    *pointer = NewTask;

    //Update the rest of the linked list
    if (NewTask->next != NULL) {
    	(NewTask->next)->Delay -= NewTask->Delay;
    }
}

void Delete_Task_Dispatcher ( void ) {
    sTask * deleteTask = SCH_Tasks;

    SCH_Tasks = SCH_Tasks->next;
    free(deleteTask);
}

void Delete_Task ( unsigned int TaskID) {
    sTask ** taskPointer = &SCH_Tasks;

    while (*taskPointer != NULL && (*taskPointer)->TaskID != TaskID)
        taskPointer = &((*taskPointer)->next);

    if (*taskPointer != NULL) {
        sTask *deleteTask = *taskPointer;
        *taskPointer = (*taskPointer)->next;
        free(deleteTask);
    }
}

void SCH_Update ( void ) {
	if (SCH_Tasks != NULL) {
		SCH_Tasks->Delay -= TIMER_CYCLE;
	}
}

void SCH_Dispatcher ( void ) {
	if (SCH_Tasks != NULL) {
		if (SCH_Tasks->Delay <= 0) {
			(*(SCH_Tasks->pTask))();
			if (SCH_Tasks->Period > 0) {
				void *func = SCH_Tasks->pTask;
				unsigned int Period = SCH_Tasks->Period;
				unsigned int TaskID = SCH_Tasks->TaskID;
				SCH_Add_Task_Dispatcher (func, Period, Period, TaskID);
			}
			Delete_Task_Dispatcher ();
		}
	}
}

//void SCH_Display() {
//    sTask * head = SCH_Tasks;
//    while (head){
//        printf("(Delay: %d, Period: %d, RunMe: %d) ", head->Delay, head->Period, head->RunMe);
//        head = head->next;
//    }
//    printf ("\n");
//}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	SCH_Update();
}
