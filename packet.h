/* Definitions and prototypes for the link (link.c)
 */

// Chris added++ 04022018
struct local_tree_info{
	int rootID;
	int rootDist;
	int parent;		// The port number of the parent
	int *portTree;
}
// Chris added--

// receive packet on port
int packet_recv(struct net_port *port, struct packet *p);

// send packet on port
void packet_send(struct net_port *port, struct packet *p);

// send tree packet, Chris modified 04022018
int send_tree_packet(int node_port_num, int node_id, 
	char node_type, struct net_port **node_port, 
	struct local_tree_info *local);
