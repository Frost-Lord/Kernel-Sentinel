#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/time.h>
#include "driver.h"
#include "suspicious.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Frost-Lord");
MODULE_DESCRIPTION("Network Traffic Analyzer");

static struct nf_hook_ops netfilter_ops;

unsigned int analyze_packet_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *ip_header;
    struct tcphdr *tcp_header;
    struct udphdr *udp_header;
    struct timespec64 ts;
    char *protocol;

    if (!skb)
        return NF_ACCEPT;

    ip_header = ip_hdr(skb);
    if (!ip_header)
        return NF_ACCEPT;

    uint32_t src_ip = ip_header->saddr;
    uint32_t dest_ip = ip_header->daddr;
    uint16_t src_port = 0;
    uint16_t dest_port = 0;

    if (ip_header->protocol == IPPROTO_TCP) {
        tcp_header = tcp_hdr(skb);
        src_port = ntohs(tcp_header->source);
        dest_port = ntohs(tcp_header->dest);
        protocol = "TCP";

        if (dest_port == 22) {
            printk(KERN_INFO "[KSentinel] SSH connection detected: src_ip=%pI4, dest_ip=%pI4, src_port=%u, dest_port=%u\n",
                   &src_ip, &dest_ip, src_port, dest_port);
        }

        if (dest_port == 80 || dest_port == 443) {
            printk(KERN_INFO "[KSentinel] HTTP/HTTPS connection detected: src_ip=%pI4, dest_ip=%pI4, src_port=%u, dest_port=%u\n",
                   &src_ip, &dest_ip, src_port, dest_port);
        }
    } else if (ip_header->protocol == IPPROTO_UDP) {
        udp_header = udp_hdr(skb);
        src_port = ntohs(udp_header->source);
        dest_port = ntohs(udp_header->dest);
        protocol = "UDP";
    } else {
        protocol = "OTHER";
    }

    ktime_get_real_ts64(&ts);

    printk(KERN_INFO "[KSentinel] Packet: Time=%lld.%09ld, Protocol=%s, src_ip=%pI4, dest_ip=%pI4, src_port=%u, dest_port=%u\n",
           (s64)ts.tv_sec, ts.tv_nsec, protocol, &src_ip, &dest_ip, src_port, dest_port);

    // Call the suspicious function
    suspicious(src_ip, dest_ip);

    return NF_ACCEPT;
}

void analyze_packet(void) {
    netfilter_ops.hook = analyze_packet_hook;
    netfilter_ops.pf = PF_INET;
    netfilter_ops.hooknum = NF_INET_PRE_ROUTING;
    netfilter_ops.priority = NF_IP_PRI_FIRST;

    nf_register_net_hook(&init_net, &netfilter_ops);
    init_suspicious();
    printk(KERN_INFO "[KSentinel] Network Traffic Analyzer initialized\n");
}

EXPORT_SYMBOL(analyze_packet);

static void __exit analyzer_exit(void) {
    nf_unregister_net_hook(&init_net, &netfilter_ops);
    exit_suspicious();
    printk(KERN_INFO "[KSentinel] Network Traffic Analyzer exited\n");
}

module_exit(analyzer_exit);
