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

/**
 * Performs a DFS on a given task's children.
 *
 * @void
 */
void DFS(struct task_struct *task, struct seq_file *m)
{
    struct task_struct *child;
    struct list_head *list;
    seq_printf(m, "name: %s, pid: [%d], state: %li\n", task->comm, task->pid, task->state);
    //printk("name: %s, pid: [%d], state: %li\n", task->comm, task->pid, task->state);
    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        DFS(child, m);
    }
}

static int hello_proc_show(struct seq_file *m, void *v) {

//  printk(KERN_INFO "%d", &init_task;

  seq_printf(m, "This is a test!\n");
  DFS(&init_task, m);
  seq_printf(m, "Hello proc!\n");
  return 0;
}

static int hello_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, hello_proc_show, NULL);
}

static const struct file_operations hello_proc_fops = {
  .owner = THIS_MODULE,
  .open = hello_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

static int __init hello_proc_init(void) {
  proc_create("hello_proc", 0, NULL, &hello_proc_fops);
  //DFS(&init_task);
  return 0;
}

static void __exit hello_proc_exit(void) {
  remove_proc_entry("hello_proc", NULL);
}

MODULE_LICENSE("GPL");
module_init(hello_proc_init);
module_exit(hello_proc_exit);
