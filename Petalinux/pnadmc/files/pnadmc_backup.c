/*
 * Xilinx AXI DMA Driver
 *
 * Authors: 
 *    Fabrizio Spada - fabrizio.spada@mail.polimi.it
 *    Gianluca Durelli - durelli@elet.polimi.it
 *    Politecnico di Milano
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <linux/io.h>
#include <linux/uaccess.h>

#define MM2S_DMACR	0x00
#define MM2S_DMASR	0x04
#define MM2S_SA		0x18
#define MM2S_LENGTH	0x28

#define S2MM_DMACR	0x30
#define S2MM_DMASR	0x34
#define S2MM_DA		0x48
#define S2MM_LENGTH	0x58

#define DRIVER_NAME "pnadmc_pdrv"
#define MODULE_NAME "pnadmc"

#define DMA_LENGTH	16384

static struct class *cl;	// Global variable for the device class 

struct pnadmc_device
{
	phys_addr_t bus_addr;
	unsigned long bus_size;
	char *virt_bus_addr;
	dev_t dev_num;
	const char *dev_name;
	struct cdev c_dev;
	char *pnadmc_addr;
	dma_addr_t pnadmc_handle;

	struct list_head dev_list;
};
LIST_HEAD( full_dev_list );

static struct pnadmc_device *get_elem_from_list_by_inode(struct inode *i)
{
	struct list_head *pos;
	struct pnadmc_device *obj_dev = NULL;
	list_for_each( pos, &full_dev_list ) {
		struct pnadmc_device *tmp;
    	tmp = list_entry( pos, struct pnadmc_device, dev_list );
    	if (tmp->dev_num == i->i_rdev)
    	{
    		obj_dev = tmp;
    		break;
    	}
  	}
  	return obj_dev;	
}
// static void dmaHalt(void){
// 	unsigned long mm2s_halt = ioread32(virt_bus_addr + MM2S_DMASR) & 0x1;
// 	unsigned long s2mm_halt = ioread32(virt_bus_addr + S2MM_DMASR) & 0x1;
// 	int count = 0;
// 	printk(KERN_INFO "Halting...\n");
// 	iowrite32(0, virt_bus_addr + S2MM_DMACR);
// 	iowrite32(0, virt_bus_addr + MM2S_DMACR);
// 	while( !mm2s_halt || !s2mm_halt){
// 		// mm2s_halt = ioread32(virt_bus_addr + MM2S_DMASR) & 0x1;
// 		mm2s_halt = virt_bus_addr[MM2S_DMASR] & 0x1;
// 		//s2mm_halt = ioread32(virt_bus_addr + S2MM_DMASR) & 0x1;
// 		s2mm_halt = virt_bus_addr[S2MM_DMASR] & 0x1;
// 		count++;
// 		if (count>100 )
// 		{
// 			break;
// 		}
// 	}

// 	printk(KERN_INFO "DMA Halted!\n");
// }

static int my_strcmp(const char *str1, const char *str2)
{
  int i;
  i = 0;
  while (str1[i] || str2[i])
    {
      if (str1[i] != str2[i])
        return (str1[i] - str2[i]);
      i++;
    }
  return (0);
}

static int dmaSynchMM2S(struct pnadmc_device *obj_dev)
{
	long long counter = 0;
	unsigned int counter2 = 0;
	
	printk(KERN_ERR "<%s> SYNC virtual address = 0x%x \n",
					 MODULE_NAME, obj_dev->virt_bus_addr);
	unsigned int mm2s_status = ioread32(obj_dev->virt_bus_addr + MM2S_DMASR);
	printk(KERN_ERR "<%s> bus address = 0x%x \n",
					 MODULE_NAME, obj_dev->bus_addr);
	while(!(mm2s_status & 1<<12) || !(mm2s_status & 1<<1) )
	{
		//printk(KERN_ERR "<%s> SYNC_MM2S #3  \n", MODULE_NAME);
		mm2s_status = ioread32(obj_dev->virt_bus_addr + MM2S_DMASR);
		//printk(KERN_ERR "<%s> SYNC_MM2S #4  \n", MODULE_NAME);
		counter++;
		if( counter>200000 )
		{
			printk(KERN_ERR "<%s> MM2S_Status = 0x%x virt_bus_addr = 0x%x \n",
			                MODULE_NAME, mm2s_status, obj_dev->virt_bus_addr);
			counter2++;
			if( counter2>20 )
			{
				counter2 = 0;
				return 1;
			}
			counter = 0;
			//return 1;
		}
	}
	
	printk(KERN_ERR "<%s> dmaSynchS2MM() completed \n");
	
	return 0;
}

static int dmaSynchS2MM(struct pnadmc_device *obj_dev)
{
	int i;
	unsigned int s2mm_status = ioread32(obj_dev->virt_bus_addr + S2MM_DMASR);
	long long counter = 0;
	unsigned int counter2 = 0;
	
	printk(KERN_ERR "<%s> S2MM_Status = 0x%x virt_bus_addr = 0x%x \n",
	                 MODULE_NAME, s2mm_status, obj_dev->virt_bus_addr);

	//while(!(s2mm_status & 1<<1))
	while(!(s2mm_status & 1<<12) || !(s2mm_status & 1<<1))
	{
		//printk(KERN_ERR "<%s> S2MM_Status = 0x%x \n", MODULE_NAME, s2mm_status);
		//printk(KERN_ERR "<%s> Sync_S2MM #2 \n", MODULE_NAME);
		s2mm_status = ioread32(obj_dev->virt_bus_addr + S2MM_DMASR);
		counter++;
		if( counter>200000 )
		{
			printk(KERN_ERR "<%s> S2MM_Status = 0x%x virt_bus_addr = 0x%x \n",
			                MODULE_NAME, s2mm_status, obj_dev->virt_bus_addr);
			counter2++;
			if( counter2>20 )
			{
				counter2 = 0;
				return 1;
			}
			counter = 0;
			//return 1;
		}
	}
	
	printk(KERN_ERR "<%s> dmaSynchS2MM() completed \n");
	
	return 0;
}

static int pnadmc_open(struct inode *i, struct file *f)
{
	/* printk(KERN_INFO "<%s> file: open()\n", MODULE_NAME); */
	struct pnadmc_device *obj_dev = get_elem_from_list_by_inode(i);
	if (!request_mem_region(obj_dev->bus_addr, obj_dev->bus_size, MODULE_NAME))
	{
		return -1;
	}	
	obj_dev->virt_bus_addr = (char *) ioremap_nocache(obj_dev->bus_addr, obj_dev->bus_size);
	return 0;
}

static int pnadmc_close(struct inode *i, struct file *f)
{
	/* printk(KERN_INFO "<%s> file: close()\n", MODULE_NAME); */
	struct pnadmc_device *obj_dev = get_elem_from_list_by_inode(i);
	iounmap(obj_dev->virt_bus_addr);
	release_mem_region(obj_dev->bus_addr, obj_dev->bus_size);
	return 0;
}

static ssize_t pnadmc_read(struct file *f, char __user * buf, size_t
			 len, loff_t * off)
{
	printk(KERN_ERR "<%s> file #00 READ \n", MODULE_NAME);
	/* printk(KERN_INFO "<%s> file: read()\n", MODULE_NAME); */
	struct pnadmc_device *obj_dev;
	if (len > DMA_LENGTH)
	{
		printk(KERN_ERR "<%s> file read len = 0\n", MODULE_NAME);
		return 0;
	}
	//printk(KERN_ERR "<%s> file #11 READ \n", MODULE_NAME);
	obj_dev = get_elem_from_list_by_inode(f->f_inode);
    //printk(KERN_ERR "<%s> file #22 READ \n", MODULE_NAME);
	iowrite32(1, obj_dev->virt_bus_addr + S2MM_DMACR);
	//printk(KERN_ERR "<%s> file #33 READ \n", MODULE_NAME);
	iowrite32(obj_dev->pnadmc_handle, obj_dev->virt_bus_addr + S2MM_DA);
	//printk(KERN_ERR "<%s> file #44 READ \n", MODULE_NAME);
	iowrite32(len, obj_dev->virt_bus_addr + S2MM_LENGTH);
	printk(KERN_ERR "<%s> file #55 READ \n", MODULE_NAME);
	//dmaSynchS2MM(obj_dev);
	if(dmaSynchS2MM(obj_dev)==0)
	{
		printk(KERN_ERR "<%s> file #66 READ \n", MODULE_NAME);
		copy_to_user(buf, obj_dev->pnadmc_addr, len);
	}
	printk(KERN_ERR "<%s> file #77 READ \n", MODULE_NAME);
//	memcpy(buf, obj_dev->pnadmc_addr, len);
	return len;
}

static ssize_t pnadmc_write(struct file *f, const char __user * buf,
			  size_t len, loff_t * off)
{
	/* printk(KERN_INFO "<%s> file: write()\n", MODULE_NAME); */

	struct pnadmc_device *obj_dev;
	if (len > DMA_LENGTH)
	{
		printk(KERN_ERR "<%s> file write len = 0\n", MODULE_NAME);
		return 0;
	}
	printk(KERN_ERR "<%s> file #0: write()\n", MODULE_NAME);
	obj_dev = get_elem_from_list_by_inode(f->f_inode);
	printk(KERN_ERR "<%s> file #1: write()\n", MODULE_NAME);
	printk(KERN_ERR "file PNADMC 0x%x \n" ,obj_dev->pnadmc_addr);
	printk(KERN_ERR "LEN=%d \n" ,len);
	printk(KERN_ERR "Beffer=0x%x \n" ,buf);
//	printk(KERN_ERR "Buffer[0] = %c \n" ,buf[0]);
	copy_from_user(obj_dev->pnadmc_addr, buf , len);
	printk(KERN_ERR "<%s> file #2: write()\n", MODULE_NAME);

	// printk(KERN_INFO "%X\n", ioread32(virt_bus_addr + MM2S_DMASR));
	// printk(KERN_INFO "%X\n", ioread32(virt_bus_addr + MM2S_DMACR));
	// printk(KERN_INFO "%X\n", ioread32(virt_bus_addr + S2MM_DMASR));
	// printk(KERN_INFO "%X\n", ioread32(virt_bus_addr + S2MM_DMACR));

	iowrite32(1, obj_dev->virt_bus_addr + MM2S_DMACR);
	printk(KERN_ERR "<%s> file #3: write()\n", MODULE_NAME);
	iowrite32(obj_dev->pnadmc_handle, obj_dev->virt_bus_addr + MM2S_SA);
	printk(KERN_ERR "<%s> file #4: write()\n", MODULE_NAME);
	iowrite32(len, obj_dev->virt_bus_addr + MM2S_LENGTH);
	printk(KERN_ERR "<%s> file #5: write()\n", MODULE_NAME);

	dmaSynchMM2S(obj_dev);

	// printk(KERN_INFO "%X\n", ioread32(virt_bus_addr + MM2S_DMASR));
	// printk(KERN_INFO "%X\n", ioread32(virt_bus_addr + MM2S_DMACR));
	// printk(KERN_INFO "%X\n", ioread32(virt_bus_addr + S2MM_DMASR));
	// printk(KERN_INFO "%X\n", ioread32(virt_bus_addr + S2MM_DMACR));	
	// printk(KERN_INFO "%X\n", bus_addr);
	// printk(KERN_INFO "%lu\n", bus_size);

	return len;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = pnadmc_open,
	.release = pnadmc_close,
	.read = pnadmc_read,
	.write = pnadmc_write,
	/* .mmap = pnadmc_mmap, */
	/* .unlocked_ioctl = pnadmc_ioctl, */
};

static int pnadmc_pdrv_probe(struct platform_device *pdev)
{
	/* device constructor */
	struct pnadmc_device *obj_dev = (struct pnadmc_device *)
            kmalloc( sizeof(struct pnadmc_device), GFP_KERNEL );
    obj_dev->bus_addr = pdev->resource[0].start;
    obj_dev->bus_size = pdev->resource[0].end - pdev->resource[0].start + 1;
	obj_dev->dev_name = pdev->name + 9;
	
	printk(KERN_INFO "<%s> init: registered\n", obj_dev->dev_name);
	if (alloc_chrdev_region(&(obj_dev->dev_num), 0, 1, obj_dev->dev_name) < 0) {
		return -1;
	}
	if (cl == NULL && (cl = class_create(THIS_MODULE, "chardrv")) == NULL) {
		unregister_chrdev_region(obj_dev->dev_num, 1);
		return -1;
	}
	if (device_create(cl, NULL, obj_dev->dev_num, NULL, obj_dev->dev_name) == NULL) {
		class_destroy(cl);
		unregister_chrdev_region(obj_dev->dev_num, 1);
		return -1;
	}
	cdev_init(&(obj_dev->c_dev), &fops);
	if (cdev_add(&(obj_dev->c_dev), obj_dev->dev_num, 1) == -1) {
		device_destroy(cl, obj_dev->dev_num);
		class_destroy(cl);
		unregister_chrdev_region(obj_dev->dev_num, 1);
		return -1;
	}

	printk(KERN_INFO "DMA_LENGTH = %u \n", DMA_LENGTH);
	/* allocate mmap area */
	obj_dev->pnadmc_addr =
	    dma_zalloc_coherent(NULL, DMA_LENGTH, &(obj_dev->pnadmc_handle), GFP_KERNEL);
	printk(KERN_ERR " Debug PNADMC 0x%x \n", obj_dev->pnadmc_addr);
	list_add( &obj_dev->dev_list, &full_dev_list );
	return 0;
}

static int pnadmc_pdrv_remove(struct platform_device *pdev)
{
	/* device destructor */
	struct list_head *pos, *q;
	list_for_each_safe( pos, q, &full_dev_list ) {
		struct pnadmc_device *obj_dev;
    	obj_dev = list_entry( pos, struct pnadmc_device, dev_list );
    	if (!my_strcmp(obj_dev->dev_name, pdev->name + 9))
    	{
    		list_del( pos );
    		cdev_del(&(obj_dev->c_dev));
    		device_destroy(cl, obj_dev->dev_num);
    		unregister_chrdev_region(obj_dev->dev_num, 1);
    		/* free mmap area */
			if (obj_dev->pnadmc_addr) {
				dma_free_coherent(NULL, DMA_LENGTH, obj_dev->pnadmc_addr, obj_dev->pnadmc_handle);
			}
    		kfree(obj_dev);
    		break;
    	}
  	}
  	if (list_empty(&full_dev_list))
  	{
  		class_destroy(cl);
  	}
	printk(KERN_INFO "<%s> exit: unregistered\n", MODULE_NAME);
	return 0;
}

static int pnadmc_pdrv_runtime_nop(struct device *dev)
{
	/* Runtime PM callback shared between ->runtime_suspend()
	 * and ->runtime_resume(). Simply returns success.
	 *
	 * In this driver pm_runtime_get_sync() and pm_runtime_put_sync()
	 * are used at open() and release() time. This allows the
	 * Runtime PM code to turn off power to the device while the
	 * device is unused, ie before open() and after release().
	 *
	 * This Runtime PM callback does not need to save or restore
	 * any registers since user space is responsbile for hardware
	 * register reinitialization after open().
	 */
	return 0;
}

static const struct dev_pm_ops pnadmc_pdrv_dev_pm_ops = {
	.runtime_suspend = pnadmc_pdrv_runtime_nop,
	.runtime_resume = pnadmc_pdrv_runtime_nop,
};

static struct of_device_id pnadmc_of_match[] = {
	{ .compatible = "xlnx,axi-dma-1.00.a", },
	{ /* This is filled with module_parm */ },
	{ /* Sentinel */ },
};
MODULE_DEVICE_TABLE(of, pnadmc_of_match);
module_param_string(of_id, pnadmc_of_match[1].compatible, 128, 0);
MODULE_PARM_DESC(of_id, "Openfirmware id of the device to be handled by uio");

static struct platform_driver pnadmc_pdrv = {
	.probe = pnadmc_pdrv_probe,
	.remove = pnadmc_pdrv_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.pm = &pnadmc_pdrv_dev_pm_ops,
		.of_match_table = of_match_ptr(pnadmc_of_match),
	},
};

// in case of no init and exit function
//module_platform_driver(pnadmc_pdrv);

MODULE_AUTHOR("Fabrizio Spada, Gianluca Durelli");
MODULE_DESCRIPTION("AXI DMA driver");
MODULE_LICENSE("GPL v2");

static int __init pnadmc_pdrv_init(void)
{
	printk("<1>Hello Mr.Binaee.\n");

	return platform_driver_register(&pnadmc_pdrv);
}


static void __exit pnadmc_pdrv_exit(void)
{
	platform_driver_unregister(&pnadmc_pdrv);
	printk(KERN_ALERT "Goodbye Mr.Bright.\n");
}

module_init(pnadmc_pdrv_init);
module_exit(pnadmc_pdrv_exit);


