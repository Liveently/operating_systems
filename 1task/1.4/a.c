#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>



void* routine() {

     while(1) {
     	printf("%s\n", "hello");
     }

}

int main() {
    pthread_t tid;

    const int err1 = pthread_create(&tid, NULL, routine, NULL);
    if (err1) {
        fprintf(stderr, "main: pthread_create() failed: %s\n", strerror(err1));
        return 1;
    }

    sleep(3);

    const int err2 = pthread_cancel(tid);
    if (err2) {
        fprintf(stderr, "main: pthread_cancel() failed: %s\n", strerror(err2));
        return 1;
    }

    const int err3 = pthread_join(tid, NULL);
    if (err3) {
        fprintf(stderr, "main: pthread_join() failed: %s\n", strerror(err3));
        return 1;
    }
}
