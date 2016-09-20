#Makefile:
#

COMMON_FLAG= -DARM=1 -DMIPS=2

ifeq ($(cpuarch) , arm)
CC=arm-linux-gcc
LD=arm-linux-ld
OBJCOPY=arm-linux-objcopy
CFLAGS= -O2 -g -I$(PRJPATH)/inc $(COMMON_FLAG) -DCPUARCH=ARM
ASFLAGS= -O2 -g -I$(PRJPATH)/inc $(COMMON_FLAG) -DCPUARCH=ARM
LDFLAGS=-Tlotus_works_arm.lds -Ttext 30000000
endif

ifeq ($(cpuarch) , mips)
CC=mips-sde-elf-gcc
LD=mips-sde-elf-ld
OBJCOPY=mips-sde-elf-objcopy
CFLAGS= -O2 -g -I$(PRJPATH)/inc -mips1 $(COMMON_FLAG) -DCPUARCH=MIPS
ASFLAGS= -O2 -g -I$(PRJPATH)/inc -mips1 $(COMMON_FLAG) -DCPUARCH=MIPS
LDFLAGS=-Tlotus_works_mips.lds
endif

export CC
export LD
export OBJCOPY

PRJPATH= $(shell pwd)
OUTPUTPATH= $(PRJPATH)/output
OUTPUTOBJ= $(wildcard output/*.o) 


export OUTPUTPATH
export CFLAGS
export ASFLAGS



MODULE=arch int mem std task usr sem uart clk msg ltai


lotus_works: $(MODULE)
	$(CC) -static -nostartfiles -nostdlib $(LDFLAGS) $(OUTPUTOBJ) -o $(OUTPUTPATH)/$@ -lgcc
	$(OBJCOPY) -O binary $(OUTPUTPATH)/$@ $(OUTPUTPATH)/lotus_works.bin
	cp $(OUTPUTPATH)/lotus_works.bin lotus_works.bin

.PHONY: test 
test:
	@echo $(cpuarch)
	@echo $(MODULE)
	$(CC) $(CFLAGS) -c setup.S -o setup.o
	$(CC) $(CFLAGS) -c boot.c -o boot.o
	cp setup.o ./output/setup.o
	cp boot.o ./output/boot.o


.PHONY: arch 
arch:
	make -C $(PRJPATH)/arch/$(cpuarch)

arch_clean:
	make -C $(PRJPATH)/arch/$(cpuarch) clean
	
.PHONY: int 
int:
	make -C $(PRJPATH)/int
	
int_clean:
	make -C $(PRJPATH)/int clean

.PHONY: mem
mem:
	make -C $(PRJPATH)/mem
	
mem_clean:
	make -C $(PRJPATH)/mem clean

.PHONY: std 
std:
	make -C $(PRJPATH)/std
	
std_clean:
	make -C $(PRJPATH)/std clean


.PHONY: task
task:
	make -C $(PRJPATH)/task
	
task_clean:
	make -C $(PRJPATH)/task clean
	
.PHONY: sem
sem:
	make -C $(PRJPATH)/sem

sem_clean:
	make -C $(PRJPATH)/sem clean
	
.PHONY: usr 	
usr:
	make -C $(PRJPATH)/usr
	
usr_clean:
	make -C $(PRJPATH)/usr clean
	
.PHONY: uart 	
uart:
	make -C $(PRJPATH)/uart
	
uart_clean:
	make -C $(PRJPATH)/uart clean

.PHONY: clk 	
clk:
	make -C $(PRJPATH)/clk
	
clk_clean:
	make -C $(PRJPATH)/clk clean

.PHONY: msg 	
msg:
	make -C $(PRJPATH)/msg
	
msg_clean:
	make -C $(PRJPATH)/msg clean
	
.PHONY: ltai 	
ltai:
	make -C $(PRJPATH)/ltai
	
ltai_clean:
	make -C $(PRJPATH)/ltai clean

clean:  arch_clean int_clean mem_clean std_clean task_clean usr_clean sem_clean uart_clean clk_clean msg_clean ltai_clean
	rm -rf $(OUTPUTPATH)/
	mkdir $(OUTPUTPATH)/
