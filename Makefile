BINARY_NAME = beep-morse
INSTALLDIR = /usr/local/bin
SOURCES = beep-morse.c

obj-m := morse-module.o
KERNELVERSION ?= $(shell uname -r)
KDIR := /lib/modules/$(KERNELVERSION)/build
PWD := $(shell pwd)
INSTALL_MOD_DIR ?= extra

.PHONY: all install unistall clear

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	
install: all 
	mkdir -p $(INSTALLDIR)
	gcc $(SOURCES) -o $(INSTALLDIR)/$(BINARY_NAME)
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

uninstall:
	$(RM) $(INSTALLDIR)/$(BINARY_NAME)
	$(RM) /lib/modules/$(KERNELVERSION)/$(INSTALL_MOD_DIR)/morse_module.ko.xz

clean:
	$(RM) $(BINARY_NAME)
	$(MAKE) -C $(KDIR) M=$(PWD) clean
