#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bogachenko Artem");

#define KEYB_IRQ 1

char my_tasklet_data[] = "KEYBOARD IRQ";
struct tasklet_struct my_tasklet;

void my_tasklet_func(unsigned long data) {
    printk(KERN_DEBUG "+ Tasklet: state - %d, count - %d, data - %ld\n",
           my_tasklet.state, my_tasklet.count, my_tasklet.data);
}

irqreturn_t irq_handler(int irq, void *dev) {
    if (irq == KEYB_IRQ) {
        tasklet_schedule(&my_tasklet);
        printk(KERN_DEBUG "+ Interrupt - KEYBOARD!\n");

        return IRQ_HANDLED;
    } else
        return IRQ_NONE;
}

static int __init my_module_init(void) {
    printk(KERN_DEBUG "+ loaded!\n");

    int ret = request_irq(KEYB_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED,
                          "keyboard", (void *)(irq_handler));

    if (ret != 0) {
        printk(KERN_ERR "+ KEYBOARD IRQ handler wasn't registered");
        return ret;
    }

    printk(KERN_DEBUG "+ KEYBOARD IRQ handler was registered successfully");

    tasklet_init(&my_tasklet, my_tasklet_func, (void *)(irq_handler));
    return ret;
}

static void __exit my_module_exit(void) {
    free_irq(KEYB_IRQ, (void *)(irq_handler));

    tasklet_disable(&my_tasklet);
    tasklet_kill(&my_tasklet);
    printk(KERN_DEBUG "+ unloaded!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
