#include "../rs232_class.hpp"
#include <time.h>

time_t start=0, end;
int main(int argc, char** argv){
	srand (time(NULL));
	Rs232_class arduino_talk(24);
	unsigned char * buf;
	while (1){
		arduino_talk.send_number(rand());
		arduino_talk.receive();
		buf = arduino_talk.get_buf();
		time(&start);
		do time(&end);
		while(end-start < 1);
		
	}
	return 0;

}
