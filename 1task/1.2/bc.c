#include <stdio.h>
#include <pthread.h>
#include <string.h>


void* number() {
    pthread_exit((void*)42);
}

void* string() {
    pthread_exit("Hello, world!");
}

int main() {
    pthread_t tid;

    const int err1 = pthread_create(&tid, NULL, string, NULL);
    if (err1) {
        fprintf(stderr, "main: pthread_create() failed: %s\n", strerror(err1));
        return -1;
    }

    void* ret_value;
   pthread_join(tid, &ret_value);


    //printf("Returned number from created pthread is %ld\n", (long)ret_value);
    printf("Returned string from created pthread is \"%s\"\n", (char*)ret_value);

    return 0;
}
