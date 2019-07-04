BASE = ../../../
FCMAKE = C:/Program Files/FITkit/bin/fcmake.exe
FKFLASH = C:/Program Files/FITkit/bin/fkflash.exe
FKTERM = C:/Program Files/FITkit/bin/fkterm.exe
FKTEST = C:/Program Files/FITkit/bin/fktest.exe
PROJECT = project.xml
OUTPUTPREFIX = robot
FPGACHIP = xc3s50
FPGASPEEDGRADE = 4
FPGAPACKAGE = pq208

all: dependencycheck build/robot_f1xx.hex build/robot_f2xx.hex build/robot.bin

#MCU part
#=====================================================================
HEXFILE_F1XX = build/robot_f1xx.hex
HEXFILE_F2XX = build/robot_f2xx.hex

INCLUDE_ARGS = -Irtos/include -Imcu -Irtos/portable/msp430

build/mcu/keyboard_f1xx.o: ../../../mcu/libs/keyboard/keyboard.c
	$(comp_tpl_f1xx)

build/mcu/keyboard_f2xx.o: ../../../mcu/libs/keyboard/keyboard.c
	$(comp_tpl_f2xx)

build/mcu/display_f1xx.o: ../../../mcu/libs/lcd/display.c
	$(comp_tpl_f1xx)

build/mcu/display_f2xx.o: ../../../mcu/libs/lcd/display.c
	$(comp_tpl_f2xx)

build/mcu/tasks_f1xx.o: rtos/tasks.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f1xx)

build/mcu/tasks_f2xx.o: rtos/tasks.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f2xx)

build/mcu/list_f1xx.o: rtos/list.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f1xx)

build/mcu/list_f2xx.o: rtos/list.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f2xx)

build/mcu/queue_f1xx.o: rtos/queue.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f1xx)

build/mcu/queue_f2xx.o: rtos/queue.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f2xx)

build/mcu/heap_2_f1xx.o: rtos/portable/memory/heap_2.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f1xx)

build/mcu/heap_2_f2xx.o: rtos/portable/memory/heap_2.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f2xx)

build/mcu/port_f1xx.o: rtos/portable/msp430/port.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f1xx)

build/mcu/port_f2xx.o: rtos/portable/msp430/port.c rtos/include/FreeRTOS.h mcu/FreeRTOSConfig.h rtos/include/projdefs.h rtos/include/portable.h rtos/portable/msp430/portmacro.h rtos/include/task.h rtos/include/list.h rtos/include/croutine.h rtos/include/queue.h
	$(comp_tpl_f2xx)

build/mcu/main_f1xx.o: mcu/main.c mcu/FreeRTOSConfig.h
	$(comp_tpl_f1xx)

build/mcu/main_f2xx.o: mcu/main.c mcu/FreeRTOSConfig.h
	$(comp_tpl_f2xx)

OBJFILES_F1XX = build/mcu/keyboard_f1xx.o build/mcu/display_f1xx.o build/mcu/tasks_f1xx.o build/mcu/list_f1xx.o build/mcu/queue_f1xx.o build/mcu/heap_2_f1xx.o build/mcu/port_f1xx.o build/mcu/main_f1xx.o
OBJFILES_F2XX = build/mcu/keyboard_f2xx.o build/mcu/display_f2xx.o build/mcu/tasks_f2xx.o build/mcu/list_f2xx.o build/mcu/queue_f2xx.o build/mcu/heap_2_f2xx.o build/mcu/port_f2xx.o build/mcu/main_f2xx.o

#FPGA part
#=====================================================================
BINFILE = build/robot.bin
HDLFILES  = ../../../fpga/units/clkgen/clkgen_config.vhd
HDLFILES += ../../../fpga/units/clkgen/clkgen.vhd
HDLFILES += ../../../fpga/units/math/math_pack.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_adc_entity.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_adc.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_adc_autoincr.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_reg.vhd
HDLFILES += ../../../fpga/ctrls/spi/spi_ctrl.vhd
HDLFILES += ../../../fpga/chips/fpga_xc3s50.vhd
HDLFILES += ../../../fpga/chips/architecture_bare/arch_bare_ifc.vhd
HDLFILES += ../../../fpga/chips/architecture_bare/tlv_bare_ifc.vhd
HDLFILES += ../../../fpga/ctrls/lcd/lcd_raw.vhd
HDLFILES += ../../../fpga/ctrls/keyboard/keyboard_ctrl.vhd
HDLFILES += ../../../fpga/ctrls/keyboard/keyboard_ctrl_high.vhd
HDLFILES += fpga/top_level.vhd

build/robot.bin: build/fpga/robot.par.ncd build/fpga/robot.pcf

PKGS_LIST = ../../../mcu/libs/keyboard/package.xml
PKGS_LIST += ../../../mcu/libs/lcd/package.xml
PKGS_LIST += rtos/package_msp430.xml
PKGS_LIST += ../../../fpga/units/clkgen/package.xml
PKGS_LIST += ../../../fpga/units/math/package.xml
PKGS_LIST += ../../../fpga/ctrls/spi/package.xml
PKGS_LIST += ../../../fpga/chips/architecture_bare/package.xml
PKGS_LIST += ../../../fpga/ctrls/lcd/package.xml
PKGS_LIST += ../../../fpga/ctrls/keyboard/package.xml

include $(BASE)/base/Makefile.inc
