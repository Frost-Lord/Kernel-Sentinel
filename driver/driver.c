#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include "driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Frost-Lord");
MODULE_DESCRIPTION("Network Traffic Analyzer");

static struct nf_hook_ops netfilter_ops;

unsigned int analyze_packet_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *ip_header;
    struct tcphdr *tcp_header;
    struct udphdr *udp_header;

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
    } else if (ip_header->protocol == IPPROTO_UDP) {
        udp_header = udp_hdr(skb);
        src_port = ntohs(udp_header->source);
        dest_port = ntohs(udp_header->dest);
    }

    // float features[] = {src_ip, dest_ip, src_port, dest_port};
    // int result = run_ml_model(features);

    // Log the packet details (for demonstration purposes)
    printk(KERN_INFO "[KSentinel] Packet: src_ip=%pI4, dest_ip=%pI4, src_port=%u, dest_port=%u\n",
           &src_ip, &dest_ip, src_port, dest_port);

    // if (result == ANOMALY_DETECTED) {
    //     // Take action for anomaly
    //     printk(KERN_WARNING "Anomaly detected!\n");
    //     return NF_DROP;
    // }

    return NF_ACCEPT;
}

void analyze_packet(void) {
    netfilter_ops.hook = analyze_packet_hook;
    netfilter_ops.pf = PF_INET;
    netfilter_ops.hooknum = NF_INET_PRE_ROUTING;
    netfilter_ops.priority = NF_IP_PRI_FIRST;

    nf_register_net_hook(&init_net, &netfilter_ops);
    printk(KERN_INFO "[KSentinel] Network Traffic Analyzer initialized\n");
}

EXPORT_SYMBOL(analyze_packet);

static void __exit analyzer_exit(void) {
    nf_unregister_net_hook(&init_net, &netfilter_ops);
    printk(KERN_INFO "[KSentinel] Network Traffic Analyzer exited\n");
}

module_exit(analyzer_exit);
