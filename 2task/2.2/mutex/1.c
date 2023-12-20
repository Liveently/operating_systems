#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;

void* thread_function(void* arg) {

    pthread_mutex_unlock(&mutex);
    printf("Поток 2 освободил мутекс\n");
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_mutex_init(&mutex, NULL);

    // Захват мутекса в главном потоке
    pthread_mutex_lock(&mutex);
    printf("Поток 1 захватил мутекс\n");
    
    
    // Создание нового потока
    pthread_create(&tid, NULL, thread_function, NULL);

   

    pthread_join(tid, NULL);
    pthread_mutex_destroy(&mutex);

    return 0;
}
