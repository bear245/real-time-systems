/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#define TASK_HIGH_PRIO 4
#define TASK_STACK_SIZE 512
#define N_TASKS_AMOUNT 4
#define INPUT_POS_Y 5
#define TASKS_POS_Y 6
#define ASCII_CODE_ZERO 49
#include "../common.h"

byte freePrio = TASK_HIGH_PRIO + 1;
OS_STK tasksStack[N_TASKS_AMOUNT][TASK_STACK_SIZE];
OS_STK initialTaskStack[TASK_STACK_SIZE];

int getFreePrio(){
    return freePrio++;
}

struct TaskData{
    int cCounter;
    int tCounter;
    int id;
    OS_EVENT *semaphore;
};

void initStruct(struct TaskData* data, int id){
    data->id = id + 1;
    data->cCounter = 0;
    data->tCounter = 0;
    data->semaphore = OSSemCreate(0);
}

void simpleTask(void* data){    
    struct TaskData* tdata = (struct TaskData*)data;
    char buffer[100];
    UBYTE err;
    
    sprintf(buffer, "id: %d |cc: %d | tc: %d", tdata->id, tdata->cCounter, tdata->tCounter);
    print(0, TASKS_POS_Y + tdata->id, buffer);

    while(TRUE){
        OSSemPend(tdata->semaphore, OS_TICKS_PER_SEC * 4, &err);
        //TODO: ADD CODE ERRORS
        if(err != 0){
            tdata->tCounter++;
        }else{
            tdata->cCounter++;
        }
        print(0, TASKS_POS_Y + tdata->id, EMPTY_STRING);
        sprintf(buffer, "id: %d |cc: %d | tc: %d", tdata->id, tdata->cCounter, tdata->tCounter);
        print(0, TASKS_POS_Y + tdata->id, buffer);
    }
}

void initialTask(void* data){    
    struct TaskData tasks[N_TASKS_AMOUNT];
    char keyBuffer[100];
    char msg[] = "Calling task:";
    int tab = strlen(msg), i = 0, keyNumber = 0, taskNumber;
    INT16S key;
    UBYTE err;

    for (i = 0; i < N_TASKS_AMOUNT; i++) {
        initStruct(&tasks[i], i);
        createTask(simpleTask, (void *)&tasks[i], (void *)&tasksStack[i][TASK_STACK_SIZE - 1],getFreePrio());
    }

    print(0, INPUT_POS_Y, msg);
    while(1){
        if(!PC_GetKey(&key)){
            waitMili(200);
            continue;
        }

        if(key == KEY_ESC){
            exit(0);
        }

        print(tab, INPUT_POS_Y, EMPTY_STRING);
        sprintf(keyBuffer, "%c", key);
        taskNumber = key - ASCII_CODE_ZERO;
        
        if(taskNumber < N_TASKS_AMOUNT){
            print(tab, INPUT_POS_Y, keyBuffer);
            OSSemPost(tasks[taskNumber].semaphore);
        }
        else{
            sprintf(keyBuffer, "%c is undefined.", key);
            print(tab, INPUT_POS_Y, keyBuffer);
        }
    }
}

int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    print(26,  0, "uC/OS-II, The Real-Time Kernel");
    print(33,  1, "Inter-process communication mechanisms");
    print(34,  2, "Yevhenii Maliavka");
    print(33,  3, "(press ESC to exit)");
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], freePrio++);
    OSStart();
    return 0;
}