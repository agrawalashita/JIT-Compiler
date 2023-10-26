/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * jitc.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include "system.h"
#include "jitc.h"

/**
 * Needs:
 *   fork()
 *   execv()
 *   waitpid()
 *   WIFEXITED()
 *   WEXITSTATUS()
 *   dlopen()
 *   dlclose()
 *   dlsym()
 */

/* research the above Needed API and design accordingly */


struct jitc
{
    void *library;
};

int jitc_compile(const char *input, const char *output)
{
    int pid,status;

    char* args[] = {"gcc", "-O3", "-fpic", "-shared", "-o", NULL, NULL, NULL};
    args[5]=(char *)output;
    args[6]=(char *)input;

    pid = fork();
    
    if(pid == 0)
    {
        if(execv("/usr/bin/gcc",args) == -1)
        {
            exit(1);
        }
    }
    else
    {
        int exit_status = WEXITSTATUS(status);
        waitpid(pid,&status,0);

        if (WIFEXITED(status)) {
         /*Child process exited normally with status 0*/
            return 0;
        } else {
            printf("Child process did not exit normally, failed with status %d\n", exit_status);
            return 1;
        }
    }

    return -1;
}

struct jitc *jitc_open(const char *pathname)
{
    struct jitc *jitc = malloc(sizeof(struct jitc));
    if(jitc == NULL)
    {
        TRACE("Out of memory");
        return NULL;
    }

    jitc->library = dlopen(pathname, RTLD_LAZY | RTLD_LOCAL);

    if(jitc->library == NULL)
    {
        TRACE(dlerror());
        free(jitc);
        return NULL;
    }
    return jitc;
}

void jitc_close(struct jitc *jitc)
{
    if(jitc != NULL){
        if(jitc->library != NULL){
            dlclose(jitc->library);
        }
    }
   free(jitc);
}


long jitc_lookup(struct jitc *jitc, const char *symbol)
{
    void *evaluate = dlsym(jitc->library,symbol);

    if(evaluate == NULL)
    {
        TRACE("dlsym");
        dlclose(jitc->library);
        return 0;
    }

    return (long)evaluate;
}
