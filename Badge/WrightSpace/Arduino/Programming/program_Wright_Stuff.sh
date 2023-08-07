# You will probably have to select/rename the correct port based upon your UART device


esptool.py --chip esp32s2 --port /dev/cu.usbserial-14201 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 WrightStuff.ino.bootloader.bin 0x8000 WrightStuff.ino.partitions.bin 0xe000 boot_app0.bin 0x10000 WrightStuff.ino.bin