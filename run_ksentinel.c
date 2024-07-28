#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define MODULES_DIR "./build/"
#define CONFIG_FILE "modules.conf"

int is_module_loaded(const char *module_name) {
    FILE *fp;
    char path[1035];
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "lsmod | grep -w ^%s", module_name);

    /* Open the command for reading. */
    fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen");
        return 0;
    }

    /* Read the output. */
    if (fgets(path, sizeof(path) - 1, fp) != NULL) {
        /* Module is loaded */
        pclose(fp);
        return 1;
    }

    /* Close the file. */
    pclose(fp);
    return 0;
}

int load_module(const char *module_path) {
    int status;
    if (fork() == 0) {
        execl("/sbin/insmod", "insmod", module_path, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    wait(&status);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

int unload_module(const char *module_name) {
    int status;
    if (fork() == 0) {
        execl("/sbin/rmmod", "rmmod", module_name, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    wait(&status);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

void load_all_modules(const char *directory, const char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char module_name[256];
    char module_path[1024];
    while (fgets(module_name, sizeof(module_name), file)) {
        module_name[strcspn(module_name, "\n")] = '\0'; // Remove newline character
        snprintf(module_path, sizeof(module_path), "%s%s", directory, module_name);
        
        char *name = strrchr(module_name, '/');
        name = name ? name + 1 : module_name;
        name[strlen(name) - 3] = '\0'; // Remove .ko extension
        
        if (is_module_loaded(name)) {
            printf("Module %s is already loaded.\n", name);
            continue;
        }

        printf("Loading module: %s\n", module_path);
        if (load_module(module_path)) {
            printf("Module %s loaded successfully.\n", module_path);
        } else {
            fprintf(stderr, "Failed to load module %s.\n", module_path);
        }
    }

    fclose(file);
}

void unload_all_modules(const char *directory, const char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char module_name[256];
    while (fgets(module_name, sizeof(module_name), file)) {
        module_name[strcspn(module_name, "\n")] = '\0'; // Remove newline character
        char *name = strrchr(module_name, '/');
        name = name ? name + 1 : module_name;
        name[strlen(name) - 3] = '\0'; // Remove .ko extension
        
        printf("Unloading module: %s\n", name);
        if (unload_module(name)) {
            printf("Module %s unloaded successfully.\n", name);
        } else {
            fprintf(stderr, "Failed to unload module %s.\n", name);
        }
    }

    fclose(file);
}

int main() {
    printf("Loading all modules...\n");
    load_all_modules(MODULES_DIR, CONFIG_FILE);

    sleep(5);

    printf("Unloading all modules...\n");
    unload_all_modules(MODULES_DIR, CONFIG_FILE);

    return EXIT_SUCCESS;
}
