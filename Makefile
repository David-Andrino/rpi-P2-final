GXX=/home/ubuntu/Desktop/buildroot2/buildroot-2022.11/output/host/bin/arm-buildroot-linux-gnueabihf-gcc
FLAGS=-pthread -Wall -pedantic -O2
SRC=src
OUT=out
IPFILE=.lastip

.PHONY=all clean upload setIP

all: clean main upload

outdir:
	-@if [ ! -d "$(OUT)" ]; then mkdir "$(OUT)"; fi

clean:
	-@rm -rf $(OUT)/* 2>/dev/null || true
	-@rm -f main 2>/dev/null || true

cleanIP:
	-@rm $(IPFILE) 2>/dev/null || true

setIP:
	@read -p "Enter RaspberryPi's IP address: " raspiIP; \
    echo -n "$$raspiIP" > $(IPFILE); \
    unset raspiIP

upload: 
	scp main root@$$(cat $(IPFILE)):.

main: $(OUT)/main.o $(OUT)/accelerometer.o $(OUT)/colorSensor.o 
	$(GXX) $(FLAGS) -o $@ $^

$(OUT)/main.o: $(SRC)/main.c outdir
	$(GXX) -c $(FLAGS) -o $@ $<
	
$(OUT)/accelerometer.o: $(SRC)/Accelerometer/accelerometer.c $(SRC)/Accelerometer/accelerometer.h outdir
	$(GXX) -c $(FLAGS) -o $@ $<
	
$(OUT)/colorSensor.o: $(SRC)/ColorSensor/colorSensor.c $(SRC)/ColorSensor/colorSensor.h outdir
	$(GXX) -c $(FLAGS) -o $@ $<
