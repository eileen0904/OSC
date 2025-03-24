from serial import Serial
from pwn import *
import argparse
from sys import platform
import time

if platform == "linux" or platform == "linux2":
    parser = argparse.ArgumentParser(description='NYCU OSDI kernel sender')
    parser.add_argument('--filename', metavar='PATH', default='./kernel/kernel8.img', type=str, help='path to kernel8.img')
    parser.add_argument('--device', metavar='TTY',default='/dev/ttyUSB0', type=str,  help='path to UART device')
    parser.add_argument('--baud', metavar='Hz',default=115200, type=int,  help='baud rate')
    args = parser.parse_args()

    with open(args.filename,'rb') as fd:
        with Serial(args.device, args.baud) as ser:

            kernel_raw = fd.read()
            length = len(kernel_raw)
            print("Kernel image size : ", hex(length))
            ser.write(length.to_bytes(4, byteorder='big'))
            ser.flush()

            print("Start sending kernel image by uart1...")
            for i in range(length):
                # Use kernel_raw[i: i+1] is byte type. Instead of using kernel_raw[i] it will retrieve int type then cause error
                ser.write(kernel_raw[i: i+1])
                ser.flush()
                print(kernel_raw[i: i+1]," ",ser.read(1))
                if i % 100 == 0:
                    time.sleep(0.05)
                    # print("{:>6}/{:>6} bytes".format(i, length))
            print("{:>6}/{:>6} bytes".format(length, length))
            print("Transfer finished!")