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
#include <linux/string.h>


char * numToString(int num);

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
    char * str = NULL;

    seq_printf(m, "Process ID=%d Name=%s ", task->pid, task->comm);

    //increments to get number of children
    list_for_each(list, &task->children) {
        numberOfChildren++;
    }
    //print number of children to file anch first child info (or no children)
    if (numberOfChildren == 0) {
      seq_printf(m, "*No Children ");
    } else {
      seq_printf(m, "number_of_children=%d", numberOfChildren);
	numberOfChildren = 1; //reset for sake of numToString method
        child = list_first_entry(list, struct task_struct, sibling);
        str = numToString(numberOfChildren); //gets string placement value
        seq_printf(m, " %s_child_pid=%d %s_child_name=%s", str, child->pid, str, child->comm);
    }

    seq_printf(m, "\n");

    //now use this method recursively to print everthing else
    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        getMainProcess(child, m);
    }
}

/*
 * Turns integers into words indicating placement, from 1 = "first" to 999 = "nine_hundred_ninety_ninth".
 */
char * numToString(int num) {
	//the below arrays contain strings corresponding to a placement value.
	char * hundreds[9] = {"one_hundred", "two_hundred", "three_hundred", "four_hundred", "five_hundred", "six_hundred", "seven_hundred", "eight_hundred", "nine_hundred"};
	char * tens[9] = {"ten", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};
	char * singles[9] = {"first", "second", "third", "fourth", "fifth", "sixth", "seventh", "eighth", "ninth"};
	char * hSpecials[9] = {"one_hundredth", "two_hundredth", "three_hundredth", "four_hundredth", "five_hundredth", "six_hundredth", "seven_hundredth", "eight_hundredth", "nine_hundredth"};
	char * tSpecials[9] = {"tenth", "twentieth", "thirtieth", "fortieth", "fiftieth", "sixtieth", "seventieth", "eightieth", "ninetieth"};
	char * specials[9] = {"eleventh", "twelfth", "thirteenth", "fourteenth", "fifteeth", "sixteenth", "seventeenth", "eighteenth", "nineteenth"};

	static char retString[80];
	
	if (num/100 > 0){ //if num > 100
		if (num%100 == 0){ //if num is a multiple of 100
			strcpy(retString, hSpecials[(num/100)-1]);
		} else if (num%10 == 0) { //if num is some value n*100+m*10
			strcpy(retString, hundreds[(num/100) - 1]);
			strcat(retString, "_");
			strcat(retString, tSpecials[((num%100)/10) - 1]);
		} else if (num%100 > 10 && num%100 < 20){ //if n contains a teen value
			strcpy(retString, hundreds[(num/100) - 1]);
			strcat(retString, "_");
			strcat(retString, specials[((num%100)%10) - 1]);
		} else { //all other cases
			strcpy(retString, hundreds[(num/100) - 1]);
			strcat(retString, "_");
			strcat(retString, tens[((num%100)/10) - 1]);
			strcat(retString, "_");
			strcat(retString, singles[(num%10) - 1]);
		}
	} else if (num/10 > 0) { //num > 10 and num < 100
		if (num%10 == 0) { //if num is some value m*10
			strcpy(retString, tSpecials[(num/10) - 1]);
		} else if (num > 10 && num < 20){ //if n contains a teen value
			strcpy(retString, specials[(num%10) - 1]);
		} else {//all other cases
			strcpy(retString, tens[(num/10) - 1]);
			strcat(retString, "_");
			strcat(retString, singles[(num%10) - 1]);
		}
	} else { //num < 10
		strcpy(retString, singles[num - 1]);
	}
	retString[strlen(retString)] = '\0';
	return retString;
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
