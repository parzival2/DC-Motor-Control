# DC_Motor_Control
Control of DC motor using the encoder and STM32F103 using Chibios

## Setting up
The STM32F103(Bluepill) has been used to achieve this project. It can either be programmed using [JTAG](https://www.aliexpress.com/i/1619197946.html?spm=2114.12057483.0.0.11a979f8zSeFl3) or can be done using UART.

The pinout of the cheap JTAG connector can be seen here

![alt text](Resources/JTAG_Pinout_AliEx.jpg "JTAG connector from AliExpress")

and the pinout of the SWD pins of the Bluepull can be seen here

![alt text](Resources/bluepill_jtag_pinout.png "Bluepill SWD pinout"). Connecting each pin to their respective other you should be able to program the board.

| ST-LinkV2        Bluepill |
|---------------------------|
| 3V3        ->      3V3    |
| T_SWCLK    ->     SWCLK   |
| T_SWDIO    ->     SWDIO   |
| GND        ->      GND    |

---
**NOTE**

It is important to note that the Chinese clones of Bluepill has a bootloader on them that prevents them to be programmed using SWD. But it can be easily overcome by connecting the 
`T_JRST` to `Reset` pin on Bluepill.

![alt test](Resources/Reset_Pin.jpg "Reset pin on the Bluepill")

---

## IDE
The main IDE that I have used for the project is [VSCode](https://code.visualstudio.com/) as it has support for debugging cortex microcontrollers using the [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) extension and also it is fast. 

## Debuggin
OpenOCD has been used for debugging. If you run into problems with OpenOCD saying that `Cannot halt device ..` then you might be using the cheap chinese STM32 microcontroller which have a different ID. If you look into [bluepill_openocd.cfg](bluepill_openocd.cfg) config file then you can see that I am also using a custom config file which contains the modified device ID.
You can copy and paste the contents of `target/stm32f1x.cfg` into a new file and modify the line 


```
      # this is the SW-DP tap id not the jtag tap id
      set _CPUTAPID 0x1ba01477
```

to

```

      # this is the SW-DP tap id not the jtag tap id
      set _CPUTAPID 0x2ba01477

```