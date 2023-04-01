GXX=/home/ubuntu/Desktop/buildroot2/buildroot-2022.11/output/host/bin/arm-buildroot-linux-gnueabihf-gcc
FLAGS=-lpthread -Wall -pedantic -O2
SRC=src
OUT=out
IPFILE=.lastip

.PHONY=all clean upload setIP

all: 
	clean
	main
	setIP
	upload

clean:
	-rm -rf $(OUT)/*
	-rm -f main
	-rm $(IPFILE)

setIP:
	@read -p "Enter RaspberryPi's IP address: " raspiIP; \
    echo -n "$$raspiIP" > $(IPFILE); \
    unset raspiIP

upload: 
	scp main root@$$(cat $(IPFILE)):.

main: $(OUT)/main.o $(OUT)/accelerometer.o $(OUT)/colorSensor.o
	$(GXX) $(FLAGS) -o $@ $^

$(OUT)/main.o: $(SRC)/main.c
	$(GXX) -c $(FLAGS) -o $@ $^
	
$(OUT)/accelerometer.o: $(SRC)/Accelerometer/accelerometer.c $(SRC)/Accelerometer/accelerometer.h
	$(GXX) -c $(FLAGS) -o $@ $<
	
$(OUT)/colorSensor.o: $(SRC)/ColorSensor/colorSensor.c $(SRC)/ColorSensor/colorSensor.h
	$(GXX) -c $(FLAGS) -o $@ $<
