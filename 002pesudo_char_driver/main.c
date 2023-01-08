#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/err.h>

#define DEV_MEM_SIZE 512
char device_buffer[DEV_MEM_SIZE];

// Device number
dev_t device_number;

// cdev variable
struct cdev pcd_cdev;


loff_t pcd_lseek (struct file *filp, loff_t off, int whence)
{
	loff_t temp;

	printk("lseek requested\n");
	switch(whence)
	{
		case SEEK_SET: 
			if((off > DEV_MEM_SIZE) || (off < 0))
				return -EINVAL;
			filp->f_pos = off;
			break;
		case SEEK_CUR:
			temp = filp->f_pos + off;
			if((temp > DEV_MEM_SIZE) || (temp < 0))
				return -EINVAL;
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = DEV_MEM_SIZE + off;

			if((temp > DEV_MEM_SIZE) || (temp < 0))
                                return -EINVAL;

			filp->f_pos = temp;
			break;
		default : 
			return -EINVAL;

	
	}
	return filp->f_pos;
}
ssize_t pcd_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	printk("%d bytes read requested\n",(int) count);

	// Adjust count
	if((*f_pos+ count) > DEV_MEM_SIZE)
	{
		count = DEV_MEM_SIZE - *f_pos;
	}

	// copy to user
	if(copy_to_user(buff, &device_buffer[*f_pos], count))
	{
		return -EFAULT;
	}

	/* update file pos*/
	*f_pos += count;

	printk("read successfull\n");
	return count;
}
ssize_t pcd_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{

	printk("%d bytes write requested\n",(int)count);
	
	// adjust count 
	if((*f_pos + count) > DEV_MEM_SIZE)
	{
		count = DEV_MEM_SIZE - *f_pos;
	}

	if(!count)
	{
		return -ENOMEM;
	}

	// copy from user
	if(copy_from_user(&device_buffer[*f_pos],buff,count))
	{
		return -EFAULT;
	}

	/* update file pos*/
	*f_pos += count;

	printk("write successfull\n");
	return count;
}
int pcd_open (struct inode *inode, struct file *filp)
{

	printk("open requested\n");
	return 0;
}	
int pcd_release (struct inode *inode, struct file *filp)
{

	printk("release requested\n");
	return 0;
}


// File operation structure 
struct file_operations pcd_fops={
.open = pcd_open,
.release = pcd_release,
.read = pcd_read,
.write = pcd_write,
.llseek = pcd_lseek,
.owner = THIS_MODULE	 	
};

//
struct class *class_pcd;

//
struct device* device_pcd;
static int __init pcd_driver_init(void)
{
	int ret;
	// allocate device number dynamically
	ret = alloc_chrdev_region(&device_number,0,1,"pesudo_char_device");
	if(ret <0)
	{
		goto out;
	}
	printk("%s device number <major>:<minor> = %d:%d\n", __func__,MAJOR(device_number),MINOR(device_number));

	// cdev init
	cdev_init(&pcd_cdev, &pcd_fops);

	// cdev add (Register device(cdev structure) with VFS)
	pcd_cdev.owner = THIS_MODULE;

	ret = cdev_add(&pcd_cdev, device_number, 1);
	if(ret < 0)
	{
		goto unreg_chrdev;
	}	
	
	// create device class under /sys/class
	class_pcd = class_create(THIS_MODULE, "pcd_class");
	if(IS_ERR(class_pcd))
	{
		printk("class creation fail\n");
		ret = PTR_ERR(class_pcd);
		goto cdev_del;
	}
	
	// Device file creation
	device_pcd = device_create(class_pcd,NULL,device_number,NULL,"pcd");
	if(IS_ERR(device_pcd))
	{
		printk("device creation fail\n");
		ret = PTR_ERR(device_pcd);
		goto class_del; 
	}

	printk("Module inti done\n");
	return 0;

class_del:
	 class_destroy(class_pcd);
cdev_del:
	cdev_del(&pcd_cdev);

unreg_chrdev:
	unregister_chrdev_region(device_number,1);
out:
	return ret;
}

static void __exit pcd_driver_exit(void)
{
	device_destroy(class_pcd,device_number);
	class_destroy(class_pcd);

	cdev_del(&pcd_cdev);

	unregister_chrdev_region(device_number,1);

	printk("Module unloaded\n");

}

module_init(pcd_driver_init);
module_exit(pcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sachin patel");


