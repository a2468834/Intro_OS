#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/types.h>

/* Implementation of DFS iteration function */
void DFS_Iteration(struct task_struct *task)
{
	struct list_head *list;
	struct task_struct *next_task;
	
	list_for_each(list, &task->children)
	{
		next_task = list_entry(list, struct task_struct, sibling);
		
		printk(KERN_INFO "%s (%d) \n", next_task->comm, next_task->pid);
		
		DFS_Iteration(next_task);
	}
}


/* This function is called when the module is loaded. */
int project2_mod_init(void)
{
	struct task_struct *task;
	
	printk(KERN_INFO "Loading Module...\n");
	
	/* Iterating over tasks linearly */
	/* First, point on one task. */
	/* Second, point to next task. */
	printk(KERN_INFO "LINEAR\n");
	printk(KERN_INFO "Display_Form: PName (PID) \n");
	for_each_process(task)
	{
		printk(KERN_INFO "%s (%d) \n", task->comm, task->pid);
	}
	
	/* Iterating over tasks with DFS method */
	/* First, point on one task. */
	/* Second, point to its child. */
	printk(KERN_INFO "DFS\n");
	printk(KERN_INFO "Display_Form: PName (PID) \n");
	DFS_Iteration(&init_task);
	
	return 0;
}

/* This function is called when the module is removed. */
void project2_mod_exit(void)
{
	printk(KERN_INFO "Removing Module...\n");
}

/* Macros for registering module entry and exit points. */
module_init( project2_mod_init );
module_exit( project2_mod_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Project2_Iterate_over_Tasks");
MODULE_AUTHOR("0416047_Andy_Wang");
