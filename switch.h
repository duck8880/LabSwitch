/* 
 * switch.h 
 */

struct forwarding_table_entry{
	int valid;
	char dest;
	int port;
};

void switch_main(int switch_id);


