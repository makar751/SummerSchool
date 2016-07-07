#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#define GPIO_NUM 67
#define GPIO_NAME "Interrupt Line"
#define IRQ_NAME "GPIO IRQ"

static int gpio_irq_num=0;

irqreturn_t irq_handler(int irq, void *notused)
{
	printk("IRQ Intterupt\n");
	return IRQ_HANDLED;
}

int init_module(void)
{
	if (gpio_request(GPIO_NUM,GPIO_NAME))
	{
		printk("IRQ failed\n");
		return 0;
	}
	if ((gpio_irq_num=gpio_to_irq(GPIO_NUM))<0)
	{
		printk("IRQ failed\n");
		return 0;
	}
	if (request_irq(gpio_irq_num,irq_handler,0,IRQ_NAME,NULL))
	{
		printk("IRQ failed\n");
		return 0;
	}
	irq_set_irq_type(gpio_irq_num,IRQ_TYPE_EDGE_RISING | IRQ_TYPE_EDGE_FALLING);
	return 0;
}

void cleanup_module(void)
{
	free_irq(gpio_irq_num,NULL);
	gpio_free(GPIO_NUM);
}

MODULE_LICENSE("GPL");
