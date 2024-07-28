#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DRIVER_MODULE_NAME "driver"
#define DRIVER_MODULE_PATH "./build/driver.ko"
#define KSENTINEL_MODULE_NAME "ksentinel"
#define KSENTINEL_MODULE_PATH "./build/ksentinel.ko"

int main() {
    int status;

    printf("Loading the driver module...\n");
    if (fork() == 0) {
        execl("/sbin/insmod", "insmod", DRIVER_MODULE_PATH, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    wait(&status);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("Driver module loaded successfully.\n");
    } else {
        fprintf(stderr, "Failed to load driver module.\n");
        return EXIT_FAILURE;
    }

    printf("Loading the ksentinel module...\n");
    if (fork() == 0) {
        execl("/sbin/insmod", "insmod", KSENTINEL_MODULE_PATH, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    wait(&status);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("KSentinel module loaded successfully.\n");
    } else {
        fprintf(stderr, "Failed to load ksentinel module.\n");
        return EXIT_FAILURE;
    }

    sleep(5);

    printf("Unloading the ksentinel module...\n");
    if (fork() == 0) {
        execl("/sbin/rmmod", "rmmod", KSENTINEL_MODULE_NAME, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    wait(&status);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("KSentinel module unloaded successfully.\n");
    } else {
        fprintf(stderr, "Failed to unload ksentinel module.\n");
        return EXIT_FAILURE;
    }

    printf("Unloading the driver module...\n");
    if (fork() == 0) {
        execl("/sbin/rmmod", "rmmod", DRIVER_MODULE_NAME, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    wait(&status);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("Driver module unloaded successfully.\n");
    } else {
        fprintf(stderr, "Failed to unload driver module.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
