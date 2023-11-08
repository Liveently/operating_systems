#include <pthread.h>
#include <stdio.h>
#include <unistd.h>  

struct ThreadParams {
    int num;
    const char *message;
};

void *mythread(void *arg) {
    struct ThreadParams *params = (struct ThreadParams *)arg;
    printf("Thread %d: %s\n", params->num, params->message);
    pthread_exit(NULL);
}

int main() {
     struct ThreadParams params = {1, "Hello from thread"};

     pthread_t tid;

     if (pthread_create(&tid, NULL, mythread, &params) != 0) {
       perror("pthread_create");
      return 1;
     }

    pthread_join(tid, NULL) ;


    sleep(2);
    return 0;
}
