#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *block_signals(void *arg) {
    printf("block_signals [%d]: Hello from mythread!\n", gettid());
    sigset_t set; //множество сигналов
    sigfillset(&set);  // Блокировать все сигналы
    sigaddset(&set, SIGINT);

    // Установить блокировку для текущего потока
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    while (1) {
        sleep(1);
    }
    return NULL;
}

void sigint_handler(int sig) { // Обработчик сигнала SIGINT
    printf("Received SIGINT\n");
}

void *handle_sigint(void *arg) {
    printf("handle_sigint [%d]: Hello from mythread!\n", gettid());

    // настройка обработчика сигнала сигинт
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    while (1) {
        sleep(1);
    }
    return NULL;
}

void *handle_sigquit(void *arg) {
    printf("handle_sigquit [%d]: Hello from mythread!\n", gettid());

    int sig;
    sigset_t set; //создание множества сигналов
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT); //добавление сигинт и сигквайт
    sigaddset(&set, SIGINT);

    sigwait(&set, &sig); // Ожидание сигнала из множества
    printf("Received SIGQUIT\n");

    while (1) {
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t block_thread;  // sigint_thread, sigquit_thread;
    pthread_t sigint_thread;
    pthread_t  sigquit_thread;

    printf("mythread [%d]: Hello from mythread!\n", getpid());
    pthread_create(&block_thread, NULL, block_signals, NULL);
    pthread_create(&sigint_thread, NULL, handle_sigint, NULL);
    pthread_create(&sigquit_thread, NULL, handle_sigquit, NULL);

    pthread_join(block_thread, NULL);
     pthread_join(sigint_thread, NULL);
     pthread_join(sigquit_thread, NULL);
    pause();
    return 0;
}
