# Coding-Challenge---RISC-V-ACT-Framework-Enablement-and-M-Mode-Firmware-Validation-on-Hardware-Board

The code detects incoming data, and prints on output.

I have use Linux Mint Cinannom for running this. I have tried kepping a USB device and tried to recieve data.
As I have not used MCU like ESP, code cannot open a USB port and so I recieved output as No port detected.

The commands to run in linux terminal are as follows

gcc serial_test.c -o serial_test #to compile the code
sudo ./serial_test #to run the code

The result is as follows

<img width="815" height="70" alt="Screenshot 2026-05-16 132330" src="https://github.com/user-attachments/assets/240654a9-ec71-4246-b27a-da41e300b2b4" />
