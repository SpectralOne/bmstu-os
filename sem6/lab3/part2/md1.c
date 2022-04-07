#include "md.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bogachenko Artem");

char* md1_data = "Привет мир!";
int md1_number = 17;

extern char* md1_proc(void)
{
    return md1_data;
}
extern int md1_fact(int n) {
    int i = 1, res = 1;
    while (i++ < n)
        res *= i;
    return res;
}
static char* md1_local(void)
{
    return md1_data;
}
extern char* md1_noexport(void)
{
    return md1_data;
}

EXPORT_SYMBOL(md1_number);
EXPORT_SYMBOL(md1_fact);
EXPORT_SYMBOL(md1_data);
EXPORT_SYMBOL(md1_proc);
// EXPORT_SYMBOL(md1_local);
EXPORT_SYMBOL(md1_noexport);


static int __init md_init(void)
{
    printk("+ module md1 start!\n");
    return 0;
}

static void __exit md_exit(void)
{
    printk("+ module md1 unloaded!\n");
}

module_init(md_init);
module_exit(md_exit);
