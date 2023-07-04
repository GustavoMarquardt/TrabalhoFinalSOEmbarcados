#include <stdio.h>
#include "trabalhoso.h"

Process processes[MAX_PROCESSES];
Queue queues[MAX_NUM_QUEUES];
Scheduler scheduler;

int executaSPN;
int executaSRTN;

// cria os tipos de fila
Queue SRTN; // Shortest Remaining Time Next (Tempo Restante Mais Curto Seguinte ) não tem preempção
Queue SPN;  // Shortest Process Next (Processo mais curto Próximo) tem preempção

int Tempo_execucao = 0;

FILE *arqTxtEscrita;
FILE *arq;
void escreveProcessosNoArquivoDeTexto(int id, int Tempo_execucao)
{

    // função para escrever em um arquivo txt o processo e o tempo de execução, procura a última linha do arquivo e escreve na linha de baixo

    arq = fopen("processosExecutados.txt", "a");
    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
    }
    else
    {
        fprintf(arq, "Processo %d executado no tempo %d\n", id, Tempo_execucao);
    }

    fclose(arq);
}

void scheduler_init(Scheduler *s)
{
    s->num_queues = 0;
}

int scheduler_add_queue(Scheduler *s, Queue *q)
{
    // adiciona a fila ao scheduler
    if (s->num_queues >= MAX_NUM_QUEUES)
    {
        return -1;
    }
    s->queues[s->num_queues] = *q;
    s->num_queues++;
    return s->num_queues - 1;
}

int add_Processes_Queue(Queue *q, Process *p)
{
    if ((q->tail + 1) % MAX_PROCESSES == q->head)
    {
        return -1;
    }
    q->processes[q->tail] = *p;
    q->tail = (q->tail + 1) % MAX_PROCESSES;
    return 0;
}

int executaProcesso(Process *p)
{
    // executa o processo
    p->tempo_exec = p->tempo_exec - QUANTUM;
    if (p->tempo_exec == 0)
    {
        return 1;
    }
    return 0;
}

int removeProcesso(Queue *q, Process *p)
{
    // remove o processo da fila
    if (q->head == q->tail)
    {
        // printf("Fila vazia remove processo\n");
        return -1;
    }
    *p = q->processes[q->head];
    q->head = (q->head + 1) % MAX_PROCESSES;
    // printf("Head = %d\n", q->head);
    // printf("Tail = %d\n", q->tail);
    // zerar o processo
    p->id = 0;
    p->data_criacao = 0;
    p->tempo_exec = 0;
    p->prioridade = 0;
    return 0;
}

void printaProcessos()
{
    for (int i = 0; i < 20; i++)
    {
        printf(" id=(%d)  data_criacao=(%d) tempo_exec=(%d) prioridade=(%d) \n",
               processes[i].id, processes[i].data_criacao, processes[i].tempo_exec, processes[i].prioridade);
    }
}

int readArquivo()
{

    // função para ler o arquivo e armazenar os processos
    FILE *arq;
    int i = 0;
    int id, data_criacao, tempo_execucao, prioridade;
    arq = fopen("processosTeste.txt", "r");
    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return 0;
    }

    while (fscanf(arq, "%d %d %d %d", &id, &data_criacao, &tempo_execucao, &prioridade) != EOF)
    {
        processes[i].id = id;
        processes[i].data_criacao = data_criacao;
        processes[i].tempo_exec = tempo_execucao;
        processes[i].prioridade = prioridade;
        i++;
    }

    fclose(arq);
}
void apagaArquivoEscrita()
{
    // função para escrever em um arquivo txt o processo e o tempo de execução, procura a última linha do arquivo e escreve na linha de baixo
    FILE *arq;
    arq = fopen("processos.txt", "a");
    fclose(arq);
}
int executaProcessoSRTN()
{
    // printf("Executa processo SRTN\n");
    printQueue(&SRTN);

    // procura se existe algum processos menor ou igual ao tempo de execução atual
    int idProcessExec = -1;

    for (int i = 0; i < MAX_PROCESS_PER_QUEUE; i++)
    {
        if (SRTN.processes[i].data_criacao <= Tempo_execucao && SRTN.processes[i].tempo_exec != 0)
        {
            if (idProcessExec == -1)
            {
                idProcessExec = i;
                break;
            }
        }
    }
    // agora procura se existe algum processo menor do que o tempo de execução atual
    for (int i = 0; i < MAX_PROCESS_PER_QUEUE; i++)
    {
        if (SRTN.processes[i].data_criacao <= Tempo_execucao && SRTN.processes[i].tempo_exec != 0)
        {
            if (SRTN.processes[i].tempo_exec < SRTN.processes[idProcessExec].tempo_exec)
            {
                idProcessExec = i;
            }
        }
    }

    if (idProcessExec == -1)
    {
        escreveProcessosNoArquivoDeTexto(-1, Tempo_execucao);
        Tempo_execucao++;
        return 0;
    }

    SRTN.processes[idProcessExec].tempo_exec--;
    printf("Processo que vai executar = %d\n", SRTN.processes[idProcessExec]);
    // chamar funcao que escreve em um arquvio txt o processo e tempo de execucão
    escreveProcessosNoArquivoDeTexto(SRTN.processes[idProcessExec].id, Tempo_execucao);

    if (SRTN.processes[idProcessExec].tempo_exec == 0)
    {
        // printf("Remove processo %d\n", SRTN.processes[idProcessExec].id);
        removeProcesso(&SRTN, &SRTN.processes[idProcessExec]);
    }

    // printQueue(&SRTN);
    //  slepp(5);
    Tempo_execucao++;
    // se todos os processo da fila acabar retorna -1
    if (SRTN.head == SRTN.tail)
    {
        executaSRTN = -1;
        return -1;
    }
    return 0;
}
// aqui está errado mesmo
int executaProcessoSPN()
{
    // printf("Executa processo SRTN\n");
    printQueue(&SPN);

    // procura se existe algum processos menor ou igual ao tempo de execução atual
    int idProcessExec = -1;

    for (int i = 0; i < MAX_PROCESS_PER_QUEUE; i++)
    {
        if (SPN.processes[i].data_criacao <= Tempo_execucao && SPN.processes[i].tempo_exec != 0)
        {
            if (idProcessExec == -1)
            {
                idProcessExec = i;
                break;
            }
        }
    }
    // agora procura se existe algum processo menor do que o tempo de execução atual
    for (int i = 0; i < MAX_PROCESS_PER_QUEUE; i++)
    {
        if (SPN.processes[i].data_criacao <= Tempo_execucao && SPN.processes[i].tempo_exec != 0)
        {
            if (SPN.processes[i].tempo_exec < SPN.processes[idProcessExec].tempo_exec)
            {
                idProcessExec = i;
            }
        }
    }

    if (idProcessExec == -1)
    {
        escreveProcessosNoArquivoDeTexto(-1, Tempo_execucao);
        Tempo_execucao++;
        return 0;
    }
    while (SPN.processes[idProcessExec].tempo_exec > 0)
    {
        SPN.processes[idProcessExec].tempo_exec--;
        printf("Processo que vai executar = %d no tempo = %d\n", SPN.processes[idProcessExec].id, Tempo_execucao);
        escreveProcessosNoArquivoDeTexto(SPN.processes[idProcessExec].id, Tempo_execucao);
        Tempo_execucao++;
    }

    if (SPN.processes[idProcessExec].tempo_exec == 0)
    {
        removeProcesso(&SPN, &SPN.processes[idProcessExec]);
    }
    if (SPN.head == SPN.tail)
    {
        executaSPN = -1;
        return -1;
    }
    return 0;
}

int kernel()
{
    if (SRTN.head == SRTN.tail)
    {
        printf("Fila SRTN vazia\n");
        return 0;
    }
    // verifica se a fila SRTN está vazia
    if (SPN.head == SPN.tail)
    {
        printf("Fila SPN vazia\n");
        return 0;
    }

    // executa o processo da fila SRTN
    int filaSRTN = 0;
    int filaSPN = 0;
    // printQueue(&SPN);
    while (1)
    {
        printf("Tempo de execucao = %d\n", Tempo_execucao);
        if (executaSRTN != -1)
            // Esta esta funcionando direitinho
            filaSRTN = SRTN.process();
        if (executaSPN != -1)
            filaSPN = SPN.process();
        sleep(1);

        if (filaSPN == -1)
        {
            if (filaSRTN == -1)
            {
                printf("Fila SRTN e SPN vazia\n");
                return 0;
            }
        }
    }
}

void printQueue(Queue *q)
{

    if (q->type == 0)
    {
        // printf("Fila SRTN\n");
        for (int i = 0; i < MAX_PROCESS_PER_QUEUE; i++)
        {
            printf("id :%d %d %d %d \n", q->processes[i].id, q->processes[i].data_criacao,
                   q->processes[i].tempo_exec, q->processes[i].prioridade);
        }
    }
    else
    {
        // printf("Fila SPN\n");
        for (int i = 0; i < MAX_PROCESS_PER_QUEUE; i++)
        {
            printf("id: %d %d %d %d \n", q->processes[i].id, q->processes[i].data_criacao,
                   q->processes[i].tempo_exec, q->processes[i].prioridade);
        }
    }
}

int main()
{
    // inicializa o scheduler
    scheduler_init(&scheduler);

    SRTN.type = SRTN_QUEUE; // 0 = SRTN
    SPN.type = SPN_QUEUE;   // 1 = SPN

    SRTN.head = 0;
    SRTN.tail = 0;

    // adiciona as filas ao scheduler
    scheduler_add_queue(&scheduler, &SRTN);
    scheduler_add_queue(&scheduler, &SPN);

    readArquivo();
    // a adição dos processo vai ser de forma que se quando i = par ele vai adicionar na fila SRTN
    //  caso ele seja impar ele vai adicionar na fila SPN
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (i % 2 == 0)
        {
            add_Processes_Queue(&SRTN, &processes[i]);
        }
        else
        {
            add_Processes_Queue(&SPN, &processes[i]);
        }
    }

    if (SRTN.type == 0)
        // SRTN, atribuir o ponteiro de função para a função executaProcessoSRTN
        SRTN.process = executaProcessoSRTN;
    if (SPN.type == 1)
        SPN.process = executaProcessoSPN;
    printaProcessos();
    kernel();
}