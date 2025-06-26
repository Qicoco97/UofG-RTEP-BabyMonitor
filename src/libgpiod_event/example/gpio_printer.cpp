#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "gpioevent.h"

class EventPrinter : public GPIOPin::GPIOEventCallbackInterface {
	virtual void hasEvent(gpiod_line_event& e) override {
	    switch (e.event_type) {
	    case GPIOD_LINE_EVENT_RISING_EDGE:
		printf("Rising!\n");
		break;
	    case GPIOD_LINE_EVENT_FALLING_EDGE:
		printf("Falling\n");
		break;
	    default:
		printf("Unkown event\n");
	    }
	}
};

int main(int argc, char *argv[]) {
	fprintf(stderr,"Press any key to stop.\n");
	EventPrinter callbackInterface;
	GPIOPin gpiopin;
	gpiopin.registerCallback(&callbackInterface);
	const int gpioPinNo = 27;
	gpiopin.start(gpioPinNo);
	getchar();
	gpiopin.stop();
	return 0;
}
