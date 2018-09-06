#include "mbed.h"
#include "FlashIAP.h"
#include "mbed.h"

#define DBG 1
DigitalOut led1(LED1);
DigitalOut led2(LED2);
#if DBG
Serial pc(USBTX, USBRX);
#endif

// main() runs in its own thread in the OS
void flashiap_init_test()
{
    FlashIAP flash_device;
    flash_device.init();    
    flash_device.deinit();
}

//#define MAX(x, y) (((x) > (y)) ? (x) : (y))
uint32_t flashiap_program_test()
{
    FlashIAP flash_device;
    uint32_t ret = flash_device.init();

    // get the last sector size (flash size - 1)
    uint32_t sector_size = flash_device.get_sector_size(flash_device.get_flash_start() + flash_device.get_flash_size() - 1UL);
    uint32_t page_size = flash_device.get_page_size();
    uint32_t prog_size = MAX(page_size, (uint32_t)8);
    uint8_t *data = new uint8_t[prog_size + 2];
    for (uint32_t i = 0; i < prog_size + 2; i++) {
        data[i] = i;
    }
    uint32_t address;
    // the one before the last sector in the system
    for(int i = 0; i < 10; i++)
    {
    	address = flash_device.get_flash_start() + (sector_size * i);
#if DBG
		pc.printf("address = 0x%x\n\r",address);
#endif        
		ret = flash_device.erase(address, sector_size);
    }


    for (uint32_t i = 0; i < sector_size / prog_size; i++) {
        uint32_t prog_addr = address + i * prog_size;
        ret = flash_device.program(data, prog_addr, prog_size);
    }

    uint8_t *data_flashed = new uint8_t[prog_size];
    for (uint32_t i = 0; i < sector_size / prog_size; i++) {
        uint32_t page_addr = address + i * prog_size;
        ret = flash_device.read(data_flashed, page_addr, prog_size);
        
    }

    // check programming of unaligned buffer and size
    ret = flash_device.erase(address, sector_size);
    ret = flash_device.program(data + 2, address, prog_size);
    ret = flash_device.read(data_flashed, address, prog_size - 1);
    

    delete[] data;
    delete[] data_flashed;

    ret = flash_device.deinit();
    return ret;
}


int main()
{
#if DBG
	pc.baud(115200);
	pc.printf("START \n\r");
#endif    
    led2 = !led2;
    led2 = !led2;
    wait(2);
    
    flashiap_init_test();
    flashiap_program_test();
    led2 = !led2;
    while (true)
    {
        led1 = !led1;
        //led2 = !led2;
#if DBG
        pc.printf("LED \n\r");
#endif
        wait(1);
    }
}




