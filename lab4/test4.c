#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include "i8254.h"
#include "test4.h"
#include "peripherals.h"
#include "utilities.h"

//types

//state of the machine
typedef enum {
	INIT, DRAW, COMP
} state_t;

//event of the machine
typedef enum {
	RDOWN, RUP, MOVE
} event_type_t;

//struct for handling the events
typedef struct {
	event_type_t type;
} event_t;


int check_pos_line(event_t * evt, short length, int x, int y);//function that simulates a state machine

int test_packet(unsigned short cnt) {
	int ipc_status; //ipc_status
	message msg; //message
	int r; //variable that stores the error
	int mouse_hook_id = MOUSE_IRQ;
	long irq_set = BIT(mouse_hook_id);
	if (mouse_subscribe_int(&mouse_hook_id) == -1) {
		printf("Error: It wasn't possible to subscribe the interruption\n");
		return -1;
	}
	if (mouse_cmd_write(MOUSE_SET_STREAM) != 0) {
		printf("Error: It wasn't possible to set stream on the mouse\n");
		return -1;
	}
	if (mouse_cmd_write(MOUSE_ENABLE) != 0) {
		printf("Error: It wasn't possible to enable the mouse\n");
		return -1;
	}
	unsigned long counter = 0; //this variable stores the number of packets received
	unsigned long next_byte = 0; //this variable stores the next byte of the packet to be received
	unsigned char packet[3]; //array that is going to store the packets
	ignore_interrupts(MOUSE_IRQ);
	while (counter < cnt) {
		/* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					unsigned long old_byte = next_byte;//this variable stores the value of the old next byte
					if (mouse_get_packet(&next_byte, packet) != 0) {
						return -1;
					}
					if (old_byte != next_byte && next_byte == 0) {
						print_packet(packet);
						counter++;
					}
				}

				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	if (mouse_cmd_write(MOUSE_DISABLE_STREAM_MODE) != 0) {
		return 1;
	}
	if (mouse_unsubscribe_int(&mouse_hook_id) != 0) {
		printf("Error: It was not possible to unsubscribe the mouse\n");
		return 1;
	}
	return 0;
}

int test_async(unsigned short idle_time) {
	int ipc_status; //ipc_status
	message msg; //message
	int r; //variable that stores the error
	long irq_set_timer = BIT(timer_hook_id);
	if (timer_subscribe_int() == -1) {
		printf("Error: It wasn't possible to subscribe the interruption\n");
		return -1;
	}
	int mouse_hook_id = MOUSE_IRQ;
	long irq_set_mouse = BIT(mouse_hook_id);
	if (mouse_subscribe_int(&mouse_hook_id) == -1) {
		printf("Error: It wasn't possible to subscribe the interruption\n");
		return -1;
	}

	if (mouse_cmd_write(MOUSE_SET_STREAM) != 0) {
		printf("Error: It wasn't possible to set stream on the mouse\n");
		return -1;
	}
	if (mouse_cmd_write(MOUSE_ENABLE) != 0) {
		printf("Error: It wasn't possible to enable the mouse\n");
		return -1;
	}
	unsigned long counter = 0; //this variable stores the time that passed between each interruption
	unsigned long next_byte = 0; //this variable stores the next byte of the packet
	unsigned char packet[3]; //array that is going to store the packets
	ignore_interrupts(MOUSE_IRQ);//ignores pevious interrupts
	while (counter < idle_time * 60) {
		/* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */
					unsigned long old_byte = next_byte;//this variable stores the value of the old next byte
					if (mouse_get_packet(&next_byte, packet) != 0) {
						return -1;
					}
					if (old_byte != next_byte && next_byte == 0) {
						print_packet(packet);
					}
					counter = 0;
				}
				if (msg.NOTIFY_ARG & irq_set_timer) { /* subscribed interrupt */
					counter++;
				}

				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if (mouse_cmd_write(MOUSE_DISABLE_STREAM_MODE) != 0) {
		return 1;
	}
	if (mouse_unsubscribe_int(&mouse_hook_id) != 0) {
		printf("Error: It was not possible to unsubscribe the mouse\n");
		return 1;
	}
	if (timer_unsubscribe_int() != 0) {
		return 1;
	}
	printf("Exit: Timeout\n");
	return 0;
}

int test_config(void) {
	int mouse_hook_id = 12;//hook for handling the mouse interruptions
	if (mouse_subscribe_int(&mouse_hook_id) == -1) {
		printf("Error: It wasn't possible to subscribe the interruption\n");
		return -1;
	}

	if (mouse_cmd_write(MOUSE_DISABLE_STREAM_MODE) != 0) {
		printf("Error: It wasn't possible to disable stream mode\n");
		return 1;
	}

	if (mouse_cmd_write(MOUSE_STATUS_REQUEST) != 0) {
		printf("Error:It was not possible to write to the mouse\n");
		return 1;
	}
	unsigned char packet_status[3];
	unsigned int i; //counter
	for (i = 0; i < 3; i++) {
		if (mouse_cmd_read(&packet_status[i]) != 0) {
			printf("Error:It was not possible to read from the mouse\n");
			return 1;
		}
	}
	if ((packet_status[0] & BIT(6)) == BIT(6)) {
		printf("Mode: Remote\n");
	} else {
		printf("Mode: Stream\n");
	}
	if ((packet_status[0] & BIT(5)) == BIT(5)) {
		printf("Data Reporting: Enabled\n");
	} else {
		printf("Data Reporting: Disabled\n");
	}
	if ((packet_status[0] & BIT(4)) == BIT(4)) {
		printf("Scaling: 2:1\n");
	} else {
		printf("Scaling: 1:1\n");
	}
	if ((packet_status[0] & BIT(0)) == BIT(0)) {
		printf("Left Button: Pressed\n");
	} else {
		printf("Left Button: Released\n");
	}
	if ((packet_status[0] & BIT(2)) == BIT(2)) {
		printf("Middle Button: Pressed\n");
	} else {
		printf("Middle Button: Released\n");
	}
	if ((packet_status[0] & BIT(1)) == BIT(1)) {
		printf("Right Button: Pressed\n");
	} else {
		printf("Right Button: Released\n");
	}
	if ((packet_status[1]) == 0) {
		printf("Resolution: 1 count per mm\n");
	} else if ((packet_status[1]) == 1) {
		printf("Resolution: 2 counts per mm\n");
	} else if ((packet_status[1]) == 2) {
		printf("Resolution: 4 counts per mm\n");
	} else if ((packet_status[1]) == 3) {
		printf("Resolution: 8 counts per mm\n");
	}
	printf("Sample rate: %d\n", packet_status[2]);

	return 0;
}

int test_gesture(short length) {
	int ipc_status; //ipc_status
	message msg; //message
	int r; //variable that stores the error
	int mouse_hook_id = MOUSE_IRQ;
	long irq_set = BIT(mouse_hook_id);
	if (mouse_subscribe_int(&mouse_hook_id) == -1) {
		printf("Error: It wasn't possible to subscribe the interruption\n");
		return -1;
	}

	if (mouse_cmd_write(MOUSE_SET_STREAM) != 0) {
		printf("Error: It wasn't possible to set stream on the mouse\n");
		return -1;
	}
	if (mouse_cmd_write(MOUSE_ENABLE) != 0) {
		printf("Error: It wasn't possible to enable the mouse\n");
		return -1;
	}
	int bool_line = 0; //this variable is set to 0 if
	unsigned long next_byte = 0; //this variable stores the next byte of the packet to be received
	unsigned char packet[3]; //array that is going to store the packets
	ignore_interrupts(MOUSE_IRQ);//ignore interrupts in the mouse
	while (bool_line == 0) {//continue loop until the line with positive slope has been found
		/* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					unsigned long old_byte = next_byte;
					if (mouse_get_packet(&next_byte, packet) != 0) {
						return -1;
					}
					if (old_byte != next_byte && next_byte == 0) {
						bool_line = test_gesture_aux(length, packet);
						print_packet(packet);
					}
				}

				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	printf("Exit: Line with positive slope found\n");
	if (mouse_cmd_write(MOUSE_DISABLE_STREAM_MODE) != 0) {
		return 1;
	}
	if (mouse_unsubscribe_int(&mouse_hook_id) != 0) {
		printf("Error: It was not possible to unsubscribe the mouse\n");
		return 1;
	}
	return 0;
}


void print_packet(unsigned char * packet) {
	printf("B1=0x%X ", packet[0]);
	printf("B2=0x%X ", packet[1]);
	printf("B3=0x%X ", packet[2]);
	printf("LB=%d ", packet[0] & BIT(0));
	printf("MB=%d ", (packet[0] & BIT(2)) >> 2);
	printf("RB=%d ", (packet[0] & BIT(1)) >> 1);
	printf("XOV=%d ", packet[0] & BIT(6));
	printf("YOV=%d ", packet[0] & BIT(7));
	if ((packet[0] & BIT(4)) == BIT(4)) {
		printf("X=%d ", -conv_to_decimal(two_complement_sym(packet[1])));
	} else {
		printf("X=%d ", (int) packet[1]);
	}
	if ((packet[0] & BIT(5)) == BIT(5)) {
		printf("Y=%d\n", -conv_to_decimal(two_complement_sym(packet[2])));
	} else {
		printf("Y=%d\n", (int) packet[2]);
	}
}


int test_gesture_aux(short length, unsigned char * packet) {
	static event_type_t ev_type = RUP; //stores the value of the last packet event
	int x, y; //these variables will store the values of x and y in the packet
	if ((packet[0] & BIT(4)) == BIT(4)) {

		x = -conv_to_decimal(two_complement_sym(packet[1]));
	} else {
		x = (int) packet[1];
	}
	if ((packet[0] & BIT(5)) == BIT(5)) {
		y = -conv_to_decimal(two_complement_sym(packet[2]));
	}

	else {
		y = (int) packet[2];
	}
	int r_pressed = 0; //this variable will be set to 1 if the r button is pressed
	if ((packet[0] & BIT(1)) == BIT(1)) {
		r_pressed = 1;
	}
	if (r_pressed == 0) {
		ev_type = RUP;
	} else if (ev_type == RUP) {
		ev_type = RDOWN;
	} else {
		ev_type = MOVE;
	}
	event_t new_event; // new event that corresponds to the current packet received
	new_event.type = ev_type;
	if (check_pos_line(&new_event, length, x, y) == 1) {
		return 1;
	} else {
		return 0;
	}

}


int check_pos_line(event_t * evt, short length, int x, int y) {
	static state_t state = INIT; // initial state; keep state
	static int x_length = 0;
	static int y_length = 0;
	switch (state) {
	case INIT:
		switch (evt->type) {
		case RDOWN:
			state = DRAW;
			break;

		case MOVE:
			;//empty statement
			int array_lengths[5];
			array_lengths[0] = x_length;
			array_lengths[1] = y_length;
			array_lengths[2] = x;
			array_lengths[3] = y;
			array_lengths[4] = length;
			if (samesignarray(5, array_lengths) == 0) {
				//RESET
				state = INIT;
				x_length = 0;
				y_length = 0;
			}
			else {
				y_length = y_length + y;
				x_length = x_length + x;
				if (abs(y_length) >= abs(length)) {
					state = COMP;
					return 1;
				}
				else {
					state=DRAW;
				}
			}
			break;
		default:
			break;
		}
		break;
	case DRAW:
		switch ((evt->type)) {
		case MOVE:
			; //empty statement
			int array_lengths[5];
			array_lengths[0] = x_length;
			array_lengths[1] = y_length;
			array_lengths[2] = x;
			array_lengths[3] = y;
			array_lengths[4] = length;
			if (samesignarray(5, array_lengths) == 0) {
				//RESET
				state = INIT;
				x_length = 0;
				y_length = 0;
			} else {
				y_length = y_length + y;
				x_length = x_length + x;
				if (abs(y_length) >= abs(length)) {
					state = COMP;
					return 1;
				}
				else {

				}
			}
			break;

		case RUP:
			state = INIT;

			x_length = 0;
			y_length = 0;
			break;

		case RDOWN:
			state = DRAW;
			break;
		default:
			break;

		}
		break;
	default:
		break;
	}
	return 0;
}

