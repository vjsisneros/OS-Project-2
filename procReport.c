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
void getMainProcess(struct task_struct *task, struct seq_file *m) {
    struct task_struct *child;
    struct list_head *list;
    int numberOfChildren = 0;

    seq_printf(m, "Process ID=%d Name =%s ", task->pid, task->comm);

    //printk("name: %s, pid: [%d], state: %li\n", task->comm, task->pid, task->state);

    //increments to get number of children
    list_for_each(list, &task->children) {
        numberOfChildren++;
    }
    //print number of children to file (or no children)
    if (numberOfChildren == 0) {
      seq_printf(m, "*No Children ");
    } else {
      seq_printf(m, "number_of_children=%d ", numberOfChildren);
    }

    //now print out the children information
    numberOfChildren = 0; //reset
    list_for_each(list, &task->children) {
      numberOfChildren++;
        child = list_entry(list, struct task_struct, sibling);
        seq_printf(m, "%d_child_pid=%d %d_child_name=%s", numberOfChildren, child->pid, numberOfChildren, child->comm);
        //getChildProcess(child, m);
    }

    seq_printf(m, "\n");

    //now use this method recursively to print everthing else
    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        getMainProcess(child, m);
    }
}

/*
* Call the methods to print to the proc file
*/
static int hello_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "PROCESS REPORTER\n");
  getProcessStates(&init_task, m);
  seq_printf(m, "Unrunnable: %d\nRunnable: %d\nStopped: %d\n", unrunnable, runnable, stopped);
  getMainProcess(&init_task, m);
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
  proc_create("proc_report", 0, NULL, &hello_proc_fops);
  return 0;
}
/*
* Where the proc file is ended
*/
static void __exit hello_proc_exit(void) {
  remove_proc_entry("proc_report", NULL);
}

MODULE_LICENSE("GPL");
module_init(hello_proc_init);
module_exit(hello_proc_exit);
