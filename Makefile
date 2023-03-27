GXX=/home/ubuntu/Desktop/buildroot2/buildroot-2022.11/output/host/bin/arm-buildroot-linux-gnueabihf-gcc
FLAGS=-Wall -pedantic -O2
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
	rm -rf $(OUT)/*
	rm -f main
	rm $(IPFILE)

setIP:
	echo -n "Raspberry Pi IP: "
	# TODO

upload: 
	scp main root@$$(cat $(IPFILE)):.

main: $(OUT)/main.o $(OUT)/accelerometer.o $(OUT)/colorsensor.o
	$(GXX) $(FLAGS) -o $@ $^

$(OUT)/main.o: $(SRC)/main.c
	$(GXX) -c $(FLAGS) -o $@ $^
	
$(OUT)/accelerometer.o: $(SRC)/Accelerometer/accelerometer.c $(SRC)/Accelerometer/accelerometer.h
	$(GXX) -c $(FLAGS) -o $@ $<
	
$(OUT)/colorsensor.o: $(SRC)/ColorSensor/colorsensor.c $(SRC)/ColorSensor/colorsensor.h
	$(GXX) -c $(FLAGS) -o $@ $<
