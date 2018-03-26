/* Definitions and prototypes for the link (link.c)
 */


// receive packet on port
int packet_recv(struct net_port *port, struct packet *p);

// send packet on port
void packet_send(struct net_port *port, struct packet *p);

// send tree packet
int send_tree_packet(int node_port_num, int node_id, 
	char node_type, struct net_port **node_port, 
	int localRootID, int localRootDist, int localParent);
