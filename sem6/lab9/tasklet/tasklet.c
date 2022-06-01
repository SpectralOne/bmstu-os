#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Untilova");

#define KEYB_IRQ 1

char my_tasklet_data[] = "KEYBOARD IRQ";
struct tasklet_struct my_tasklet;

// state: TASKLET_STATE_SCHED (Scheduled) or TASKLET_STATE_RUN (Running) = 2.
// count – nonzero value if the tasklet - disabled; 0 - enabled.
// func –  This is the main function of the tasklet.
// data –  Data to be passed to the function “func“.
void my_tasklet_func(unsigned long data)
{
	// state - состояние тасклета
	// счетчик ссылок
	// data - аргумент ф-ии обработчика тасклета
  printk(KERN_DEBUG "+ Tasklet: state - %d, count - %d, data - %ld\n",
    my_tasklet.state, my_tasklet.count, my_tasklet.data);
}

// Обработчик прерывания
irqreturn_t irq_handler(int irq, void *dev)
{
  // Проверка, что произошло именно 1-е прерывание
  if(irq == KEYB_IRQ)
  {
    // Постановка тасклета в очередь на выполнение
    tasklet_schedule(&my_tasklet);
    printk(KERN_DEBUG "+ Interrupt - KEYBOARD!\n");

    return IRQ_HANDLED; // прерывание обработано
  }
  else
    return IRQ_NONE; // прерывание не обработано
}

// Инициализация модуля
static int __init my_module_init(void)
{
  printk(KERN_DEBUG "+ loaded!\n");

  // номер irq
  // указ-ль на наш обработчик прерывания
  // флаг разделение(совместное использование) линии IRQ с другими устройствами
  // имя устройства, связ. с прерыванием
  // идентификатор устройства, нужен для для разделения линии прер-я, отключения с линии прерваний с помощью free_irq
  // === регистрация обработчика прерывания ===
  int ret = request_irq(KEYB_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED,
				"keyboard", (void *)(irq_handler));

  if (ret != 0)
  {
    printk(KERN_ERR "+ KEYBOARD IRQ handler wasn't registered");
    return ret;
  }

  printk(KERN_DEBUG "+ KEYBOARD IRQ handler was registered successfully");

  // инициализирует структуру tasklet_struct в соответствие с данными, предоставленными пользователем.
  // my_tasklet_func - обработчик тасклета
  // знач-е 3его пар-ра - ф-я обработчик
  tasklet_init(&my_tasklet, my_tasklet_func, (void *)(irq_handler));
  return ret;
}

// Выход загружаемого модуля
static void __exit my_module_exit(void)
{
  // Освобождение линии irq от указанного обработчика
  // Данные по указателю dev (2ой пар-р) требуются для удаления только конкретного устройства. 
  free_irq(KEYB_IRQ, (void *)(irq_handler));

  // Удаление тасклета
  tasklet_disable(&my_tasklet);
  // ждет заверш-я тасклета и удаляет тасклет из очереди на выполнение
  tasklet_kill(&my_tasklet);
  printk(KERN_DEBUG "+ unloaded!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);