app: bsp
	make -C app

bsp: app_bsp/Makefile
	make -C app_bsp

app_bsp/Makefile:
	cd app_bsp ; nios2-bsp-generate-files --bsp-dir . --settings settings.bsp

create-bsp: app_bsp/Makefile

clean:
	make -C app clean
	make -C app_bsp clean
	rm app_bsp/Makefile