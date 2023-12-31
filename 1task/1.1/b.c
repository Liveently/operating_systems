#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
    return NULL;
}

int main() {
    pthread_t tid[5];
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());



    for(int i = 0; i < 5; ++i) {
        err = pthread_create(&tid[i], NULL, mythread, NULL);
        if (err) {
            fprintf(stderr, "main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    for(int i = 0; i < 5; ++i) {
        err = pthread_join(tid[i], NULL);
        if (err) {
            fprintf(stderr, "main: pthread_join() failed %s\n", strerror(err));
        }
    }
    
    return 0;
}
