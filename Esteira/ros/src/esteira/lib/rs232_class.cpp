#include "rs232_class.hpp"

Rs232_class::Rs232_class(int p, int r){
	// --------------------- RS-232 Init-------------------------
	cport_nr = p;
	bdrate = r;
	mode[0] = '8';
	mode[1] = 'N';
	mode[2] = '1';
	mode[3] = 0;
	if(RS232_OpenComport(cport_nr, bdrate, mode)){
		std::cout<< "Can not open comport\n";
		exit(0);
	}
}

void Rs232_class::send_number(int instruction){
	// --------------------- RS-232 Enviar-------------------------
	std::ostringstream convert;
	convert << instruction;
	std::string send_string = convert.str() + '\n';
	RS232_cputs(cport_nr, send_string.c_str());
	std::cout<< "Enviado: " << send_string << std::endl;
}

void Rs232_class::send_string(std::string send_string){
	// --------------------- RS-232 Enviar-------------------------
	send_string = send_string + '\n';
	RS232_cputs(cport_nr, send_string.c_str());
	std::cout<< "Enviado: " << send_string << std::endl;
}

void Rs232_class::receive(){
	// --------------------- RS-232 Ler-------------------------
	int n = RS232_PollComport(cport_nr, buf, 4095);
	if(n > 0){
		buf[n] = 0;   /* always put a "null" at the end of a string! */
		for(int j=0; j < n; j++)
			if(buf[j] < 32)  /* replace unreadable control-codes by dots */
				buf[j] = '.';
		std::cout<< "Received " << n << " bytes: " << (char *)buf << std::endl;
	}
}