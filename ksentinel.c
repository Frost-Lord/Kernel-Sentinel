#include <linux/module.h>
#include <linux/init.h>
#include "driver/driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Frost-Lord");
MODULE_DESCRIPTION("Network Traffic Analyzer");

static int __init ksentinel_init(void) {
    printk(KERN_INFO "[KSentinel] Loading KSentinel module...\n");
    analyze_packet();
    return 0;
}

static void __exit ksentinel_exit(void) {
    printk(KERN_INFO "[KSentinel] Unloading KSentinel module.\n");
}

module_init(ksentinel_init);
module_exit(ksentinel_exit);
