twConvertFirmware2c.exe is a c utility to convert the Timberwolf devices (Zl38040/5x/6x/8x) firmware image (.s3) and config files (.cr2) into a *.c and/or .bin output.

  
This utility gives user a flexibility to generate image with different blocksizes. 16 to 128 WORDS for firmware image and 1-128 for configuration record.
i.e. whole image will be written to device in the block size as mentioned by user in command line arguments.



Usage: 
twConvertFirmware2c.exe -i <input file> -o <output file> -b <block_size> -f <firmware code>

Note: 
1) -f option is required only for firmware image conversion. Configuration record doesnt need  this.
2) Bin output is supported only for firmware image. Configuration record still supports .C/.h ONLY output.

Example:

To see usage menu, run
twConvertFirmware2c.exe -h


for generating C-image with block size of 128 words
twConvertFirmware2c.exe -i zl38040_firmware.s3 -o zl38040_firmware.c -b 128 -f zl38040

for creating bin image 
twConvertFirmware2c.exe -i zl38040_firmware.s3 -o zl38040_firmware.bin -b 128 -f zl38040

for converting config record
twConvertFirmware2c.exe i zl38040_config.cr -o zl38040_config.c
