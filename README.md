# ft_malcolm - ARP Spoofing Tool

![Network Security](https://img.shields.io/badge/security-ARP_spoofing-red)
![C Programming](https://img.shields.io/badge/language-C-blue)
![Linux](https://img.shields.io/badge/platform-Linux-lightgrey)


`ft_malcolm` is a C project to demonstrate how ARP spoofing works at a low network level using raw sockets.

⚡ **This tool must be tested inside a Virtual Machine (VM)** and used in a safe lab environment for learning purposes.

---

## 📚 Description

`ft_malcolm` listens for ARP requests and sends forged ARP replies on your local network.

You can:

- Select a **source IP** and **source MAC** (the spoofed identity).
- Define the **target IP** and **target MAC** (the victim machine).
- When the victim sends an ARP request (for example by using `ping`), `ft_malcolm` replies with the spoofed data.

---

## ⚙️ Compilation

Compile the project using:

```bash
make
```

This will generate an executable:

```bash
./ft_malcolm
```

---

## 💻 Usage

**Run the program with administrator rights**:

```bash
sudo ./ft_malcolm <source_ip> <source_mac> <target_ip> <target_mac>
```

(linux : ip a, MACOS : ifconfig)

---

✅ **Testing Procedure:**

1. Launch the program inside a Virtual Machine.
2. On the target machine (victim), use:

```bash
ping <source_ip>
```

3. `ft_malcolm` will detect the ARP request and send a forged ARP reply to the target.


<!--Manually configure the default gateway for the network : 
- ```sudo ip addr add 10.14.5.50/16 dev enp0s3``` : Add an IP address to the interface
- ```sudo ip link set enp0s3 up``` : Enable network interface
- ```sudo ip route add default via 10.14.0.1``` : Configure the default gateway-->



⚠️ **Disclaimer:**  
This project is for **learning and educational use only**.  
Do not use it on real networks or without permission — you risk breaking network trust and facing legal consequences.
