#ifndef __ADDR_IOCTL_H__
#define __ADDR_IOCTL_H__

/*
 * ref:
 *  http://www.makelinux.net/ldd3/chp-6-sect-1
 *  http://elixir.free-electrons.com/linux/latest/source/Documentation/ioctl/ioctl-number.txt
 */

/* Gives us access to ioctl macros _IO and friends below. */
#include <linux/ioctl.h>

struct vaddr {
    pid_t pid;
    unsigned long addr;
};

#define ADDR_IOC_MAGIC  0xD1
#define ADDR_TRANS _IOW(ADDR_IOC_MAGIC, 1, struct vaddr)
#define ADDR_IOC_MAXNR 1


#endif /* end of include guard: __ADDR_IOCTL_H__ */

