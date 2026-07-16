"""
烧录字库位图到 W25Q64 (通过串口)

用法:
  1. 确保 MCU 已烧录并运行
  2. 修改下方 COM_PORT 为实际串口号
  3. 运行: python burn_font_to_flash.py

协议:
  PC -> MCU: [0xA5][addr_4B_BE][len_2B_BE][data...][crc8]
  MCU -> PC: [0x06]=OK  [0x15]=ERR
"""

import serial
import struct
import time
import os

# ===== 配置 =====
COM_PORT = 'COM17'          # 修改为你的串口号
BAUD = 115200
BIN_FILE = r'd:\STM32\LVGL\docs\lv_font_hanzi_16_bitmap.bin'
FLASH_ADDR = 0x000000      # W25Q64 目标地址
CHUNK_SIZE = 2048          # 每包字节数
# ================

def crc8(data):
    """简单的 XOR checksum"""
    c = 0
    for b in data:
        c ^= b
    return c & 0xFF

def send_chunk(ser, addr, data):
    """发送一个数据块"""
    header = struct.pack('>BIH', 0xA5, addr, len(data))
    packet = header + data + bytes([crc8(data)])
    ser.write(packet)
    ser.flush()
    
    # 等待响应
    resp = ser.read(1)
    if resp == b'\x06':
        return True
    elif resp == b'\x15':
        return False
    else:
        print(f'  Unexpected response: {resp.hex()}')
        return False

def main():
    file_size = os.path.getsize(BIN_FILE)
    print(f'File: {BIN_FILE}')
    print(f'Size: {file_size:,} bytes ({file_size/1024:.1f} KB)')
    print(f'Target: W25Q64 @ 0x{FLASH_ADDR:06X}')
    print(f'Port: {COM_PORT} @ {BAUD}')
    print()
    
    print(f'Opening {COM_PORT}...')
    ser = serial.Serial(COM_PORT, BAUD, timeout=2)
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    # 等待 MCU 进入烧录模式（读空 printf 输出，等待握手）
    print('Waiting for MCU burn mode...')
    # 先清空缓冲区中的 printf 残留数据
    time.sleep(0.5)
    ser.reset_input_buffer()

    # 反复发送 ping 直到收到 0xAA 响应
    max_retry = 100
    for i in range(max_retry):
        ser.write(b'\x55')
        time.sleep(0.05)
        if ser.in_waiting:
            resp = ser.read(1)
            if resp == b'\xAA':
                print('MCU ready!')
                break
        if i % 10 == 0:
            print(f'  Retry {i}/{max_retry}...')
    else:
        print(f'ERROR: MCU not responding after {max_retry} retries')
        ser.close()
        return

    # 握手后清空缓冲区 (MCU 的 printf 输出可能残留在 RX 缓冲区)
    time.sleep(0.1)
    ser.reset_input_buffer()

    # 发送 FLASH_ADDR 和总长度
    info = struct.pack('>II', FLASH_ADDR, file_size)
    info_crc = crc8(info)
    print(f'  INFO: addr=0x{FLASH_ADDR:06X} size={file_size}')
    ser.write(b'\xA0' + info + bytes([info_crc]))
    ser.flush()
    ser.timeout = 10  # 擦除可能需要几秒
    resp = ser.read(1)
    ser.timeout = 2
    if resp:
        print(f'  MCU resp: 0x{resp[0]:02X}')
    else:
        print(f'  MCU resp: (timeout)')
    if resp != b'\x06':
        print('ERROR: MCU rejected flash info')
        ser.close()
        return
    
    # 逐块发送
    with open(BIN_FILE, 'rb') as f:
        offset = 0
        total = file_size
        addr = FLASH_ADDR
        
        while offset < total:
            chunk = f.read(CHUNK_SIZE)
            if not chunk:
                break
            
            progress = (offset + len(chunk)) * 100 // total
            print(f'\r  Sending: {offset+len(chunk):,}/{total:,} ({progress}%)', end='')
            
            if not send_chunk(ser, addr, chunk):
                print(f'\nERROR at offset {offset}')
                ser.close()
                return
            
            offset += len(chunk)
            addr += len(chunk)
            time.sleep(0.01)  # 给MCU写入时间
    
    print(f'\n\nDone! {total:,} bytes written to W25Q64 @ 0x{FLASH_ADDR:06X}')
    
    # 等待 MCU 完成最后的写入
    time.sleep(0.5)
    
    # 验证
    print('Verifying...')
    ser.write(b'\xA1')  # verify command
    resp = ser.read(1)
    if resp == b'\x06':
        print('Verify OK!')
    else:
        print(f'Verify FAILED (got {resp.hex() if resp else "nothing"})')
    
    ser.close()

if __name__ == '__main__':
    main()
