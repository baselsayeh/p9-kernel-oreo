/*
 * coderunstartup
 *
 * Support for running code on startup
 *
 * Copyright (C) 2017/2018 BaselSayeh.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sizes.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/memblock.h>
#include <linux/memblock.h>
#include <asm/io.h>

#include <asm/coderunstartup.h>

#define modulename "coderunstartup_module"
#define moduledevname modulename
#define devmajor 79
unsigned char *memarea;

#define memareasize CODERUNSTARTUP_SIZE

static ssize_t memo_read(struct file *f, char *buf, size_t count, loff_t *f_pos) {
	ssize_t bytestoread = count;
	ssize_t readbytes = 0;

	if ( count > memareasize)
		bytestoread = memareasize;

	if (memareasize <= (*f_pos) ) {
		return 0;
	}

	while (bytestoread > 0) {
		if (copy_to_user(buf + readbytes, memarea + (*f_pos), ((bytestoread > 512) ? 512 : bytestoread) ) != 0) {
			pr_err("error writing %d bytes to user!", ((bytestoread > 512) ? 512 : (int)bytestoread) );
			return -EFAULT;
		}

		(*f_pos) += ((bytestoread > 512) ? 512 : bytestoread);
		count -= ((bytestoread > 512) ? 512 : bytestoread);
		readbytes += ((bytestoread > 512) ? 512 : bytestoread);
		bytestoread -= ((bytestoread > 512) ? 512 : bytestoread);
	}

	return readbytes;
}

static ssize_t memo_write(struct file *f, const char *buf, size_t count, loff_t *f_pos) {
	ssize_t bytestowrite = count;
	ssize_t writtenbytes = 0;

	if ( count > memareasize)
		bytestowrite = memareasize;

	if (memareasize <= (*f_pos) ) {
		return 0;
	}

	while (bytestowrite > 0) {
		if (copy_from_user(memarea + (*f_pos), buf + writtenbytes, ((bytestowrite > 512) ? 512 : bytestowrite) ) != 0) {
			pr_err("error reading %d bytes from user!", ((bytestowrite > 512) ? 512 : (int)bytestowrite) );
			return -EFAULT;
		}

		(*f_pos) += ((bytestowrite > 512) ? 512 : bytestowrite);
		count -= ((bytestowrite > 512) ? 512 : bytestowrite);
		writtenbytes += ((bytestowrite > 512) ? 512 : bytestowrite);
		bytestowrite -= ((bytestowrite > 512) ? 512 : bytestowrite);
	}

	return writtenbytes;
}

static long memo_ioctl(struct file *f, unsigned int cmd, unsigned long arg) {
	unsigned int tmp = 0;
	pr_info("ioctl\n");

	switch (cmd) {
			case 0x8A2B0001:
				tmp = 0;
				for (; tmp<memareasize;tmp++)
					memarea[tmp] = 0x00;
				pr_info("reset to zero!\n");
				return 0;

		default:
			pr_info("invalid code %u\n", cmd);
			break;
	}

	return -EINVAL;
}

static struct file_operations mem_fops = {
	.owner = THIS_MODULE,
	.read = memo_read,
	.write = memo_write,
	.unlocked_ioctl = memo_ioctl,
};

static int m_init(void) {
	int result,err=0,i=0;

	pr_info(modulename ": init started\n");

	if( request_mem_region(CODERUNSTARTUP_ADDR, memareasize, modulename) == NULL ) {
		pr_err(modulename ": Cannot Reserve memory!\n");
		err = -ENOMEM;
		goto init_err;
	}

	memarea = ioremap(CODERUNSTARTUP_ADDR, memareasize);
	if (memarea == NULL) {
		pr_err(modulename ": Cannot Get memory!\n");
		err = -ENOMEM;
		goto init_err;
	}

	for (i=0; i<memareasize; i++)
		memarea[i] = 0x00;

	result = register_chrdev(devmajor, moduledevname, &mem_fops);
	if(result < 0) {
		pr_err(modulename ": Cannot obtain major number %d\n", devmajor);
		err = result;
		goto init_err;
	}

	pr_info(modulename ": init finished\n");
	return 0;

	init_err:
	unregister_chrdev(devmajor, moduledevname);
	iounmap(memarea);
	release_mem_region(CODERUNSTARTUP_ADDR, memareasize);

	return err;
}

static void m_exit(void) {
	pr_info(modulename ": exiting!\n");
	unregister_chrdev(devmajor, moduledevname);
	iounmap(memarea);
	release_mem_region(CODERUNSTARTUP_ADDR, memareasize);
	return;
}

module_init(m_init);
module_exit(m_exit);

MODULE_AUTHOR("Basel Sayeh <basel.sayeh@hotmail.com>");
MODULE_DESCRIPTION("Code run on startup support");
MODULE_LICENSE("GPL v2");
