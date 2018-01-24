#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/types.h>

struct birthday
{
	int day;
	int month;
	int year;
	struct list_head list;
};

/* This function is called when the module is loaded. */
int project1_mod_init(void)
{
	printk(KERN_INFO "Loading Module\n");
	
	static LIST_HEAD(birthday_list);
	
	/*Create five "birthday" structs*/
	struct birthday *person1;
	struct birthday *person2;
	struct birthday *person3;
	struct birthday *person4;
	struct birthday *person5;
	
	person1 = kmalloc(sizeof(*person1), GFP_KERNEL);
	person1->day = 3;
	person1->month = 12;
	person1->year = 1935;
	INIT_LIST_HEAD(&person1->list);
	
	person2 = kmalloc(sizeof(*person2), GFP_KERNEL);
	person2->day = 22;
	person2->month = 11;
	person2->year = 1949;
	INIT_LIST_HEAD(&person2->list);
	
	person3 = kmalloc(sizeof(*person3), GFP_KERNEL);
	person3->day = 16;
	person3->month = 5;
	person3->year = 1988;
	INIT_LIST_HEAD(&person3->list);
	
	person4 = kmalloc(sizeof(*person4), GFP_KERNEL);
	person4->day = 26;
	person4->month = 4;
	person4->year = 1963;
	INIT_LIST_HEAD(&person4->list);
	
	person5 = kmalloc(sizeof(*person5), GFP_KERNEL);
	person5->day = 17;
	person5->month = 4;
	person5->year = 1996;
	INIT_LIST_HEAD(&person5->list);
	
	/*Add them to the end of linked list*/
	list_add_tail(&person1->list, &birthday_list);
	printk(KERN_INFO "Add element 1\n");
	list_add_tail(&person2->list, &person1->list);
	printk(KERN_INFO "Add element 2\n");
	list_add_tail(&person3->list, &person2->list);
	printk(KERN_INFO "Add element 3\n");
	list_add_tail(&person4->list, &person3->list);
	printk(KERN_INFO "Add element 4\n");
	list_add_tail(&person5->list, &person4->list);
	printk(KERN_INFO "Add element 5\n");
	
	struct birthday *itr;
	list_for_each_entry(itr, &birthday_list, list)
	{
		printk(KERN_INFO "Birthday: %d/%d/%d\n", itr->year, itr->month, itr->day);
	}
	
	return 0;
}

/* This function is called when the module is removed. */
void project1_mod_exit(void)
{
	printk(KERN_INFO "Removing Module\n");
	
	struct birthday *ptr, *next;
	static LIST_HEAD(birthday_list);
	
	printk(KERN_INFO "Delete element 1\n");
	printk(KERN_INFO "Delete element 2\n");
	printk(KERN_INFO "Delete element 3\n");
	printk(KERN_INFO "Delete element 4\n");
	printk(KERN_INFO "Delete element 5\n");
	
	//int i=1;
	list_for_each_entry_safe(ptr, next, &birthday_list, list)
	{
		//printk(KERN_INFO "Delete element %d\n", i);
		//i++;
		list_del(&ptr->list);
		kfree(ptr);
	}
}

/* Macros for registering module entry and exit points. */
module_init( project1_mod_init );
module_exit( project1_mod_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Project1_Write_a_Simple_Module");
MODULE_AUTHOR("0416047_Andy_Wang");

