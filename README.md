# TUFF Tester User’s Guide

The TUFF tester is a simple program on an MSP-EXP430F5529LP. It’s written using Energia, an Arduino port for the MSP430 series. Therefore, the code could actually be used on any board supported by the Arduino or Energia IDEs. It uses 1 serial port, and the “CmdArduino” library, from 

https://github.com/fakufaku/CmdArduino

Plugging in the MSP-EXP430F5529 should produce 2 USB Serial Ports on the computer. If the program you use to connect to the 
serial port can view the 'port names' as presented by the USB device, the desired serial port will show up as "MSP Application UART."
Otherwise it should be the second USB serial port created. The first is for talking to the built-in programmer on the board.
Open the serial port using 9600 bps, 8N1. If you use a program like Tera Term Pro (the good one, from 
https://ttssh2.osdn.jp/index.html.en ), these should be the defaults anyway.

## Connecting to the TUFF

The TUFF/MINITUFF connections are:

|  1   |  2   |  3	|  4	|  5	|  6	|   7  |  8  |  9   |  10 |
|------|------|-----|-----|-----|-----|------|-----|------|-----|
|+BIASV| 	GND |	+5V | GND |	CLK |	GND |	DATA | GND |#RESET| GND |

The last 2 pins are not present on the MINITUFF.

Connect:
* Pin 2/3 to GND/+5V.
* Pin 5 to P2.5 on the MSP-EXP430F5529.
* Pin 6 to GND (above P2.0) on MSP-EXP430F5529.
* Pin 7 to P2.4 on the MSP-EXP430F5529.

You can use any pins to connect GND, all the grounds are common. This connection scheme leaves 1 space (pin 4) 
between power connections and the data connections, so it makes it a bit easier.

+BIASV only goes to the bias-tee and out the input side, through the Coilcraft broadband chokes. Just to repeat, don't forget
if you apply power to +BIASV, there will be DC voltage at the input connector! The Coilcraft chokes can handle multiple amps
of current, and have practically zero resistance (0.0151 ohms).

**Note** : using the 'wire bundle cable' shipped with the MINITUFFv2, the colors are yellow = clock, orange = data, green = ground.

## TUFF Tester Command Line

Push the RST button on the MSP-EXP430F5529. On the serial port, you should see:

```
** TUFF Command Line Tester Ready **
TUFF>>
```

## Command Overview

The TUFF tester has 8 commands:

* on: Turns on a notch.
* off: Turns off a notch.
* update: Changes the capacitor setting for a notch. Also can update the setting in flash so that it’s the default.
* reset: Resets the TUFF. This reloads all switches to their ‘default’ flash values, and turns off all notches.
* synchronize: This synchronizes the TUFF tester/TUFF interface.
* cmd: Sends arbitrary command to TUFF. For testing.
* clock: Sets/clears the clock line. For testing.
* data: Sets/clears the data line. For testing.

"help" displays all the available commands. All commands that take an option (everything except reset/synchronize) 
also display their usage when entered alone (e.g. "on" by itself gives syntax).

## Initialization

After connecting to the TUFF tester, do:

1.	reset
2.	reset
3.	synchronize

The 2 resets are required to make sure that the TUFF actually gets the 'reset' command. This puts it in a 'locked' state where it 
accepts no commands – however, at this point the interface is known to be reset and at bit 0. The 'synchronize' command sends a 
magic command (0xD00D) to the TUFF so that it can know that the commander (the TUFF tester) and the TUFF are starting at bit 0. 
Once the TUFF sees 0xD00D, it unlocks the interface and is ready for commands.

In theory, disconnecting the TUFF tester and reconnecting it should not require doing "reset/reset/synchronize" again, however if 
any spurious clocks are seen on the CLK line, the TUFF will get out of sync. In practice I’ve found it necessary to do a
"reset/reset/synchronize" when disconnecting/reconnecting the TUFF tester.

## Turning on notches

The 'on' command takes 2 arguments: the channel (1-6) and the notch number (1-3). 
The MINITUFFv2 has only 2 channels (1 and 2).
So turning on notch 2 of channel 1 would be:

```
TUFF>> on 1 2
Sending cmd 1120
```

(The command will change depending on the notch and channel number).

## Turning off notches

The 'off' command is exactly the same as the ‘on’ command. So again, turning off notch 3 of channel 2 would be:

```
TUFF>> off 2 3
Sending cmd 2200
```

## Updating notch settings

The 'update' command takes at least 3 arguments, and has a 4th optional argument. The required arguments are channel (1-6), 
notch (1-3) and setting (0-31).

If a 4th argument is present, and has a non-zero value, that setting will be stored into flash as the new default.

So to update notch 1 to setting 14, it would be:

```
TUFF>> update 1 1 14
Sending cmd 104E
```

To update notch 2 to setting 19 and store it in flash, it would be:

```
TUFF>> update 1 2 19 1
Sending cmd 1173
```
