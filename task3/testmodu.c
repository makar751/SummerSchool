#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

/* Physical address of the imaginary acme device */

static void *acme_buf;
static int acme_bufsize = sizeof(int);

static int acme_count = 1;
static dev_t acme_dev = MKDEV(202, 128);

static struct cdev acme_cdev;


static ssize_t
acme_read(struct file *file, char __user * buf, size_t count, loff_t * ppos)
{
	printk("file read\n");
	/* The acme_buf address corresponds to a device I/O memory area */
	/* of size acme_bufsize, obtained with ioremap() */
	int remaining_size, transfer_size;

	remaining_size = acme_bufsize - (int)(*ppos);
				/* bytes left to transfer */
	if (remaining_size == 0) {
				/* All read, returning 0 (End Of File) */
		return 0;
	}

	/* Size of this transfer */
	transfer_size = min_t(int, remaining_size, count);

	if (copy_to_user
	    (buf /* to */ , acme_buf + *ppos /* from */ , transfer_size)) {
		return -EFAULT;
	} else {		/* Increase the position in the open file */
		*ppos += transfer_size;
		return transfer_size;
	}
}

static ssize_t
acme_write(struct file *file, const char __user *buf, size_t count,
	   loff_t *ppos)
{
	printk("file write\n");
if (file->f_mode & FMODE_WRITE)
{
	int remaining_bytes;

	/* Number of bytes not written yet in the device */
	remaining_bytes = acme_bufsize - (*ppos);

	if (count > remaining_bytes) {
		/* Can't write beyond the end of the device */
		return -EIO;
	}

	if (copy_from_user(acme_buf + *ppos /*to*/ , buf /*from*/ , count)) {
		return -EFAULT;
	} else {
		/* Increase the position in the open file */
		*ppos += count;
		return count;
	}
}
else
return -EBUSY;	
}

int icp_open (struct inode *ino, struct file *fl)
{
	printk("file open\n");
		return 0;
	
}

int icp_close (struct inode *ino, struct file *fl)
{
	return 0;
}

static const struct file_operations acme_fops = {
	.owner = THIS_MODULE,
	.read = acme_read,
	.write = acme_write,
	.open=icp_open,
	.release=icp_close
};

static int __init acme_init(void)
{
	int err;
	acme_buf=kmalloc(sizeof(int),GFP_KERNEL);
	if (!acme_buf) {
		err = -ENOMEM;
		goto err_exit;
	}

	cdev_init(&acme_cdev, &acme_fops);

	if (cdev_add(&acme_cdev, acme_dev, acme_count)) {
		err = -ENODEV;
		goto err_dev_unregister;
	}
	printk("before0\n");
	return 0;
	printk("after0\n");
 err_dev_unregister:
	unregister_chrdev_region(acme_dev, acme_count);
 err_exit:
	return err;
}

static void __exit acme_exit(void)
{
	cdev_del(&acme_cdev);
	unregister_chrdev_region(acme_dev, acme_count);
}

module_init(acme_init);
module_exit(acme_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Example character driver");
MODULE_AUTHOR("Free Electrons");
