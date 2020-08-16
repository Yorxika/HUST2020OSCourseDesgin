#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>


#define MAXNUM 1024
#define MAJOR_NUM 300  //���豸�ţ�û�б�ʹ��  �鿴/proc/devices��֪

//ָ��license�汾
MODULE_LICENSE("GPL");


struct dev {
	struct cdev devm; //�ַ��豸
	/*struct semaphore sem;
	wait_queue_head_t outq;//�ȴ�����,ʵ����������
	int flag; //�������ѱ�־*/
	char buf[MAXNUM + 1]; //�ַ�������
	//char* rd, * wr, * end; //��,д,βָ��
}globalvar;

static struct class* my_class;

//�ļ��򿪺���
static int my_open(struct inode* inode, struct file* filp)
{
	printk("Character device is open.\n");
	//try_module_get(THIS_MODULE);//ģ�������һ
	filp->private_data = &globalvar;
	return 0;
}

//�ļ��ͷź���
static int my_release(struct inode* inode, struct file* filp)
{
	printk("Character device is released.\n");
	//module_put(THIS_MODULE); //ģ�������һ
	return 0;
}


//ģ��ж�غ���
static void mydev_exit(void)
{
	printk("Unload module: mydev.\n");
	device_destroy(my_class, MKDEV(MAJOR_NUM, 0));
	class_destroy(my_class);
	cdev_del(&globalvar.devm);  //ע���豸
	unregister_chrdev_region(MKDEV(MAJOR_NUM, 0), 1);  //�ͷ��豸��
}

//������
static ssize_t my_read(struct file* filp, char __user* buf, size_t len, loff_t* off) {
	printk(KERN_INFO "Character device start read.\n");
	int rtn = 0;
	size_t free_mem = MAXNUM - *off;  //ʣ����ÿռ�
	struct dev* dev = filp->private_data;
	if (len <= free_mem) {  //���ÿռ����
		if (copy_to_user(buf, dev->buf + *off, len)) {
			printk(KERN_ALERT "1.Copy to user buffer failed.\n");
			return -EFAULT;
		}
		*off += len;
		rtn = len;
	}
	else {
		//���ÿռ䲻��
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

//д����
static ssize_t my_write(struct file* filp, const char __user * buf, size_t len, loff_t* off) {
	printk(KERN_INFO "Start write.\n");
	int rtn = 0;
	size_t free_mem = MAXNUM - *off;  //ʣ����ÿռ�
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

//�ļ���λ����
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

//���ó�ʼ����ں���
static int __init mydev_init(void)
{
	//printk(KERN_DEBUG "hello %s!!!\n", name);
	printk(KERN_DEBUG "Load module: mydev\n");
	int rtn;
	dev_t devno = MKDEV(MAJOR_NUM, 0);  //ע���ַ��豸����
	rtn = register_chrdev_region(devno, 1, "mydev");  //��̬�����豸���
	if (rtn < 0) {
		printk(KERN_ALERT "Register character device error.");
		return rtn;
	}
	memset(globalvar.buf, '\0', MAXNUM + 1);
	//printk(KERN_DEBUG "buf:%s|\n", globalvar.buf);
	cdev_init(&(globalvar.devm), &my_fops); //ע���ַ��豸�������豸�ź�file_operations�ṹ����а�
	globalvar.devm.owner = THIS_MODULE;  //ͨ��THIS_MODULE��������ģ���struct module�ṹ
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