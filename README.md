# Raspberry Pi Embedded Project

Project for the Embedded System Design with Raspberry Pi subject in the 2022/23 course. Made by David Andrino and Fernando Sanz.

Multi-threaded application that uses I2C to communicate with the TCS3462 color sensor and displays its measurements to the user through a terminal. It allows the user to select different color measurement calculations such as IR reject, clear compensation, etc.

## Build from Source
To use this application you have to clone the repository, compile it with a cross-compiling tool and update to the Raspberry Pi. You need a compiler created with the same Buildroot configuration that was used to generate the Raspberry Pi's embedded operating system.

```shelll
git clone https://github.com/SoraSpades/rpi-P2-final/ && \
cd rpi-P2-final/ && \
make && \
scp ./main root@<raspberry pi's IP>:.
```

After the execution of that command, the application can be found inside the home folder of the root user in the Raspberry Pi.

The usage of this application depends on the availability of the I2C virtual device inside the `/dev` folder in the Raspberry Pi. If it is missing, two things must be done:
1. Add the following lines to the `config.txt` file inside the boot partition of the SD card:
```
dtparam=i2c_arm=on
dtparam=i2c1=on
```
2. Create a file called `S60i2c` inside the `/etc/init.d/` folder in the Raspberry Pi with the following content and make it executable (`chmod +x /etc/init.d/S60i2c`):
```
#!/bin/sh
modprobe i2c-bcm2835
modprobe i2c-dev
```

## Download Release
You can download an alredy compiled version in the [Releases](https://github.com/SoraSpades/rpi-P2-final/releases) tab. It, however, will only work in a Raspberry Pi with an embedded operating system compiled with buildroot with the same parameters as in the classes and it needs i2c enabled. Download the executable and copy it with an app like `scp` or `sftp`.

## Main Features
- Interactive UI
- Multiple threads in Producer-Consumer model
- Use of 4 threads to accomodate to the 4 cores of the Raspberry
- Code checked with cppcheck for possible memory leaks
- Compliant to Embedded C Standard (check sources)
- Configurable color sensing

## Sources
- [MPU-6000 and MPU-6050 Register Map and Descriptions](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf)
- [MPU-6000 and MPU-6050 Product Specification](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)
- [TCS3472 Datasheet](https://cdn-shop.adafruit.com/datasheets/TCS34725.pdf)
- [Adafruit's Color Sensor Library for Arduino](https://github.com/adafruit/Adafruit_TCS34725)
- [POSIX thread (pthread) libraries](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html)
- [POSIX mutex specification](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_lock.html)
- [Embedded C Coding Standard](https://barrgroup.com/embedded-systems/books/embedded-c-coding-standard)
- [Raspberry Pi Cookbook](https://books.google.es/books?id=T0JRAgAAQBAJ&printsec=frontcover&hl=es#v=onepage&q&f=false)
- [I2C-tools Wiki](https://www.mankier.com/package/i2c-tools)
- [StackOverflow question regarding i2c](https://stackoverflow.com/questions/52975817/setup-i2c-reading-and-writing-in-c-language)
- [How to activate Raspberry-pi’s i2c bus](https://openest.io/en/services/activate-raspberry-pi-4-i2c-bus/)
- [Doxygen](https://www.doxygen.nl/index.html)
- [HSL and HSV](https://en.wikipedia.org/wiki/HSL_and_HSV)
