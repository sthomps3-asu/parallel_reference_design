export SHELL=/bin/bash
export ROOTDIR=$(PWD)
include Makefile.globals
include config.mk

.PHONY: all platform hbilnx ssl hbi

PROJECTS = platform/$(PLATFORM) drivers

all: platform ssl hbi hbilnx 
tools:
	@echo "Building Toolchain...."
	$(MAKE) -C $(PLATFORM_DIR)/tools 
	
platform:
	@echo "Building Platform...."
	mkdir -p $(INSTALL_LIB_PATH)
	$(MAKE) -C $(ROOTDIR)/platform/$(PLATFORM)/

hbilnx: platform ssl hbi
	$(MAKE) -C $(ROOTDIR)/lnxdrivers/

hbilnx_clean:
	$(MAKE) -C $(ROOTDIR)/lnxdrivers/ clean

hbi_test: hbilnx_clean hbilnx
	$(MAKE) -C $(ROOTDIR)/tst/ clean
	$(MAKE) -C $(ROOTDIR)/tst/ 

hbi_test_clean:
	$(MAKE) -C $(ROOTDIR)/tst/ clean

hbi: 
	mkdir -p $(INSTALL_LIB_PATH)
	$(MAKE) -C $(ROOTDIR)/drivers/hbi/

hbi_clean:
	$(MAKE) -C $(ROOTDIR)/drivers/hbi/ clean

ssl: 
	mkdir -p $(INSTALL_LIB_PATH)
	$(MAKE) -C $(ROOTDIR)/platform/$(PLATFORM)/driver/ssl
codec :
	$(MAKE) -C $(ROOTDIR)/platform/$(PLATFORM)/driver/sound
codec_clean:
	$(MAKE) -C $(ROOTDIR)/platform/$(PLATFORM)/driver clean

ssl_clean:
	$(MAKE) -C $(ROOTDIR)/platform/$(PLATFORM)/driver/ssl clean

help:
	@echo "ROOTDIR="$(ROOTDIR)
	@echo "PLATFORM="$(PLATFORM)
	@echo "ARCH="$(ARCH)
	@echo "VPROC_DEV_CLASS="$(VPROC_DEV_CLASS)
	@echo "KSRC="$(KSRC)
	@echo "SHELL="$(SHELL)
  
# make clean causes a script to go into each of the 
# project folders and run the make clean command
clean:
	rm -f *.symvers *.orders *.o 
	@ (										\
		for dir in $(PROJECTS) ; do						\
		DIR=$${dir}/;						\
		$(MAKE) -C $$DIR clean ;					\
		done                                                                    \
	)


# make install causes a script to go into each of the
# project folders and run the make install command
install:





