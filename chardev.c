#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h> /* for put_user */

/*
 * Prototypes - this would normally go in a .h file
 */

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 1024

/*
 * Global variables are declared as static, so are global within the file.
 */

static int eof_flag = 0;
static int Major;
static int device_counter = 0;
static char msg[BUF_LEN] = "Hello world!";

static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release,
  .owner = THIS_MODULE
};

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
  Major = register_chrdev(0, DEVICE_NAME, &fops);

  if (Major < 0) {
    printk(KERN_ALERT "Registering char device failed with %d\n", Major);
    return Major;
  }

  printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
  printk(KERN_INFO "the driver, create a dev file with\n");
  printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
  printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
  printk(KERN_INFO "the device file.\n");
  printk(KERN_INFO "Remove the device file and module when done.\n");

  return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
  /*
   * Unregister the device
   */
  unregister_chrdev(Major, DEVICE_NAME);
}

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *filp)
{
  if (device_counter)
    return -EBUSY;

  device_counter++;

  try_module_get(THIS_MODULE);

  return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *filp)
{
  device_counter--;

  eof_flag = 0;
  module_put(THIS_MODULE);

  return SUCCESS;
}

/*
 * Called when a process, which already opened the dev file, attempts to read
 * from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h   */
                           char __user *buffer,      /* buffer to fill with data */
                           size_t length,     /* length of the buffer     */
                           loff_t *offset)
{
  /*
   * Number of bytes actually written to the buffer
   */
  size_t i = 0;

  /* Set EOF flag to that we output the buffer only once */
  if(eof_flag) {
      return 0;
  }
  eof_flag++;

  while(i < length && i < BUF_LEN) {
      put_user(msg[i], &buffer[i]);
      i++;
  }

  return i;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/hello
 */
static ssize_t
device_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
  printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
  return -EINVAL;
}
