#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <inc/types.h>
#include <kern/pci.h>
#include <kern/pmap.h>
#include <inc/string.h>

#define PCI_VENDOR_ID_82540EM		0x8086
#define PCI_DEVICE_ID_82540EM		0x100e

#define E1000_TDBAL    (0x03800 >> 2)  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    (0x03804 >> 2)  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    (0x03808 >> 2)  /* TX Descriptor Length - RW */
#define E1000_TDH      (0x03810 >> 2)  /* TX Descriptor Head - RW */
#define E1000_TDT      (0x03818 >> 2)  /* TX Descripotr Tail - RW */
#define E1000_TIPG     (0x00410 >> 2)  /* TX Inter-packet gap -RW */
#define E1000_TCTL     (0x00400 >> 2)  /* TX Control - RW */

/* Transmit Control */
#define E1000_TCTL_RST    0x00000001    /* software reset */
#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_BCE    0x00000004    /* busy check enable */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
#define E1000_TCTL_CT     0x00000ff0    /* collision threshold */
#define E1000_TCTL_COLD   0x003ff000    /* collision distance */
#define E1000_TCTL_SWXOFF 0x00400000    /* SW Xoff transmission */
#define E1000_TCTL_PBE    0x00800000    /* Packet Burst Enable */
#define E1000_TCTL_RTLC   0x01000000    /* Re-transmit on late collision */
#define E1000_TCTL_NRTU   0x02000000    /* No Re-transmit on underrun */
#define E1000_TCTL_MULR   0x10000000    /* Multiple request support */

#define NTXDESC 32
#define PKT_BUF_SIZE 2048

struct tx_desc
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
};

struct packet
{
	char buf[PKT_BUF_SIZE];
};

int pci_vendor_attach(struct pci_func*);
int send_packet(char *, uint16_t);

#endif	// JOS_KERN_E1000_H
