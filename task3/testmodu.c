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

#define buf_size 100

static DECLARE_WAIT_QUEUE_HEAD(w_queue);
static DECLARE_WAIT_QUEUE_HEAD(r_queue);

static int acme_count = 1;
static dev_t acme_dev = MKDEV(202, 128);

static struct cdev acme_cdev;
static struct buffer
{
	char bu[buf_size];
	char *begin, *end;
} buff;

static char* next_ptr(char *cur)
{
	char *next=cur++;
	if (next>buff.bu+buf_size)
		next=buff.bu;
	return next;
}

static ssize_t acme_read(struct file *file, char __user * buf, size_t count, loff_t * ppos)
{
	int readed = 0;
	char *tmp=kmalloc(sizeof(char)*count,GFP_KERNEL);
	while (readed != count)
	{
		if (buff.begin == buff.end) 
		{
			if (readed != 0) 
				return readed;
			wait_event_interruptible(r_queue, buff.begin != buff.end);
		}
		tmp[readed]=*buff.begin;
		buff.begin = next_ptr(buff.begin);
		readed++;
	}
	copy_to_user(buf, tmp, count);
	wake_up_interruptible(&w_queue);
	kfree(tmp);
	return readed;
}

static ssize_t acme_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int written=0;
	while (written != count)
	{
		if (next_ptr(buff.end)==buff.begin) 
			 wait_event_interruptible(w_queue, next_ptr(buff.end) != buff.begin);
		copy_from_user(buff.end, (buf+written++), 1);
		buff.end = next_ptr(buff.end);
	}
	wake_up_interruptible(&r_queue);
	return written;
}

int icp_open (struct inode *ino, struct file *fl)
{
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
	buff.begin=buff.end=buff.bu;
	cdev_init(&acme_cdev, &acme_fops);

	if (cdev_add(&acme_cdev, acme_dev, acme_count)) {
		err = -ENODEV;
		goto err_dev_unregister;
	}
	return 0;
 err_dev_unregister:
	unregister_chrdev_region(acme_dev, acme_count);
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
