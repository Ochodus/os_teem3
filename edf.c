#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CNT 2
#define MAX_LINE_LENGTH 20
#define input_data_path "./edf_task_data.txt"

int timer = 0;
int idle_time = 0;

// ?�로?�스 ?�행?? ?�요?? ?�보�? ?�는 구조�? (arrival time, burst time, deadline)
typedef struct Task {
    int arrival_t;
    int burst_t;
    int deadline;
} Task;

typedef struct TaskStatus {
    int waiting_t;
    int turnaround_t;
    int response_t;
    int remain_burst_t;
    int remain_deadline;
    int cpu_flag;
    Task task;
} TaskStatus;

typedef struct Node
{
    TaskStatus data;
    struct Node *next;
}Node;
 
 
typedef struct Queue //Queue 구조�? ?�의
{
    Node *front; //�? ??(꺼낼 ?�치)
    Node *rear; //�? ??(보�??? ?�치)
    int count;//보�? 개수
}Queue;
 
void init_queue(Queue *queue);//?? 초기??
int is_empty(Queue *queue); //?��? 비었?��? ?�인
void enqueue(Queue *queue, TaskStatus data); //?�에 보�?
void sorted_enqueue(Queue *queue, TaskStatus data); //?�에 보�?
TaskStatus dequeue(Queue *queue); //?�에?? 꺼냄

Queue ready_queue;

void init_queue(Queue *queue)
{
    queue->front = queue->rear = NULL; //front?� rear�? NULL�? ?�정
    queue->count = 0;//보�? 개수�? 0?�로 ?�정
}

int is_empty(Queue *queue)
{
    return queue->count == 0;    //보�? 개수가 0?�면 �? ?�태
}

void enqueue(Queue *queue, TaskStatus data)
{

}

void sorted_enqueue(Queue *queue, TaskStatus data)
{
    Node *now = (Node *)malloc(sizeof(Node)); //?�드 ?�성
    now->data = data;//?�이?? ?�정
    now->next = NULL;
 
    if (is_empty(queue))//?��? 비어?�을 ??
    {
        queue->front = now;//�? ?�을 now�? ?�정
    }
    else//비어?��? ?�을 ??
    {
        Node * curr = queue->front;
        Node * prev = NULL;
        for(int i = 0 ; i < queue->count; i++)
        {
            if (curr->data.remain_deadline > now->data.remain_deadline)
                break;
            prev = curr;
            curr = curr->next;
        }
        // enqueue ?�는 값이 가?? ?��? deadline?? 가�? 경우
        if (curr == queue->front)
        {
            now->next = queue->front;
            queue->front = now;
        }
        // enqueue ?�는 값이 가?? ?? deadline?? 가�? 경우
        else if (curr == NULL)
        {
            now->next = NULL;
            queue->rear->next = now;
        }
        else
        {
            prev->next = now;
            now->next = curr;
        }
    }
    queue->count++;//보�? 개수�? 1 증�?
}
 
TaskStatus dequeue(Queue *queue)
{
    TaskStatus re;
    Node *now;
    if (is_empty(queue))//?��? 비었?? ??
    {
        re.cpu_flag = -1;
        return re;
    }
    now = queue->front;//�? ?�의 ?�드�? now?? 기억
    re = now->data;//반환?? 값�? now?? data�? ?�정
    queue->front = now->next;//�? ?��? now?? ?�음 ?�드�? ?�정
    free(now);//now ?�멸
    queue->count--;//보�? 개수�? 1 감소
    return re;
}

// ?�로?�스 ?�행 ?�보?�의 배열
Task tasks[MAX_CNT];

void input_read(){
    char *resource_path = input_data_path;
    char buffer[MAX_LINE_LENGTH];
    Task task;
    int file_size = 0;
    FILE * fp;
    
    /* ?�일?? ?�다. */
    if ((fp = fopen(resource_path, "rb")) == NULL) {
        return;
    }
    for(int i = 0; fgets(buffer, MAX_LINE_LENGTH, fp) != NULL; i++){
        char * temp = strtok(buffer, " ");
        task.arrival_t = atoi(temp);
        temp = strtok(NULL, " ");
        task.burst_t = atoi(temp);
        temp = strtok(NULL, " ");
        task.deadline = atoi(temp);
        // printf("arrival: %d burst: %d deadline: %d\n", task.arrival_t, task.burst_t, task.deadline);
        tasks[i] = task;
    }
    fclose(fp);
}

void ready_sorted_enqueue(){
    for (int i = 0; i < MAX_CNT; i++)
    {
        if (tasks[i].arrival_t == timer)
        {
            TaskStatus task_status;
            task_status.turnaround_t = 0;
            task_status.waiting_t = 0;
            task_status.response_t = 0;
            task_status.remain_burst_t = tasks[i].burst_t;
            task_status.cpu_flag = 0;
            task_status.task = tasks[i];
            task_status.remain_deadline = tasks[i].deadline + timer;
            sorted_enqueue(&ready_queue, task_status);
        }
        else if (timer > tasks[i].arrival_t && (timer - tasks[i].arrival_t) % tasks[i].deadline == 0)
        {
            TaskStatus task_status;
            task_status.turnaround_t = 0;
            task_status.waiting_t = 0;
            task_status.response_t = 0;
            task_status.remain_burst_t = tasks[i].burst_t;
            task_status.cpu_flag = 0;
            task_status.task = tasks[i];
            task_status.remain_deadline = tasks[i].deadline + timer;
            sorted_enqueue(&ready_queue, task_status);
        }
    }
}

/*
    ready queue?? ?�어가 ?�는 task?�의 ?�성?�을 1�? ?�데?�트 ?�다.
*/
void update_ready_queue(Queue *queue) // deadline, response time, turnaround time, waiting
{
    Node * cur = queue->front;
    if(queue->front != NULL)
    {
        for(int i = 0; i < queue->count; i++)
        {
            cur->data.turnaround_t += 1; // always increase
            if(cur != queue->front)
            {
                cur->data.waiting_t += 1; // increasse waiting time when not grabbing cpu
            }
            if(cur->data.cpu_flag == 1)
            {
                cur->data.response_t += 1;
            }
            if(cur->data.remain_deadline > timer)
            {
                // dequeue
            }
            
            cur = cur->next;
        }
    }
    return;
}

/*
    ?�재 CPU�? ?�해 ?�행중인 task?? ?�성?�을 1�? ?�데?�트 ?�다.
*/
void update_running_task(){
    TaskStatus * running_task_status =  &ready_queue.front->data;
    if (running_task_status->cpu_flag == -1)
    {
        idle_time += 1;
        return;
    }
    else if (running_task_status->cpu_flag == 0)
    {
        running_task_status->cpu_flag = 1;
        running_task_status->response_t = running_task_status->waiting_t;
    }
    else if (running_task_status->cpu_flag == 1)
    {
        running_task_status->remain_deadline -= 1;
        running_task_status->remain_burst_t -= 1;
        running_task_status->turnaround_t += 1;
    }
    
    if (running_task_status->remain_burst_t <= 0)
    {
        dequeue(&ready_queue);
        return;
    }
}

/*
    ?��?줄링?? ?�반?�인 ?�행결과�? 계산?�다.
*/
void cal_performance(Queue* result)
{
    float tot_waiting_t = 0.0;
    float tot_turnaround_t = 0.0;
    float tot_response_t = 0.0;
    float avg_waiting_t = 0.0;
    float avg_turnaround_t = 0.0;
    float avg_response_t = 0.0; 
    int total_count = result->count;   
    
    while(result->count > 0){
        TaskStatus task = dequeue(result);
        tot_waiting_t += task.waiting_t;
        tot_turnaround_t += task.turnaround_t;
        tot_response_t += task.response_t;
    }

    avg_waiting_t = tot_waiting_t/total_count;
    avg_turnaround_t = tot_turnaround_t/total_count;
    avg_response_t = tot_response_t/total_count;

    printf("avg waiting time: %.3f\n",avg_waiting_t);
    printf("avg turnaround time: %.3f\n",avg_turnaround_t);
    printf("avg response time: %.3f\n",avg_response_t);
}

int main(void){
    init_queue(&ready_queue);
    input_read();
    while(timer < 160){
        ready_sorted_enqueue();
        update_ready_queue();
        update_running_task();
        timer++;
    }
    // cal_performance(result_queue);
    return 0;
}