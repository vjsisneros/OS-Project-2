/*
TCSS 422 Assignment 2
Portions of code taken from resources provided on assignment sheet
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>

/**
 * Performs a DFS on a given task's children.
 *
 * @void
 */
void DFS(struct task_struct *task)
{   
    struct task_struct *child;
    struct list_head *list;

    printk("name: %s, pid: [%d], state: %li\n", task->comm, task->pid, task->state);
    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        DFS(child);
    }
}

static int hello_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "Hello proc!\n");
 DFS(&init_task);
  
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

static int __init proc_report_init(void) {
  proc_create("proc_report", 0, NULL, &hello_proc_fops);
  //printk(KERN_INFO "PROCESS REPORTER\n");
  // DFS(&init_task);
  return 0;
}

static void __exit proc_report_exit(void) {
  remove_proc_entry("proc_report", NULL);
}





MODULE_LICENSE("GPL");
module_init(proc_report_init);
module_exit(proc_report_exit);