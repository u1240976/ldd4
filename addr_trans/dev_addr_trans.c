#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>    /* printk() */
#include <linux/slab.h>        /* kmalloc() */
#include <linux/fs.h>        /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/cdev.h>

#include <asm/uaccess.h>    /* copy_*_user */

#include "addr_ioctl.h"

MODULE_LICENSE("Dual BSD/GPL");

unsigned long lookup_paddr(pid_t pid, unsigned long vaddr);

struct addr_dev {
    struct cdev cdev;
};
struct addr_dev addr_device;

#define DEV_MAJOR 141
#define DEV_MINOR 0

int addr_open(struct inode *inode, struct file *filp)
{
    struct addr_dev *dev;

    dev = container_of(inode->i_cdev, struct addr_dev, cdev);
    filp->private_data = dev;

    return 0;
}

int addr_release(struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t addr_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

ssize_t addr_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

long addr_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    /*
     * ref:
     *  [ldd4] scull/main.c: scull_ioctl()
     *  [linux-4.7] arch/x86/kvm/x86.c: kvm_arch_dev_ioctl() KVM_GET_MSR_INDEX_LIST
     */

    int err = 0;
    
    /*
     * Extract the type and number bitfields, and don't decode incorrect
     * cmds: return ENOTTY (inappropriate ioctl) before access_ok().
     */
    if (_IOC_TYPE(cmd) != ADDR_IOC_MAGIC)
        return -ENOTTY;
    if (_IOC_NR(cmd) > ADDR_IOC_MAXNR)
        return -ENOTTY;

    /*
     * The direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while access_ok is
     * kernel-oriented, so the concept of "read" and "write" is reversed.
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    if (err)
        return -EFAULT;

    switch(cmd) {
        case ADDR_TRANS: {
            void __user *argp = (void __user*)arg;
            struct vaddr __user* user_virt_addr = (struct vaddr __user*)argp;
            struct vaddr virt_addr;
            copy_from_user(&virt_addr, user_virt_addr, sizeof(struct vaddr));

            printk("translate (pid, vaddr) = (%d, %lx)\n", virt_addr.pid, virt_addr.addr);
            unsigned long paddr = lookup_paddr(virt_addr.pid, virt_addr.addr);
            return paddr;
        }

        default:  /* Redundant, as cmd was checked against MAXNR. */
            return -ENOTTY;
    }

    return 0;
}

struct file_operations addr_fops = {
    .owner = THIS_MODULE,
    .open = addr_open,
    .release = addr_release,
    .read = addr_read,
    .write = addr_write,
    .unlocked_ioctl = addr_ioctl,
};

int dev_addr_init_module(void)
{
    int result;
    int err;

    dev_t devno = MKDEV(DEV_MAJOR, DEV_MINOR);
    result = register_chrdev_region(devno, 1, "addr");
    if (result < 0) {
        printk(KERN_WARNING "addr: can't get major %d\n", devno);
        return result;
    }

    /* addr device init */
    cdev_init(&addr_device.cdev, &addr_fops);
    addr_device.cdev.owner = THIS_MODULE;
    addr_device.cdev.ops = &addr_fops;
    err = cdev_add(&addr_device.cdev, devno, 1);

    if (err)
        printk(KERN_NOTICE "Error %d adding addr", err);

    return 0;
}

void dev_addr_cleanup_module(void){
    dev_t devno = MKDEV(DEV_MAJOR, DEV_MINOR);
        
    cdev_del(&addr_device.cdev);
    unregister_chrdev_region(devno, 1);
}

module_init(dev_addr_init_module)
module_exit(dev_addr_cleanup_module)
