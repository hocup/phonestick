/* A simple module that creates a proc file
 * and tweaks the joystick a bit when the character 'a' is written to it
 * Tweaking stops when any other character is written
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/input.h>

#define PROCFS_NAME "phonestick"




struct input_dev *tweaker_dev;
static struct proc_dir_entry *myProcFile;
// static int tweakingOn = 0;
int tweakState = 0;

struct AxisDef {
  char key;
  int axis;
};

struct AxisDef axes[6];

/* The function called when the proc file is read */
ssize_t pf_read(struct file *file, char __user *user, size_t count, loff_t *data){
  printk(KERN_INFO "Procfile read (/proc/%s), nothing happens.\n", PROCFS_NAME);
  return 0;
}

/* The function called when the proc file is written */
ssize_t pf_write(struct file *file, const char *buffer, size_t count,
              loff_t *data){
  int testParse = 0;
  int ret;
  int val;

  ret = kstrtoint(buffer, 16, &testParse); 
  if(ret) printk(KERN_INFO "Issues Parsins %d\n", ret);
  
  printk(KERN_INFO "Procfile write (/proc/%s) %d chars written %d (%s) \n", PROCFS_NAME, count, testParse, buffer);

  int i;
  for(i = 0; i < 6; i++) {
    if(buffer[0] == axes[i].key) {
      ret = kstrtoint(buffer + 1, 16, &val);
      if(ret) printk(KERN_INFO "Issues parsing int from string %d\n", ret);
      input_report_abs(tweaker_dev, axes[i].axis, val);
    }
  }

  input_sync(tweaker_dev);
  
  return count;
}

/* This struct sets the operations of out file to the above functions */
static const struct file_operations proc_fops = {
  .owner = THIS_MODULE,
  .read = pf_read,
  .write = pf_write
};

int init_module(){
  int ret;

  axes[0].key = 'x';
  axes[0].axis = ABS_X;
  axes[1].key = 'y';
  axes[1].axis = ABS_Y;
  axes[2].key = 'z';
  axes[2].axis = ABS_Z;
  axes[3].key = 'r';
  axes[3].axis = ABS_RY;
  axes[4].key = 'p';
  axes[4].axis = ABS_RX;
  axes[5].key = 't';
  axes[5].axis = ABS_RZ;

  myProcFile = proc_create(PROCFS_NAME, 0666, NULL, &proc_fops);

  if(myProcFile == NULL){
    remove_proc_entry(PROCFS_NAME, NULL);
    printk(KERN_ALERT "Error: could not initialize /proc/%s\n", PROCFS_NAME);
    return -ENOMEM;
  }
  printk(KERN_INFO "/proc/%s created \n", PROCFS_NAME);
  /*  Input initialization */
  tweaker_dev = input_allocate_device();

  /* set up descriptive labels */
  tweaker_dev->name = "Phonestick";
  /* phys is a unique name on a running system */
  tweaker_dev->phys = "A/Fake/Path";
  tweaker_dev->id.bustype = BUS_HOST;
  tweaker_dev->id.vendor = 0x0001;
  tweaker_dev->id.product = 0x0001;
  tweaker_dev->id.version = 0x0100;

  /* set up absolute axes */
  set_bit(EV_ABS, tweaker_dev->evbit);
  int i;
  for(i = 0; i < 6; i++) {
    set_bit(axes[i].axis, tweaker_dev->evbit);
    input_set_abs_params(tweaker_dev, axes[i].axis, 0, 65535, 0, 10);
  }

  /* need a button too */
  set_bit(EV_KEY, tweaker_dev->evbit);
  set_bit(BTN_JOYSTICK, tweaker_dev->keybit);

  /* Register with input core */
  ret = input_register_device(tweaker_dev);
  if(ret) printk("Error registering input device\n");

  return 0;
}

void cleanup_module(){
  int ret;

  remove_proc_entry(PROCFS_NAME, NULL);
 
  ret = del_timer( &(tweaker_dev->timer) );
  if (ret) printk("The timer is still in use...\n");
  input_unregister_device(tweaker_dev);
  printk(KERN_INFO "/proc/%s removed\n", PROCFS_NAME);
}
