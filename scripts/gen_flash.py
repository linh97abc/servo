import argparse
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

SOF2FLASH = os.path.join(
    NIOS2_EDS, "bin", "sof2flash.exe")

SREC_CAT_EXE = os.path.join(Path(__file__).parent / "srec_cat.exe")

class GenFlash():
    SIGNATURE = 0xa5a5a5a5

    def __init__(self, name: str, sof: str, elf: str, sw_offset: str, fw_offset: str):
        self.version = 0
        self.timestamp = 0
        self.res1 = 0
        self.res2 = 0
        self.data_len = 0
        self.data_crc = 0
        self.header_crc = 0
        self.name = name
        self.sof = sof
        self.elf = elf
        self.sw_offset = sw_offset
        self.fw_offset = fw_offset

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

    def Gen(self, out_dir: str):
        sw_bytes = None
        
        imgHex = os.path.join(out_dir, self.name)
        
        with tempfile.TemporaryDirectory() as tmpDir:
            if self.sof:
                tmpBin = f"{imgHex}.fw.bin"
                tmpFlash = ""

                if self.fw_offset > 0:
                    tmpFlash = os.path.join(tmpDir, "fw.flash")
                else:
                    tmpFlash = f"{imgHex}.fw.flash"

                sof_fix = self.sof.replace("\\", "/")
                tmpFlash = tmpFlash.replace("\\", "/")
                cmd = f"{SOF2FLASH} --input={sof_fix} --output={tmpFlash} --epcs"
                print(f"--cmd: {cmd}")
                os.system(cmd)

                cmd = SREC_CAT_EXE + f" {tmpFlash} -Motorola -o {tmpBin} -Binary"
                print(f"--cmd: {cmd}")
                os.system(cmd)

                if self.fw_offset > 0:
                    cmd = SREC_CAT_EXE + f" {imgBin} -Binary -offset {self.fw_offset} -o {imgHex}.fw.flash -Motorola -address-length=4"
                    print(f"--cmd: {cmd}")
                    os.system(cmd)

            if self.elf:
                tmpBin = os.path.join(tmpDir, "sw.bin")
                tmpFlash = os.path.join(tmpDir, "sw.flash")

                elf_fix = self.elf.replace("\\", "/")
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

                imgBin = os.path.join(imgHex + ".sw.bin")
                with open(imgBin, 'wb') as f:
                    f.write(img_bytes)

                cmd = SREC_CAT_EXE + f" {imgBin} -Binary -offset {self.sw_offset} -o {imgHex}.sw.flash -Motorola -address-length=4"
                print(f"--cmd: {cmd}")
                os.system(cmd)

            bootCfgBin = os.path.join(tmpDir, "bootCfgBin.bin")
            with open(bootCfgBin, 'wb') as f:
                f.write(struct.pack('II', self.sw_offset, self.fw_offset))

            cmd = SREC_CAT_EXE + f" {bootCfgBin} -Binary -offset 0x006F0000 -o {imgHex}.cfg.flash -Motorola -address-length=4"
            print(f"--cmd: {cmd}")
            os.system(cmd)


def parse_args():
    parser = argparse.ArgumentParser(allow_abbrev=False)

    parser.add_argument("--fw-offset",
                        default="0",
                        help="Firmware offset in flash")
    parser.add_argument("--sw-offset",
                        default="0",
                        help="Software offset in flash")
    parser.add_argument("--sof",
                        help="Sof file")
    parser.add_argument("--elf",
                        help="Elf file")
    parser.add_argument("--name",
                        default="fpga",
                        help="Elf file")
    parser.add_argument("out",
                        help="Output directory")

    return parser.parse_args()

if __name__ == "__main__":
    args = parse_args()

    sw_offset = int(args.sw_offset, 16)
    fw_offset = int(args.fw_offset, 16)

    GenFlash(args.name, args.sof, args.elf, sw_offset, fw_offset).Gen(args.out)
