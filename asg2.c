/* Kyle Compton (mkcompt)
 * ECE 3220
 * Assignment 2
 * 04/04/2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Task struct that holds info about each task
struct task{
    int task_id,
        arrival_time,
        service_time,
        remaining_time,
        completion_time,
        response_time,
        wait_time;
    struct task *next;
};

// Function for simulating sjfp
void sjfp(struct task *tasks[], int num_tasks){
    struct task *cpu_head = NULL; // Points to the task being held by the cpu
    struct task *ready_head = NULL; // Points to the head of the ready queue list
    int current_time = 0;
    int num_completed = 0;
    int next_task = 0;

    while(num_completed < num_tasks){
        // Puts the next task in the ready queue depending on its remaining time
        while (next_task <num_tasks && tasks[next_task]->arrival_time == current_time){
            if(ready_head == NULL){
                ready_head = tasks[next_task];
                ready_head->next = NULL;
            }
            else if (tasks[next_task]->remaining_time < ready_head->remaining_time){
                tasks[next_task]->next = ready_head;
                ready_head = tasks[next_task];
            }
            else{
                struct task *rover = ready_head;
                while(rover->next != NULL && tasks[next_task]->remaining_time >= rover->next->remaining_time){
                    rover = rover->next;
                }
                tasks[next_task]->next = rover->next;
                rover->next = tasks[next_task];

            }
            next_task++;
        }
        // This is the preemption step where if new task is shorter, kick current one off cpu
        // Other task that was previously held by the cpu gets put back in the ready qeueu
        if (cpu_head != NULL && ready_head != NULL) {
            if (ready_head->remaining_time < cpu_head->remaining_time){
                if (ready_head->next == NULL || cpu_head->remaining_time < ready_head->next->remaining_time){
                    cpu_head->next = ready_head->next;
                    ready_head->next = cpu_head;
                }
                else{
                    struct task *rover = ready_head->next;
                    while (rover->next != NULL && cpu_head->remaining_time >= rover->next->remaining_time){
                        rover = rover->next;
                    }
                    cpu_head->next = rover->next;
                    rover->next = cpu_head;
            }
                cpu_head = ready_head;
                ready_head = ready_head->next;
                cpu_head->next = NULL;
            }
        }
        // give cpu a task if empty
        if (cpu_head == NULL && ready_head != NULL){
            cpu_head = ready_head;
            ready_head = ready_head->next;
            cpu_head->next = NULL;
        }
        //printing the summary table
        printf("%3d   ", current_time);
        if (cpu_head == NULL){
            printf("%-7s", "--");
        } else {
            printf("%c%-6d", cpu_head->task_id, cpu_head->remaining_time);
        }

        if (ready_head == NULL){
            printf("--");
        } else{
            struct task *rover = ready_head;
            while (rover != NULL) {
                printf("%c%d", rover->task_id, rover->remaining_time);
                if (rover->next != NULL)
                    printf(", ");
                rover = rover->next;
            }
        }
        printf("\n");
        // Decrements the remaining time of the task that just ran
        if (cpu_head != NULL){
            cpu_head->remaining_time--;
            // checks if the task finished and if so, increments the number of completed tasks by one
            if(cpu_head->remaining_time == 0){
                cpu_head->completion_time = current_time + 1; // Since completion time is the second after it completes we add 1
                cpu_head = NULL;
                num_completed++;
            }
        }
        current_time++;
    }
    // Calculates the response time and wait time for each task
    for (int i = 0; i < num_tasks; i++){
        tasks[i]->response_time = tasks[i]->completion_time -tasks[i]->arrival_time;
        tasks[i]->wait_time =tasks[i]->response_time - tasks[i]->service_time;
    }

    // Prints the tid, arrival time, service time, completion time, response time, and wait times for each task
    printf("\n%-4s %-8s %-9s %-11s %-9s %s\n", "","arrival", "service", "completion","response", "wait");
    printf("%-4s %-8s %-9s %-11s %-9s %s\n", "tid", "time", "time", "time","time", "time");
    printf("-------------------------------------------------\n");
    for (int i = 0; i < num_tasks; i++){
        printf("%-4c %-8d %-9d %-11d %-9d %d\n", tasks[i]->task_id,tasks[i]->arrival_time, tasks[i]->service_time, tasks[i]->completion_time,tasks[i]->response_time, tasks[i]->wait_time);
    }
    // Sorts the tasks array in order of service time ascending
    for (int i = 0; i < num_tasks - 1; i++) {
    for (int j = 0; j < num_tasks - i - 1; j++) {
        if (tasks[j]->service_time > tasks[j+1]->service_time){
            struct task *temp = tasks[j];
            tasks[j] = tasks[j+1];
            tasks[j+1] = temp;
            }
        }
    }
    // Prints the tasks in service time ascending as well as the tasks wait time
    printf("\nService   Wait");
    printf("\ntime      time");
    printf("\n----------------");
    for ( int i = 0; i < num_tasks ; i++){
        printf("\n%d          %d", tasks[i]->service_time, tasks[i]->wait_time);
    }   
    printf("\n");
}

// Function for simulating rr
void round_robin(struct task *tasks[], int num_tasks, int time_slice){
    struct task *cpu_head = NULL; // Points to the task that is being handelled by the cpu
    struct task *ready_head = NULL; // Points to the task that is at the front of the ready queue
    struct task *ready_tail = NULL; // Poitn sto the task that is at the back of the ready queue
    int current_time = 0;
    int num_completed = 0;
    int next_task = 0;
    int slice_count = 0; // Value that keeps track of how much time a task has been running for

    while(num_completed < num_tasks){
        // Loops through tasks and places them in the ready queue either at the front if the queue is empty, or at the back if not empty
        while (next_task < num_tasks && tasks[next_task]->arrival_time == current_time){
            if(ready_head == NULL){
                ready_head = tasks[next_task];
                ready_tail = tasks[next_task];
                tasks[next_task]->next = NULL;
            }
            else{
                ready_tail->next = tasks[next_task];
                ready_tail = tasks[next_task];
                tasks[next_task]->next = NULL;
            }
            next_task++;
        }
        // If the cpu is working on a task and the tasks time slice completes, we put that task back into the ready queue
        // Need to check if the ready queue is empty
        if (cpu_head != NULL && slice_count == time_slice){
            slice_count = 0;
            cpu_head->next = NULL;
            if (ready_head == NULL){
                ready_head = cpu_head;
                ready_tail = cpu_head;
            }
            else{
                ready_tail->next = cpu_head;
                ready_tail = cpu_head;
            }
            cpu_head = NULL;
        }
        // If the cpu is not working on a task and there is a task at the head of the ready queue we assign it to the cpu
        if (cpu_head == NULL && ready_head != NULL){
            cpu_head = ready_head;
            ready_head = ready_head->next;
            cpu_head->next = NULL;
        }
        // Printing the summary table. Same as sjfp
        printf("%3d   ",current_time);
        if (cpu_head == NULL){
            printf("%-7s", "--");
        } else {
            printf("%c%-6d", cpu_head->task_id, cpu_head->remaining_time);
        }

        if(ready_head == NULL){
            printf("--");
        } else{
            struct task *rover = ready_head;
            while (rover != NULL) {
                printf("%c%d", rover->task_id, rover->remaining_time);
                if(rover->next != NULL)
                    printf(", ");
                rover = rover->next;
            }
        }
        printf("\n");

        // If cpu is working on a task we decrememnt that tasks remaining time and incrememnt its slice_count
        if (cpu_head != NULL){
            cpu_head->remaining_time--;
            slice_count++;
            // If the task finishes then we calculate the tasks completion time
            if (cpu_head->remaining_time == 0){
                cpu_head->completion_time = current_time + 1; // Since completion time is the second after it completes, we add 1
                cpu_head = NULL;
                num_completed++;
                slice_count = 0; // We set slice_count to zero so its ready for the next task to come in
            }
        }
        current_time++; // Incrememnt current time
    }
    // Calculating each tasks response and wait time. Same as sjfp
    for (int i = 0; i < num_tasks; i++){
        tasks[i]->response_time = tasks[i]->completion_time - tasks[i]->arrival_time;
        tasks[i]->wait_time = tasks[i]->response_time - tasks[i]->service_time;
    }
    // Prints the tid, arrival time, service time, completion time, response time, and wait times for each task. Same as sjfp
    printf("\n%-4s %-8s %-9s %-11s %-9s %s\n", "", "arrival", "service","completion", "response","wait");
    printf("%-4s %-8s %-9s %-11s %-9s %s\n", "tid", "time", "time", "time","time", "time");
    printf("-------------------------------------------------\n");
    for (int i = 0;i < num_tasks; i++){
        printf("%-4c %-8d %-9d %-11d %-9d %d\n", tasks[i]->task_id, tasks[i]->arrival_time, tasks[i]->service_time,tasks[i]->completion_time, tasks[i]->response_time, tasks[i]->wait_time);
    }
    // Sorts the tasks array in order of service time ascending. Same as sjfp
    for (int i = 0; i < num_tasks - 1; i++){
    for (int j = 0; j < num_tasks - i - 1; j++) {
        if (tasks[j]->service_time > tasks[j+1]->service_time){
            struct task *temp = tasks[j];
            tasks[j] = tasks[j+1];
            tasks[j+1] = temp;
            }
        }
    }
    // Prints the tasks in service time ascending as well as the tasks wait time. Same as sjfp
    printf("\nService   Wait");
    printf("\ntime      time");
    printf("\n----------------");
    for ( int i = 0; i < num_tasks ; i++){
        printf("\n%d          %d", tasks[i]->service_time, tasks[i]->wait_time);
    }   
    printf("\n");
}

int main (int argc, char * argv[]){
    int time_slice = 0;
    int policy = 0; // 0 = sjfp, 1 = rr
    // Checks for the correct command line arguments
    if (argc < 2){
        printf("Error: Need to specify -rr or -sjfp\n");
        return 1;
    }
    else if (strcmp(argv[1], "-rr") == 0 && argc != 3){
        printf("Need to input value for time slice\n");
        return 1;
        }
    else if (strcmp(argv[1], "-rr") != 0 && strcmp(argv[1], "-sjfp") != 0){
        printf("Error, policy needs to be -rr or -sjfp\n");
        return 1;
    }
    else if (strcmp(argv[1], "-rr") == 0){
        policy = 1;
        time_slice = atoi(argv[2]);
    }

    // Initializes the task struct to hold max of 26 tasks
    struct task *tasks[26];
    int num_tasks = 0;
    int arr;
    int svc;
    while (scanf("%d %d", &arr, &svc) == 2){
        // Scans the stdin line by line and fills in the info about each task
        // Increments number of tasks
        struct task *t = malloc(sizeof(struct task));
        t->task_id = 'A' + num_tasks;
        t->arrival_time = arr;
        t->service_time = svc;
        t->remaining_time = svc;
        t->completion_time = 0;
        t->response_time = -1;
        t->wait_time = 0;
        t->next = NULL;
        tasks[num_tasks] = t;
        num_tasks++;
    }
    
    //Checks if user chose sjfp or rr and calls their respctive functions
    if (policy == 0){
        printf("SJF(preemptive) scheduling results\n\n");
        printf("time cpu ready queue (tid/rst)\n");
        printf("---------------------------------\n");
        sjfp(tasks, num_tasks);
    }
    else{
        printf("RR scheduling results (time slice is %d)\n\n", time_slice);
        printf("time cpu ready queue (tid/rst)\n");
        printf("---------------------------------\n");
        round_robin(tasks, num_tasks, time_slice);
    }
    return 0;
}



