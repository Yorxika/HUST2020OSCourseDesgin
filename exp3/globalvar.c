#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>


#define MAXNUM 1024
#define MAJOR_NUM 300  //主设备号，没有被使用  查看/proc/devices可知

//指定license版本
MODULE_LICENSE("GPL");


struct dev {
	struct cdev devm; //字符设备
	/*struct semaphore sem;
	wait_queue_head_t outq;//等待队列,实现阻塞操作
	int flag; //阻塞唤醒标志*/
	char buf[MAXNUM + 1]; //字符缓冲区
	//char* rd, * wr, * end; //读,写,尾指针
}globalvar;

static struct class* my_class;

//文件打开函数
static int my_open(struct inode* inode, struct file* filp)
{
	printk("Character device is open.\n");
	//try_module_get(THIS_MODULE);//模块计数加一
	filp->private_data = &globalvar;
	return 0;
}

//文件释放函数
static int my_release(struct inode* inode, struct file* filp)
{
	printk("Character device is released.\n");
	//module_put(THIS_MODULE); //模块计数减一
	return 0;
}


//模块卸载函数
static void mydev_exit(void)
{
	printk("Unload module: mydev.\n");
	device_destroy(my_class, MKDEV(MAJOR_NUM, 0));
	class_destroy(my_class);
	cdev_del(&globalvar.devm);  //注销设备
	unregister_chrdev_region(MKDEV(MAJOR_NUM, 0), 1);  //释放设备号
}

//读函数
static ssize_t my_read(struct file* filp, char __user* buf, size_t len, loff_t* off) {
	printk(KERN_INFO "Character device start read.\n");
	int rtn = 0;
	size_t free_mem = MAXNUM - *off;  //剩余可用空间
	struct dev* dev = filp->private_data;
	if (len <= free_mem) {  //可用空间充足
		if (copy_to_user(buf, dev->buf + *off, len)) {
			printk(KERN_ALERT "1.Copy to user buffer failed.\n");
			return -EFAULT;
		}
		*off += len;
		rtn = len;
	}
	else {
		//可用空间不足
		if (copy_to_user(buf, dev->buf + *off, free_mem)) {
			printk(KERN_ALERT"2.Copy to user buffer failed.\n");
			return -EFAULT;
		}
		*off += free_mem;
		rtn = free_mem;
	}
	//printk("the read buffer is %s\n", globalvar.buf);
	printk(KERN_INFO "Character device has read %d bytes.\n", rtn);
	return rtn;
}

//写函数
static ssize_t my_write(struct file* filp, const char __user * buf, size_t len, loff_t* off) {
	printk(KERN_INFO "Start write.\n");
	int rtn = 0;
	size_t free_mem = MAXNUM - *off;  //剩余可用空间
	struct dev* dev = filp->private_data;
	//memset(dev->buf + *off, , free_mem);
	//printk(KERN_DEBUG "len: %d buf:%send.\n",strlen(dev->buf), dev->buf);
	if (len <= free_mem) {
		if (copy_from_user(dev->buf + *off, buf, len)) {
			printk(KERN_ALERT "1.Copy frome user buffer failed.\n");
			return -EFAULT;
		}
		*off += len;
		rtn = len;
	}
	else {
		if (copy_from_user(dev->buf + *off, buf, free_mem)) {
			printk(KERN_ALERT"2.Copy frome user buffer failed.\n");
			return -EFAULT;
		}
		*off += free_mem;
		rtn = free_mem;
	}
	printk("Character device has written %d bytes.\n", rtn);
	return rtn;
}

//文件定位函数
static loff_t my_llseek(struct file* filp, loff_t off, int whence)
{
	printk(KERN_INFO "Character device start lseek.\n");
	loff_t rtn = 0;
	switch (whence) {
	case SEEK_SET:
		rtn = off;
		break;
	case SEEK_CUR:
		rtn = filp->f_pos + off;
		break;
	case SEEK_END:
		rtn = MAXNUM + off;
		break;
	default:
		return -EINVAL;
	}
	if (rtn < 0 || rtn > MAXNUM)
		return -EINVAL;
	printk("Character device set offset at %d.\n", rtn);
	filp->f_pos = rtn;
	return rtn;
}

struct file_operations my_fops = {
	.open = my_open,
	.read = my_read,
	.write = my_write,
	.release = my_release,
	.llseek = my_llseek,
};

//设置初始化入口函数
static int __init mydev_init(void)
{
	//printk(KERN_DEBUG "hello %s!!!\n", name);
	printk(KERN_DEBUG "Load module: mydev\n");
	int rtn;
	dev_t devno = MKDEV(MAJOR_NUM, 0);  //注册字符设备驱动
	rtn = register_chrdev_region(devno, 1, "mydev");  //静态申请设备编号
	if (rtn < 0) {
		printk(KERN_ALERT "Register character device error.");
		return rtn;
	}
	memset(globalvar.buf, '\0', MAXNUM + 1);
	//printk(KERN_DEBUG "buf:%s|\n", globalvar.buf);
	cdev_init(&(globalvar.devm), &my_fops); //注册字符设备驱动，设备号和file_operations结构体进行绑定
	globalvar.devm.owner = THIS_MODULE;  //通过THIS_MODULE宏来引用模块的struct module结构
	rtn = cdev_add(&(globalvar.devm), devno, 1);
	if (rtn)
		printk(KERN_ALERT "Error %d adding mydev device.\n", rtn);
	else {
		printk(KERN_INFO "Character device register success.\n");
	}
	my_class = class_create(THIS_MODULE, "mydev");
	if (IS_ERR(my_class)) {
		rtn = PTR_ERR(my_class);
		printk(KERN_ALERT "Creat class for device file failed.\n");
		unregister_chrdev_region(devno, 1);
		return rtn;
	}
	device_create(my_class, NULL, devno, NULL, "mydev");
	return 0;
}

module_init(mydev_init);
module_exit(mydev_exit);