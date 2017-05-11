/*
TCSS 422 project 2
Portions of code taken from provided examples
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>

static int unrunnable = 0;
static int runnable = 0;
static int stopped = 0;

/*
 * Incriments the number of unrunanble, runnable, and stopped processes.
 */
void getProcessStates(struct task_struct *task, struct seq_file *m) {
    struct task_struct *child;
    struct list_head *list;

    if (task->state < 0) {
      unrunnable++;
    } else if (task->state == 0) {
      runnable++;
    } else {
      stopped++;
    }
    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        getProcessStates(child, m);
    }
}

/*
 * Prints all the processes and their children to the proc file.
 */
void DFS(struct task_struct *task, struct seq_file *m) {
    struct task_struct *child;
    struct list_head *list;

    if (task->state < 0) {
      //seq_printf(m, "name: %s, pid: [%d], state: Unrunnable\n", task->comm, task->pid);

    } else if (task->state == 0) {
      //seq_printf(m, "name: %s, pid: [%d], state: Runnable\n", task->comm, task->pid);

    } else {
    //  seq_printf(m, "name: %s, pid: [%d], state: Stopped\n", task->comm, task->pid);

    }
    //printk("name: %s, pid: [%d], state: %li\n", task->comm, task->pid, task->state);
    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        DFS(child, m);
    }
}



/*
* Call the methods to print to the proc file
*/
static int hello_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "PROCESS REPORTER\n");
  getProcessStates(&init_task, m);
  seq_printf(m, "Unrunnable: %d\nRunnable: %d\nStopped: %d\n", unrunnable, runnable, stopped);
  return 0;
}
/*
* Open the proc file, go do the operations (proc_show)
*/
static int hello_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, hello_proc_show, NULL);
}
/*
* Structure for file operations of proc file
*/
static const struct file_operations hello_proc_fops = {
  .owner = THIS_MODULE,
  .open = hello_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

/*
* Where the proc file is created
*/
static int __init hello_proc_init(void) {
  proc_create("hello_proc", 0, NULL, &hello_proc_fops);
  return 0;
}
/*
* Where the proc file is ended
*/
static void __exit hello_proc_exit(void) {
  remove_proc_entry("hello_proc", NULL);
}

MODULE_LICENSE("GPL");
module_init(hello_proc_init);
module_exit(hello_proc_exit);
