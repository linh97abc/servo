app: bsp
	make -C servo

bsp: app_bsp/Makefile
	make -C servo_bsp

app_bsp/Makefile:
	cd app_bsp ; nios2-bsp-generate-files --bsp-dir . --settings settings.bsp

create-bsp: app_bsp/Makefile

clean:
	make -C servo clean
	make -C servo_bsp clean
	rm servo_bsp/Makefile