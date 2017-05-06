/*
 * compilation: gcc -std=gnu11 -Wall basic_fork_ex.c -o basic_fork_ex
 */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include "../addr_ioctl.h"

#define DEV_ADDR "/dev/addr"

static int addr_fd;

void lookup_paddr_prepare(){
    addr_fd = open(DEV_ADDR, O_RDWR);

    if( addr_fd == -1 ){
        perror("open error");
        exit(1);
    }
}

/* ioctl wrapper */
unsigned long lookup_paddr(void* vaddr){
    struct vaddr virt_addr;
    virt_addr.pid = getpid();
    virt_addr.addr = (unsigned long) vaddr;

    unsigned long paddr = ioctl(addr_fd, ADDR_TRANS, &virt_addr);
    
    return paddr;
}

void child_process();
void parent_process();
int *mem_alloc;

void child_process()
{
    int temp;
    sleep(2);
    unsigned long paddr = lookup_paddr(mem_alloc);
    printf("Child pid: %d. [Var 'mem_alloc']vaddr: 0x%lx, paddr: 0x%lx, val: %d\n",
            getpid(), (unsigned long)mem_alloc, paddr, *mem_alloc);
    
    temp = *mem_alloc;
    *mem_alloc = 1;
    printf("\n*** Modify variable 'mem_alloc' from %d to %d ***\n\n", temp, *mem_alloc);

    sleep(2);
    paddr = lookup_paddr(mem_alloc);
    printf("Child pid: %d. [Var 'mem_alloc']vaddr: 0x%lx, paddr: 0x%lx, val: %d\n",
            getpid(), (unsigned long)mem_alloc, paddr, *mem_alloc);
}

void parent_process()
{
    sleep(1);
    unsigned long paddr = lookup_paddr(mem_alloc);
    printf("Parent pid: %d. [Var 'mem_alloc']vaddr: 0x%lx, paddr: 0x%lx, val: %d\n",
            getpid(), (unsigned long)mem_alloc, paddr, *mem_alloc);
    
    sleep(2); 
    paddr = lookup_paddr(mem_alloc);
    printf("Parent pid: %d. [Var 'mem_alloc']vaddr: 0x%lx, paddr: 0x%lx, val: %d\n",
            getpid(), (unsigned long)mem_alloc, paddr, *mem_alloc);
}



int main()
{
    int status;
    pid_t pid;
    
    lookup_paddr_prepare();

    // sleep(100);

    mem_alloc = (int*) malloc(sizeof(int));
    *mem_alloc = 1000;

    pid = fork();
    
    if (pid == 0)
        child_process();
    else
        parent_process();

    wait(&status);
    return 0;
}
