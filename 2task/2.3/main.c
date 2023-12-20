#include <unistd.h>
#include <pthread.h>
#include "queue.h"

#define STORAGE_CAPACITY 1000
#define THREAD_COUNT 4
#define ASC 0
#define DESC 1
#define EQ 2
#define SWAP 3

FILE* file;


void logMessage(const char* message) {

        fprintf(file, message);

}

void *ascending_thread(void *data) {

    ThreadData *thread_data = (ThreadData *) data;
    Storage *storage = thread_data->storage;
    int *counter = thread_data->counter;

    while (1) {
        Node *curr = storage->first;
        Node *curr2, *tmp;
        while (1) {
            if (curr != NULL && pthread_mutex_trylock(&curr->sync) == 0) {
                if (curr->next != NULL && pthread_mutex_trylock(&curr->next->sync) == 0) {
                    logMessage("Asc: получилось захватить 2 мутекса, работаем\n");
                    volatile int pair_count = 0;
                    curr2 = curr->next;
                    if (strlen(curr->value) < strlen(curr2->value)) {
                        pair_count++;
                    }
                    tmp = curr;
                    curr = curr->next;
                    pthread_mutex_unlock(&tmp->sync);
                    pthread_mutex_unlock(&curr->sync);
                } else {
                    logMessage("Asc: получилось захватить 1 мутекс, скип\n");
                    tmp = curr;
                    curr = curr->next;
                    pthread_mutex_unlock(&tmp->sync);
                }
            } else if (curr == NULL) {
                break;
            } else {
                logMessage("Asc: не получилось захватить мутекс, скип\n");
                curr = curr->next;
            }
        }
        (*counter)++;
    }
}

void *descending_thread(void *data) {
    ThreadData *thread_data = (ThreadData *) data;
    Storage *storage = thread_data->storage;
    int *counter = thread_data->counter;

    while (1) {
        Node *curr = storage->first;
        Node *curr2, *tmp;
        while (1) {
            if (curr != NULL && pthread_mutex_trylock(&curr->sync) == 0) {
                if (curr->next != NULL && pthread_mutex_trylock(&curr->next->sync) == 0) {
                    logMessage("Desc: получилось захватить 2 мутекса, работаем\n");
                    volatile int pair_count = 0;
                    curr2 = curr->next;
                    if (strlen(curr->value) > strlen(curr2->value)) {
                        pair_count++;
                    }
                    tmp = curr;
                    curr = curr->next;
                    pthread_mutex_unlock(&tmp->sync);
                    pthread_mutex_unlock(&curr->sync);
                } else {
                    logMessage("Desc: получилось захватить 1 мутекс, скип\n");
                    tmp = curr;
                    curr = curr->next;
                    pthread_mutex_unlock(&tmp->sync);
                }
            } else if (curr == NULL) {
                break;
            } else {
                logMessage("Desc: не получилось захватить мутекс, скип\n");
                curr = curr->next;
            }
        }
        (*counter)++;
    }
}

void *equal_length_thread(void *data) {
    ThreadData *thread_data = (ThreadData *) data;
    Storage *storage = thread_data->storage;
    int *counter = thread_data->counter;

    while (1) {
        Node *curr = storage->first;
        Node *curr2, *tmp;
        while (1) {
            if (curr != NULL && pthread_mutex_trylock(&curr->sync) == 0) {
                if (curr->next != NULL && pthread_mutex_trylock(&curr->next->sync) == 0) {
                    logMessage("Equal: получилось захватить 2 мутекса, работаем\n");
                    volatile int pair_count = 0;
                    curr2 = curr->next;
                    if (strlen(curr->value) == strlen(curr2->value)) {
                        pair_count++;
                    }
                    tmp = curr;
                    curr = curr->next;
                    pthread_mutex_unlock(&tmp->sync);
                    pthread_mutex_unlock(&curr->sync);
                } else {
                    logMessage("Equal: получилось захватить 1 мутекс, скип\n");
                    tmp = curr;
                    curr = curr->next;
                    pthread_mutex_unlock(&tmp->sync);
                }
            } else if (curr == NULL) {
                break;
            } else {
                logMessage("Equal: не получилось захватить мутекс, скип\n");
                curr = curr->next;
            }
        }
        (*counter)++;
    }
}


void *swap_thread(void *data) {
    ThreadData *thread_data = (ThreadData *) data;
    Storage *storage = thread_data->storage;
    int *counter = thread_data->counter;

    while (1) {
        Node *curr1 = storage->first;

        Node *curr2, *curr3, *tmp;
        while (1) {


            int rnd = rand() % 4;

            for(int i=0; i<=rnd; i++){

                if (curr1 == NULL) break;
                curr1 = curr1->next;
            }


            if (curr1 != NULL && pthread_mutex_trylock(&curr1->sync) == 0) {
                if (curr1->next != NULL && pthread_mutex_trylock(&curr1->next->sync) == 0) {
                    if (curr1->next->next != NULL && pthread_mutex_trylock(&curr1->next->next->sync) == 0) { //если получилось заблочить 3 мутекса


                        logMessage("Swap: получилось захватить 3 мутекса, работаем\n");

                        curr2 = curr1->next;
                        curr3 = curr1->next->next;


                            curr2->next = curr3->next;
                            curr3->next = curr2;
                            curr1->next = curr3;
                            (*counter)++;



                        tmp = curr1;
                        curr1 = tmp->next;
                        curr2 = curr1->next;

                        pthread_mutex_unlock(&tmp->sync);
                        pthread_mutex_unlock(&curr1->sync);
                        pthread_mutex_unlock(&curr2->sync);
                    } else { //если получилось заблочить 2 мутекса
                        logMessage("Swap: получилось захватить 2 мутекса, скип\n");
                        tmp = curr1;
                        curr1 = curr1->next;
                        pthread_mutex_unlock(&tmp->sync);
                        pthread_mutex_unlock(&curr1->sync);
                    }
                } else { //если получилось заблочить один мутекс
                    logMessage("Swap: получилось захватить 1 мутекс, скип\n");
                    tmp = curr1;
                    curr1 = curr1->next;
                    pthread_mutex_unlock(&tmp->sync);
                }
            } else if (curr1 == NULL) {
                break;
            } else {
                logMessage("Swap: не получилось захватить мутекс, скип\n");
                curr1 = curr1->next;
            }
        }
    }
}


void *count_monitor(void *arg) {
    int *counters = (int *) arg;
    while (1) {
        printf("ASC: %d, DESC: %d, EQ: %d, SWAP: %d\n",
               counters[ASC], counters[DESC], counters[EQ], counters[SWAP]);
        sleep(1);
    }
}

int main() {

    file = fopen("log.txt", "a");

    Storage *storage = initialize_storage(STORAGE_CAPACITY);
    fill_storage(storage);
    print_storage(storage);

    pthread_t ascending_tid, descending_tid, equal_length_tid, swap_tid, monitor; //индефикаторы потоков

    int *counters = calloc(THREAD_COUNT, sizeof(int)); // счетчики итераций

    ThreadData ascending_data = {storage, &counters[ASC]};
    ThreadData descending_data = {storage, &counters[DESC]};
    ThreadData equal_data = {storage, &counters[EQ]};
    ThreadData swap_data1 = {storage, &counters[SWAP]};

    pthread_create(&ascending_tid, NULL, ascending_thread, &ascending_data);
    pthread_create(&descending_tid, NULL, descending_thread, &descending_data);
    pthread_create(&equal_length_tid, NULL, equal_length_thread, &equal_data);
    pthread_create(&swap_tid, NULL, swap_thread, &swap_data1);
    pthread_create(&monitor, NULL, count_monitor, counters);

    pthread_join(ascending_tid, NULL);
    pthread_join(descending_tid, NULL);
    pthread_join(equal_length_tid, NULL);
    pthread_join(monitor, NULL);
    pthread_join(swap_tid, NULL);


    fclose(file);

    return 0;
}
