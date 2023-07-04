#ifndef trabalhoso.h
#define trabalhoso.h

#define MAX_PROCESSES 8
#define MAX_PROCESS_PER_QUEUE 4
#define MAX_NUM_QUEUES 2
#define QUANTUM 1
#define  SRTN_QUEUE 0 
#define SPN_QUEUE 1

typedef struct {
    int id;
    int data_criacao;
    int tempo_exec;
    int prioridade;
    int executando;
} Process;

typedef struct {
    int type;
    Process processes[MAX_PROCESSES];
    int head;
    int tail;
    //ponteiro de função para o processo, com retorno int sem
    int (*process)();
    
} Queue;

typedef struct {
    Queue queues[MAX_NUM_QUEUES];
    int num_queues;
} Scheduler;

void scheduler_init(Scheduler *s);
int add_Processes_Queue(Queue *q, Process *p);
int scheduler_enqueue(Scheduler *s, int queue_idx, Process *process);
Process *scheduler_dequeue(Scheduler *s);
int executaProcesso(Process *p);
int removeProcesso(Queue *q, Process *p);
void printaProcessos();
int readArquivo();
void apagaArquivoEscrita();
int executaProcessoSRTN();
int executaProcessoSPN();
int kernel();
void printQueue();

#endif  