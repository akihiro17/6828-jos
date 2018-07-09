#include "ns.h"
#include "inc/lib.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	uint32_t req, whom;
        int perm, r;
        void *pg;

        while (1) {
                perm = 0;
                req = ipc_recv((int32_t *) &whom, &nsipcbuf, &perm);
		if (req != NSREQ_OUTPUT) {
			cprintf("Not a NSREQ_OUTPUT\n");
			continue;
		}
		struct jif_pkt *pkt = &(nsipcbuf.pkt);
		sys_send_packet(pkt->jp_data, pkt->jp_len);
		sys_yield();
	}
}
