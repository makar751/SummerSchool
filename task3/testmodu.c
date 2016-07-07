#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/string.h>
#include <linux/mutex.h>

#define buf_size 10

static DECLARE_WAIT_QUEUE_HEAD(w_queue);
static DECLARE_WAIT_QUEUE_HEAD(r_queue);

static int icp_count = 1;
static dev_t icp_dev = MKDEV(202, 128);

static struct cdev icp_cdev;
static struct buffer
{
	char *bu;
	char *begin, *end;
} buff;

static char* next_ptr(char *cur)
{
	char *next=cur+1;
	if (next>buff.bu+buf_size)
		next=buff.bu;
	return next;
}

static ssize_t icp_read(struct file *file, char __user * buf, size_t count, loff_t * ppos)
{
	int readed = 0;
	char *tmp=kmalloc(sizeof(char)*count,GFP_KERNEL);
	while (readed != count)
	{
		if (next_ptr(buff.begin) == buff.end)
			wait_event_interruptible(r_queue, next_ptr(buff.begin) != buff.end);
		tmp[readed]=buff.begin[0];
		buff.begin = next_ptr(buff.begin);
		readed++;
	}
	copy_to_user(buf, tmp, readed);
	wake_up_interruptible(&w_queue);
	kfree(tmp);
	return readed;
}

static ssize_t icp_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	
	int written=0;
	printk("%s\n",buf);
	while (written != count)
	{	
		if (next_ptr(buff.end)==buff.begin) 
			wait_event_interruptible(w_queue, next_ptr(buff.end) != buff.begin);
		copy_from_user(buff.end, (buf+written), 1);
		buff.end = next_ptr(buff.end);
		written++;
	}
	wake_up_interruptible(&r_queue);
	return written;
}

int icp_open (struct inode *ino, struct file *fl)
{
		printk("open\n");
		return 0;	
}

int icp_close (struct inode *ino, struct file *fl)
{
	printk("close\n");
	return 0;
}

static const struct file_operations acme_fops = {
	.owner = THIS_MODULE,
	.read = icp_read,
	.write = icp_write,
	.open=icp_open,
	.release=icp_close
};

static int __init icp_init(void)
{
	int err;
	buff.bu=kmalloc(sizeof(char)*buf_size,GFP_KERNEL);
	buff.begin=buff.bu;
	buff.end=buff.bu;
	cdev_init(&icp_cdev, &acme_fops);

	if (cdev_add(&icp_cdev, icp_dev, icp_count)) {
		err = -ENODEV;
		goto err_dev_unregister;
	}
	return 0;
 err_dev_unregister:
	unregister_chrdev_region(icp_dev, icp_count);
	return err;
}

static void __exit icp_exit(void)
{
	cdev_del(&icp_cdev);
	unregister_chrdev_region(icp_dev, icp_count);
	kfree(buff.bu);
}

module_init(icp_init);
module_exit(icp_exit);
MODULE_LICENSE("GPL");
