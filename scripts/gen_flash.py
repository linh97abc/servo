import struct
import os
import tempfile
import sys
from pathlib import Path

NIOS2_EDS = os.environ["SOPC_KIT_NIOS2"]

if not NIOS2_EDS:
    sys.exit("Can not find quartus path, please set variable NIOS2_EDS")

ELF2FLASH = os.path.join(
    NIOS2_EDS, "bin", "elf2flash.exe")
SREC_CAT_EXE = os.path.join(Path(__file__).parent / "srec_cat.exe")

class GenFlash():
    SIGNATURE = 0xa5a5a5a5

    def __init__(self):
        self.version = 0
        self.timestamp = 0
        self.res1 = 0
        self.res2 = 0
        self.data_len = 0
        self.data_crc = 0
        self.header_crc = 0

    @staticmethod
    def CalcCRC32(buff: bytes):
        crcval = 0xffffffff

        for cval in buff:
            crcval ^= cval

            for _ in range(8):
                crcval = ((crcval >> 1) ^ 0xEDB88320) if (crcval & 0x00000001) else (crcval >> 1)
        
        return crcval


    def PackHeader(self):
        header = struct.pack('IIIIIII',
                        __class__.SIGNATURE, 
                        self.version, 
                        self.timestamp,
                        self.data_len, 
                        self.data_crc, 
                        self.res1,
                        self.res2)
        header_crc = __class__.CalcCRC32(header)
        return header + struct.pack('I', header_crc)

    def Gen(self, elf: str, imgHex: str, offset: str):
        sw_bytes = None
        
        with tempfile.TemporaryDirectory() as tmpDir:
            tmpBin = os.path.join(tmpDir, "sw.bin")

            
            tmpFlash = os.path.join(tmpDir, "sw.flash")

            elf_fix = elf.replace("\\", "/")
            tmpFlash = tmpFlash.replace("\\", "/")
            cmd = f"{ELF2FLASH} --input={elf_fix} --output={tmpFlash} --epcs"
            print(f"--cmd: {cmd}")
            os.system(cmd)

            cmd = SREC_CAT_EXE + f" {tmpFlash} -Motorola -o {tmpBin} -Binary"
            print(f"--cmd: {cmd}")
            os.system(cmd)

            with open(tmpBin, 'rb') as f:
                sw_bytes = f.read()
        
            self.data_len = len(sw_bytes)
            self.data_crc = __class__.CalcCRC32(sw_bytes)

            img_bytes = self.PackHeader() + sw_bytes

            imgBin = os.path.join(tmpDir, "img.bin")
            with open(imgBin, 'wb') as f:
                f.write(img_bytes)

            cmd = SREC_CAT_EXE + f" {imgBin} -Binary -offset {offset} -o {imgHex}.flash -Motorola -address-length=4"
            print(f"--cmd: {cmd}")
            os.system(cmd)

            bootCfgBin = os.path.join(tmpDir, "bootCfgBin.bin")
            with open(bootCfgBin, 'wb') as f:
                f.write(struct.pack('I', 0x400000))

            cmd = SREC_CAT_EXE + f" {bootCfgBin} -Binary -offset 0x006F0000 -o {imgHex}.cfg.flash -Motorola -address-length=4"
            print(f"--cmd: {cmd}")
            os.system(cmd)


if __name__ == "__main__":
    elf = sys.argv[1]
    imgHex = sys.argv[2]
    offset = sys.argv[3]

    GenFlash().Gen(elf, imgHex, offset)
