#include <linux/hashtable.h>
#include <linux/slab.h>
#include <linux/ip.h>
#include <linux/timer.h>
#include "suspicious.h"

int SusCap = 1000000;

DEFINE_HASHTABLE(ip_table, 8);

struct ip_entry {
    uint32_t ip;
    uint32_t count;
    struct hlist_node hnode;
};

struct timer_list clear_timer;

static void add_ip_to_table(uint32_t ip);
static void clear_ip_table(struct timer_list *t);

void suspicious(uint32_t src_ip, uint32_t dest_ip) {
    add_ip_to_table(src_ip);
    add_ip_to_table(dest_ip);

    if (src_ip == dest_ip) return;
    if (src_ip == 0 || dest_ip == 0) return;

    if (src_ip >= SusCap || dest_ip >= SusCap) {
        printk(KERN_INFO "[KSentinel] Suspicious connection detected: src_ip=%pI4, dest_ip=%pI4\n",
               &src_ip, &dest_ip);
        
    }
    return;
}

static void add_ip_to_table(uint32_t ip) {
    struct ip_entry *entry;
    struct ip_entry *tmp;
    bool found = false;

    hash_for_each_possible(ip_table, tmp, hnode, ip) {
        if (tmp->ip == ip) {
            tmp->count++;
            found = true;
            break;
        }
    }

    if (!found) {
        entry = kmalloc(sizeof(*entry), GFP_KERNEL);
        if (!entry)
            return;
        entry->ip = ip;
        entry->count = 1;
        hash_add(ip_table, &entry->hnode, ip);
    }
}

static void clear_ip_table(struct timer_list *t) {
    struct ip_entry *tmp;
    struct hlist_node *tmp_node;
    int bkt;

    hash_for_each_safe(ip_table, bkt, tmp_node, tmp, hnode) {
        hash_del(&tmp->hnode);
        kfree(tmp);
    }

    mod_timer(&clear_timer, jiffies + msecs_to_jiffies(3600000)); // 1 hour
}

void init_suspicious(void) {
    timer_setup(&clear_timer, clear_ip_table, 0);
    mod_timer(&clear_timer, jiffies + msecs_to_jiffies(3600000)); // 1 hour
}

void exit_suspicious(void) {
    del_timer_sync(&clear_timer);
    clear_ip_table(&clear_timer);
}

MODULE_LICENSE("GPL");
EXPORT_SYMBOL(suspicious);
EXPORT_SYMBOL(init_suspicious);
EXPORT_SYMBOL(exit_suspicious);
