#files for global defines

EXTRA_CFLAGS += -I$(ROOTDIR)/include
EXTRA_CFLAGS += -I$(ROOTDIR)/platform/$(PLATFORM)/include
EXTRA_CFLAGS += -D$(BUILD_TYPE) -DDEBUG_LEVEL=$(DEBUG_LEVEL)
EXTRA_LDFLAGS += -zmuldefs

ifneq ($(TARGET),)
	EXTRA_CFLAGS += -DTARGET=$(TARGET)
ifeq ($(TARGET),TW)
	EXTRA_CFLAGS += -DTW=$(TW)
endif
endif

ifneq ($(HBI),)
	EXTRA_CFLAGS += -DHBI=$(HBI)
ifeq ($(HBI),I2C)
	EXTRA_CFLAGS += -DI2C=$(I2C)
endif
ifeq ($(HBI),SPI)
	EXTRA_CFLAGS += -DSPI=$(SPI)
endif
endif
ifeq ($(BOOT_FROM_HOST),yes)
	EXTRA_CFLAGS += -DBOOT_FROM_HOST
endif

ifeq ($(FLASH_PRESENT),yes)
	EXTRA_CFLAGS += -DFLASH_PRESENT
endif

ifeq ($(VPROC_DEV_ENDIAN),little)
	EXTRA_CFLAGS += -DVPROC_DEV_ENDIAN_LITTLE=1
else
	EXTRA_CFLAGS += -DVPROC_DEV_ENDIAN_LITTLE=0
endif

ifeq ($(HOST_ENDIAN),little)
	EXTRA_CFLAGS += -DHOST_ENDIAN_LITTLE=1
else
	EXTRA_CFLAGS += -DHOST_ENDIAN_LITTLE=0
endif

ifneq ($(HBI_MAX_INST_PER_DEV),)
	EXTRA_CFLAGS += -DHBI_MAX_INST_PER_DEV=$(HBI_MAX_INST_PER_DEV)
endif

ifneq ($(HBI_MAX_INSTANCES),)
	EXTRA_CFLAGS += -DHBI_MAX_INSTANCES=$(HBI_MAX_INSTANCES)
endif

ifneq ($(VPROC_MAX_NUM_DEVS),)
	EXTRA_CFLAGS += -DVPROC_MAX_NUM_DEVS=$(VPROC_MAX_NUM_DEVS)
endif

ifneq ($(VPROC_DEV_NAME_SIZE),)
	EXTRA_CFLAGS += -DVPROC_DEV_NAME_SIZE=$(VPROC_DEV_NAME_SIZE)
endif

ifneq ($(NUM_MAX_LOCKS),)
	EXTRA_CFLAGS += -DNUM_MAX_LOCKS=$(NUM_MAX_LOCKS)
endif

ifneq ($(HBI_BUFFER_SIZE),)
	EXTRA_CFLAGS += -DHBI_BUFFER_SIZE=$(HBI_BUFFER_SIZE)
endif

EXTRA_CFLAGS += -DHBI_ENABLE_PROCFS=$(HBI_ENABLE_PROCFS)
FLAGS += $(EXTRA_CFLAGS)