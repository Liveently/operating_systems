#include <unistd.h>
#include <pthread.h>
#include "queue.h"

#define STORAGE_CAPACITY 1000
#define THREAD_COUNT 5
#define ASC 0
#define DESC 1
#define EQ 2
#define SWAP1 3
#define SWAP2 4
#define SWAP3 5

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


        while (curr != NULL) {

            pthread_spin_lock(&curr->sync);

            if (curr->next != NULL) {
                pthread_spin_lock(&curr->next->sync);

                logMessage("Equal: получилось захватить 2 мутекса, работаем\n");

                volatile int pair_count = 0;
                curr2 = curr->next;
                if (strlen(curr->value) < strlen(curr2->value)) {
                    pair_count++;
                }
                tmp = curr;
                curr = curr->next;
                pthread_spin_unlock(&tmp->sync);
                pthread_spin_unlock(&curr->sync);
            }else {
                pthread_spin_unlock(&curr->sync);
                break;
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

        while (curr != NULL) {

            pthread_spin_lock(&curr->sync);

            if (curr->next != NULL) {
                pthread_spin_lock(&curr->next->sync);

                logMessage("Equal: получилось захватить 2 мутекса, работаем\n");

                volatile int pair_count = 0;
                curr2 = curr->next;
                if (strlen(curr->value) > strlen(curr2->value)) {
                    pair_count++;
                }
                tmp = curr;
                curr = curr->next;
                pthread_spin_unlock(&tmp->sync);
                pthread_spin_unlock(&curr->sync);
            }else {
                pthread_spin_unlock(&curr->sync);
                break;
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


        while (curr != NULL) {

            pthread_spin_lock(&curr->sync);

            if (curr->next != NULL) {
                pthread_spin_lock(&curr->next->sync);

                logMessage("Equal: получилось захватить 2 мутекса, работаем\n");

                volatile int pair_count = 0;
                curr2 = curr->next;
                if (strlen(curr->value) == strlen(curr2->value)) {
                    pair_count++;
                }
                tmp = curr;
                curr = curr->next;
                pthread_spin_unlock(&tmp->sync);
                pthread_spin_unlock(&curr->sync);
            }else {
                pthread_spin_unlock(&curr->sync);
                break;
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


        while (curr1 != NULL) {

            pthread_spin_lock(&curr1->sync);


            if (curr1->next != NULL) {
                pthread_spin_lock(&curr1->next->sync);
                curr2=curr1->next;


                if ( curr1->next->next != NULL){

                    pthread_spin_lock(&curr1->next->next->sync);

                    curr3=curr2->next;

                    logMessage("Swap: получилось захватить 3 мутекса, работаем\n");

                    if (rand() % 2 == 0) {
                        curr2->next = curr3->next;
                        curr3->next = curr2;
                        curr1->next = curr3;
                        (*counter)++;
                    }

                    tmp = curr1;
                    curr1 = tmp->next;
                    curr2 = curr1->next;

                    pthread_spin_unlock(&tmp->sync);
                    pthread_spin_unlock(&curr1->sync);
                    pthread_spin_unlock(&curr2->sync);

                } else{
                    pthread_spin_unlock(&curr1->sync);
                    pthread_spin_unlock(&curr1->next->sync);
                    (*counter)++;
                    break;
                }


            }else {
                pthread_spin_unlock(&curr1->sync);
                (*counter)++;
                break;
            }

            (*counter)++;

        }


    }
}

void *count_monitor(void *arg) {
    int *counters = (int *) arg;
    while (1) {
        printf("ASC: %d, DESC: %d, EQ: %d, SWAP1: %d, SWAP2: %d, SWAP3: %d  \n",
               counters[ASC], counters[DESC], counters[EQ], counters[SWAP1], counters[SWAP2], counters[SWAP3]);
        sleep(1);
    }
}

int main() {

    file = fopen("log.txt", "a");

    Storage *storage = initialize_storage(STORAGE_CAPACITY);
    fill_storage(storage);
    print_storage(storage);

    pthread_t ascending_tid, descending_tid, equal_length_tid, swap_tid1, swap_tid2, swap_tid3, monitor; //индефикаторы потоков

    int *counters = calloc(THREAD_COUNT, sizeof(int)); // счетчики итераций

    ThreadData ascending_data = {storage, &counters[ASC]};
    ThreadData descending_data = {storage, &counters[DESC]};
    ThreadData equal_data = {storage, &counters[EQ]};
    ThreadData swap_data1 = {storage, &counters[SWAP1]};
    ThreadData swap_data2 = {storage, &counters[SWAP2]};
    ThreadData swap_data3 = {storage, &counters[SWAP3]};


    pthread_create(&ascending_tid, NULL, ascending_thread, &ascending_data);
    pthread_create(&descending_tid, NULL, descending_thread, &descending_data);
    pthread_create(&equal_length_tid, NULL, equal_length_thread, &equal_data);
    pthread_create(&swap_tid1, NULL, swap_thread, &swap_data1);
    pthread_create(&swap_tid2, NULL, swap_thread, &swap_data2);
    pthread_create(&swap_tid3, NULL, swap_thread, &swap_data3);
    pthread_create(&monitor, NULL, count_monitor, counters);




    pthread_join(ascending_tid, NULL);
    pthread_join(descending_tid, NULL);
    pthread_join(equal_length_tid, NULL);
    pthread_join(monitor, NULL);
    pthread_join(swap_tid1, NULL);
    pthread_join(swap_tid2, NULL);
    pthread_join(swap_tid3, NULL);

    fclose(file);

    return 0;
}
