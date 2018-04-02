/* 
 * switch.h 
 */

struct forwarding_table_entry{
	int valid;
	char dest;
	int port;
};

struct local_tree_info{
	int rootID;
	int rootDist;
	int parent;		// The port number of the parent
	int *portTree;
}

void switch_main(int switch_id);


