#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <asm-generic/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bogachenko Artem");

#define IRQ_NUMBER 1

struct workqueue_struct *workqueue;
struct work_struct *work;

int tmp;
unsigned char scancode;

char * scancode2ASCII(unsigned char code) {
	char * ascii[84] = {
		" ", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "Backspace", 
		"Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Ctrl",
		"A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "\"", "'", "Shift (left)", "\\", 
		"Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "Shift (right)", 
		"*", "Alt", "Space", "CapsLock", 
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
		"NumLock", "ScrollLock", "Home", "Up", "Page-Up", "-", "Left",
		" ", "Right", "+", "End", "Down", "Page-Down", "Insert", "Delete"
	};
	if (code < 84)
		return ascii[code];
	return "Error";
}

void work_handler(struct work_struct *work) {
	char code = scancode & 0x7F;
	char is_released = scancode & 0x80;
	if (is_released) {
		printk(KERN_INFO "Key '%s' pressed (code: %02x)\n", scancode2ASCII(code), code);
	}
}

irqreturn_t irq_handler(int irq, void *dev_id) {
	unsigned char status;

	// Прочитать состояние клавиатуры
	status = inb(0x64);
	scancode = inb(0x60); 

	queue_work(workqueue, work);
	return IRQ_HANDLED;
}

static int __init work_init(void) {
	workqueue = create_workqueue("my workqueue");

	work = vmalloc(sizeof(struct work_struct));
	INIT_WORK(work, work_handler);

	int flags = IRQF_SHARED;
	int ret = request_irq(IRQ_NUMBER, irq_handler, flags, "my interrupt (wq)", &tmp);
	if (ret) {
		printk (KERN_DEBUG "request irq failed\n");	
		return -1;
	}

	printk(KERN_DEBUG "module with works loaded!\n");
	return 0;
}

static void __exit work_exit(void) {
	destroy_workqueue(workqueue);
	vfree(work);
 	free_irq(IRQ_NUMBER, &tmp);
	printk (KERN_DEBUG "module with works unloaded!\n");
}

module_init(work_init);
module_exit(work_exit);
