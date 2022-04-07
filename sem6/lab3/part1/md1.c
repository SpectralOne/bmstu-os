#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bogachenko Artem");

static int __init md_init(void)
{
    printk("_ md was loaded\n");
    struct task_struct *task = &init_task;
    do {
        printk(KERN_INFO "_ %s - %d, %s - %d\n", task->comm, task->pid, task->parent->comm, task->parent->pid);
    } while ((task = next_task(task)) != &init_task);
    printk("_ %s - %d, %s - %d\n", current->comm, current->pid, current->parent->comm, current->parent->pid);
    printk("_ done work");
    return 0;
}
static void __exit md_exit(void)
{
    printk("_ md1 Good bye!\n");
}

module_init(md_init);
module_exit(md_exit);
