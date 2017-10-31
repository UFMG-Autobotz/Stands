#include "rs232.h"
#include <iostream>
#include <cstdlib>
#include <sstream>

#ifndef RS232_CLASS_H
#define RS232_CLASS_H

class Rs232_class {

private:
	int cport_nr,	//serial == 24 ou 25 (ACM0 ou ACM1) "/dev/ttyACM0"
	bdrate;		/* 9600 baud */
	char mode[4];
	unsigned char buf[4096];

public:  
	Rs232_class(int p = 24, int r = 9600);
	void send_number(int instruction);
	void send_string(std::string send_string);
	void receive();
	unsigned char* get_buf() { return buf; }
};

#endif