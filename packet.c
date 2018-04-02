
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include "main.h"
#include "packet.h"
#include "net.h"
#include "host.h"


void packet_send(struct net_port *port, struct packet *p)
{
char msg[PAYLOAD_MAX+4];
int i;

if (port->type == PIPE) {
	msg[0] = (char) p->src; 
	msg[1] = (char) p->dst;
	msg[2] = (char) p->type;
	msg[3] = (char) p->length;
	for (i=0; i<p->length; i++) {
		msg[i+4] = p->payload[i];
	}
	write(port->pipe_send_fd, msg, p->length+4);
//printf("PACKET SEND, src=%d dst=%d p-src=%d p-dst=%d\n", 
//		(int) msg[0], 
//		(int) msg[1], 
//		(int) p->src, 
//		(int) p->dst);
}

return;
}

int packet_recv(struct net_port *port, struct packet *p)
{
char msg[PAYLOAD_MAX+4];
int n;
int i;
	
if (port->type == PIPE) {
	n = read(port->pipe_recv_fd, msg, PAYLOAD_MAX+4);
	if (n>0) {
		p->src = (char) msg[0];
		p->dst = (char) msg[1];
		p->type = (char) msg[2];
		p->length = (int) msg[3];
		for (i=0; i<p->length; i++) {
			p->payload[i] = msg[i+4];
		}

// printf("PACKET RECV, src=%d dst=%d p-src=%d p-dst=%d\n", 
//		(int) msg[0], 
//		(int) msg[1], 
//		(int) p->src, 
//		(int) p->dst);
	}
}

return(n);
}

// Chris modified++ 04022018
int send_tree_packet(int node_port_num, int node_id, char node_type, struct net_port **node_port, 
		struct local_tree_info *local) {
	int k;
	struct payload_tree_packet *payload_tree;
	struct packet *new_packet;

	for (k = 0; k < node_port_num; k++) { /* Scan all ports */
		// Create the outgoing tree packet
		new_packet = (struct packet *) malloc(sizeof(struct packet));
		new_packet->src = (char) node_id;
		new_packet->dst = 0;
		new_packet->type = PKT_TREE_PACKET;
		new_packet->length = sizeof(struct payload_tree_packet);
	
		// Create the payload of the tree packet
		payload_tree = (struct payload_tree_packet *)new_packet->payload;
		payload_tree->rootID = (char)local->rootID;
		payload_tree->rootDist = local->rootDist;
		payload_tree->senderType = node_type;
		if (k == local->parent) {
			payload_tree->senderChild = 'Y';
		} else {
			payload_tree->senderChild = 'N';
		}
	
		// Send the packet
		packet_send(node_port[k], new_packet);
		free(new_packet);
	}
}
// Chris added--


