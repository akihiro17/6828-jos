#include <kern/e1000.h>

// LAB 6: Your driver code here

struct tx_desc txq[NTXDESC] __attribute__ ((aligned (16)));
struct rx_desc rxq[NTXDESC] __attribute__ ((aligned (16)));
struct packet packets[NTXDESC];
struct packet rx_packets[NRXDESC];

volatile uint32_t *e1000_regs;

int
pci_vendor_attach(struct pci_func *f) {
	pci_func_enable(f);
	e1000_regs = mmio_map_region((physaddr_t)f->reg_base[0], f->reg_size[0]);

	cprintf("device status: %x\n", e1000_regs[2]);

	// Set TDBAL (Transmit Descriptor Base Address) to the above allocated address.
	e1000_regs[E1000_TDBAL] = PADDR(txq);
	e1000_regs[E1000_TDBAH] = 0x0;

	// Set the Transmit Descriptor Length (TDLEN) register to the size (in bytes) of the descriptor ring
	e1000_regs[E1000_TDLEN] = NTXDESC * sizeof(struct tx_desc);

	// The Transmit Descriptor Head and Tail (TDH/TDT) registers are initialized (by hardware) to 0b
	e1000_regs[E1000_TDH] = 0x0;
	e1000_regs[E1000_TDT] = 0x0;

	// Initialize the Transmit Control Register (TCTL)
	// • Set the Enable (TCTL.EN) bit to 1b for normal operation.
	e1000_regs[E1000_TCTL] |= E1000_TCTL_EN;
	// Set the Pad Short Packets (TCTL.PSP) bit to 1b.
	e1000_regs[E1000_TCTL] |= E1000_TCTL_PSP;
	// Configure the Collision Distance (TCTL.COLD) to its expected value.
	// For full duplex operation, this value should be set to 40h
	e1000_regs[E1000_TCTL] |= E1000_TCTL_COLD;

	// the value that should be programmed into IPGT is 10
	e1000_regs[E1000_TIPG] = 0x0A | 0x04 << 10 | 0x06 << 20;

	memset(txq, 0, sizeof(struct tx_desc) * NTXDESC);

	for (int i = 0; i < NTXDESC; i++) {
		txq[i].addr = PADDR(&packets[i]);
		// RS bit
		txq[i].cmd |= 1 << 3;
		// 0b for legacy mode
		txq[i].cmd = 0;
		// DD bit
		txq[i].status |= 1;
	}

	// receive initialization
	e1000_regs[E1000_RAL] = E1000_MAC_LOW;
	e1000_regs[E1000_RAH] = E1000_MAC_HIGH;
	e1000_regs[E1000_RAH] |= E1000_RAH_AV;

	memset(rxq, 0, sizeof(struct rx_desc) * NRXDESC);

	for (int i = 0; i < NELEM_MTA; i++) {
		e1000_regs[E1000_MTA + i] = 0x00000000;
	}

	e1000_regs[E1000_RDBAL] = PADDR(rxq);
	e1000_regs[E1000_RDBAH] = 0x0;

	// Initialize the MTA (Multicast Table Array) to 0b
	e1000_regs[E1000_MTA] = 0x0;

	e1000_regs[E1000_RDLEN] = NRXDESC * sizeof(struct rx_desc);

	e1000_regs[E1000_RDH] = 0x0;
	e1000_regs[E1000_RDT] =	NRXDESC - 1;

	for (int i = 0; i < NRXDESC; i++) {
		rxq[i].buffer_addr = PADDR(&rx_packets[i]);
	}

	// Set the receiver Enable (RCTL.EN) bit to 1b
	e1000_regs[E1000_RCTL] |= E1000_RCTL_EN;
	// Set the Long Packet Enable (RCTL.LPE) bit
	// disable
	e1000_regs[E1000_RCTL] &= ~(E1000_RCTL_LPE);
	// Loopback Mode (RCTL.LBM) should be set to 00b
	e1000_regs[E1000_RCTL] &= E1000_RCTL_LBM_NO;
	e1000_regs[E1000_RCTL] &= E1000_RCTL_BSIZE_2048;
	e1000_regs[E1000_RCTL] |= E1000_RCTL_SECRC;

	return 0;
}

int send_packet(char *text, uint16_t length) {
	int tail = e1000_regs[E1000_TDT];

	if (length >= PKT_BUF_SIZE) {
		panic("buf");
	}

	if (txq[tail].status & 0x1) {
		memmove((void *)&packets[tail], (void *)text, length);
		txq[tail].length = length;

		// EOP
		txq[tail].cmd |= 1;
		// RPS
		// It is used is cases where the software must know that
		// the packet has been sent, and not just loaded to the transmit FIFO
		txq[tail].cmd |= 1 << 4;

		// reset DD bit
		txq[tail].status &= ~(0x1);

		e1000_regs[E1000_TDT] = (tail + 1) % NTXDESC;

		return 0;
	} else {
		return -1;
	}
}

int
receive_packet(char *text, uint16_t length) {
	return 0;
}
