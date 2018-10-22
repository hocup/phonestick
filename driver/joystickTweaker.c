/* A simple module that creates a proc file
 * and tweaks the joystick a bit when the character 'a' is written to it
 * Tweaking stops when any other character is written
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <linux/input.h>

#define PROCFS_NAME "phonestick"

struct input_dev *tweaker_dev;
static struct proc_dir_entry *myProcFile;
// static int tweakingOn = 0;
int tweakState = 0;

/* The function that gets called repeatedly when tweaking */
void tweak(struct timer_list *unused){
  int ret;
  /* move in a small square 
  if(tweakState < 30){
    input_report_rel(tweaker_dev, REL_X, 5);
  } else if(tweakState < 60){
    input_report_rel(tweaker_dev, REL_Y, 5); 
  } else if(tweakState < 90){
    input_report_rel(tweaker_dev, REL_X, -5);
  } else {
    input_report_rel(tweaker_dev, REL_Y, -5);
  }
*/
  input_sync(tweaker_dev);

  /* Cycle back to this function to update joystick state */
  ret = mod_timer(&(tweaker_dev->timer), jiffies + HZ/50);
  if (ret) printk("Error in mod_timer\n");
}

/* The function called when the proc file is read */
ssize_t pf_read(struct file *file, char __user *user, size_t count, loff_t *data){
  printk(KERN_INFO "Procfile read (/proc/%s), nothing happens.\n", PROCFS_NAME);
  return 0;
}

/* The function called when the proc file is written */
ssize_t pf_write(struct file *file, const char *buffer, size_t count,
              loff_t *data){
  int testParse = 0;
  kstrtoint(buffer, 16, &testParse); 
  printk(KERN_INFO "Procfile write (/proc/%s) %d chars written %d (%s) \n", PROCFS_NAME, count, testParse, buffer);
  
  int val = 0;

  if(buffer[0] == 'x'){
    kstrtoint(buffer + 1, 16, &val);
    printk(KERN_INFO "Procfile recieved 'x' char %d\n", val); 
    input_report_abs(tweaker_dev, ABS_X, val);
    mod_timer(&(tweaker_dev->timer), jiffies + HZ/50);
  } else if(buffer[0] == 'y') {
    kstrtoint(buffer + 1, 16, &val);
    printk(KERN_INFO "Procfile recieved 'y' char %d\n", val); 
    input_report_abs(tweaker_dev, ABS_Y, val);
    mod_timer(&(tweaker_dev->timer), jiffies + HZ/50);
  }
  
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

  /* Two absolute axes */
  set_bit(EV_ABS, tweaker_dev->evbit);
  set_bit(ABS_X, tweaker_dev->absbit);
  set_bit(ABS_Y, tweaker_dev->absbit);

  input_set_abs_params(tweaker_dev, ABS_X, 0,65535,0,10);
  input_set_abs_params(tweaker_dev, ABS_Y, 0,65535,0,10);

  /* need a button too */
  set_bit(EV_KEY, tweaker_dev->evbit);
  set_bit(BTN_JOYSTICK, tweaker_dev->keybit);

  /* Register with input core */
  input_register_device(tweaker_dev);

  /* Set up a repeating timer */
  timer_setup(&(tweaker_dev->timer), tweak, 0);
  ret = mod_timer( &(tweaker_dev->timer), jiffies + HZ/10);
  if (ret) printk("Error in mod_timer\n");


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
