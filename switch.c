 /*
  * switch.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>

#include "main.h"
#include "net.h"
#include "man.h"
#include "switch.h"
#include "packet.h"

#define MAX_MSG_LENGTH 100
#define PKT_PAYLOAD_MAX 100
#define TENMILLISEC 10000   /* 10 millisecond sleep */
#define FORWARDING_TABLE_SIZE 100

int look_up_update_ftable(struct forwarding_table_entry forwarding_table[], 
	struct packet *in_packet, int in_port) {

	int k = 0;
	int out_port = -1;
	int recorded = 0;

	// Go through the forwarding table for finding the port 
	// corresponding to the destination node.
	while (forwarding_table[k].valid == 1 && k < FORWARDING_TABLE_SIZE) {
		if (in_packet->dst == forwarding_table[k].dest) {
			// Out port number matches a forwarding table entry
			// Choose this port for forwarding the packet
			out_port = forwarding_table[k].port;
		}
		if (in_packet->src == forwarding_table[k].dest) {
			// If the source node has been recorded in the table,
			// set the flag.
			recorded = 1;
		}
		k++;
	}

	if (recorded == 0 && k < FORWARDING_TABLE_SIZE) {	
		// The source node hasn't been recorded
		forwarding_table[k].valid = 1;
		forwarding_table[k].dest = in_packet->src;
		forwarding_table[k].port = in_port;
		printf("Swtich debug: recorded node %d at port %d in the forwarding table\n", 
			in_packet->src, in_port);
	}

	// Print out the forwarding table for debugging
	k = 0;
	printf("Swtich debug: forwarding table:\n");
	printf("Valid\tDest\tPort\t\n");
	while (forwarding_table[k].valid == 1 && k < FORWARDING_TABLE_SIZE) {
		printf("%d\t%d\t%d\t\n", forwarding_table[k].valid, 
			forwarding_table[k].dest, forwarding_table[k].port);
		k++;
	}

	return out_port;
	
}	

/*
 *  Main 
 */

void switch_main(int switch_id)
{

/* State */

struct net_port *node_port_list;
struct net_port **node_port;  // Array of pointers to node ports
int node_port_num;            // Number of node ports

int i, k, n;

struct packet *in_packet; /* Incoming packet */

struct net_port *p;

struct forwarding_table_entry forwarding_table[FORWARDING_TABLE_SIZE];
int out_port;

/*
 * Initialize pipes 
 */

/*
 * Create an array node_port[ ] to store the network link ports
 * at the host.  The number of ports is node_port_num
 */
node_port_list = net_get_port_list(switch_id);

	/*  Count the number of network link ports */
node_port_num = 0;
for (p=node_port_list; p!=NULL; p=p->next) {
	node_port_num++;
}
	/* Create memory space for the array */
node_port = (struct net_port **) 
	malloc(node_port_num*sizeof(struct net_port *));

	/* Load ports into the array */
p = node_port_list;
for (k = 0; k < node_port_num; k++) {
	node_port[k] = p;
	p = p->next;
}	


// Initialize the forwarding table
for (k = 0; k < FORWARDING_TABLE_SIZE; k++) {
	forwarding_table[k].valid = 0;
	forwarding_table[k].dest = 0;
	forwarding_table[k].port = 0;
}

while(1) {
	
	/*
	 * Get packets from incoming links and translate to jobs
  	 * Put jobs in job queue
 	 */

	for (k = 0; k < node_port_num; k++) { /* Scan all ports */

		in_packet = (struct packet *) malloc(sizeof(struct packet));
		n = packet_recv(node_port[k], in_packet);

		if (n > 0){
			// Look up destination in forwading table,
			// and update forwarding table
			out_port = look_up_update_ftable(forwarding_table, in_packet, k);

			if (in_packet->dst != switch_id) {	// The destination is other node.
				if (out_port != -1) {
					// Node isn't found in the forwarding table 
					// Send packet to the destination port
					packet_send(node_port[out_port], in_packet);
					printf("Swtich %d debug: packet is sent to node %d from port %d\n", 
						switch_id, in_packet->dst, out_port);
				} else {
					// Broadcast packet
					for (i = 0; i < node_port_num; i++) {
						if (i != k) {	// Don't send to the incoming port
							packet_send(node_port[i], in_packet);
							printf("Swtich %d debug: packet going to node %d is boardcasted from port %d\n", 
								switch_id, in_packet->dst, i);
						}
					}
					
				}
			}

			free(in_packet);
		}
		else {
			free(in_packet);
		}
	}

	/* The host goes to sleep for 10 ms */
	usleep(TENMILLISEC);

} /* End of while loop */

}




