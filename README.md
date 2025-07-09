# rle.c
RLE image format with Gimp support for 8-bit CPU

DESCRIPTION
-----------
Half of century ago, run-length encoding method for (lossless) image compression was the most obvious routine. It offer lowest possible extra RAM usage (often zero, CPU registers were enough), fastest speed, least code size, compatibility and every platform support, even most tiny available.

As the palette sizes grows (EGA, VGA, TrueColor...), it become less efficient; was mostly replaced by `.gif` format on powerful platforms. While on tiny systems, and especially for simple images like logos and letters, with well limited palette, it still almost only one option.

However, i am wonder why no any simple storage format defined for it yet. There is `.pcx` format, and while it was fitted well with top platforms of that era (like 80286), it is too "fat" for low-power CPU like modern 8-bit AVR.

So this, simplest possible, `.rle` format was born.

It have the intentional property (requirement) that user will write own decoding routine for favorite language and target CPU and memory map: it is simple, and everybody prefer to have own code when it is simple. Usually our C decoder used for reference and tests.

Format optimized to be effectively used with 8-bit machine instructions. <br> Note that it will be very fast decompression to videoRAM, even faster than just bare `memcpy` of uncompressed bitmap. Probably it is close to be fastest possible drawing. So it may be worth to use `.rle` even when it gives less compression than `.gif` or `.png`.

We offer mandatory encoder and (optional) decoder for `GIMP`, to make use of this format pleasant. Tested with `GIMP 2.x` family, include up to `2.10.38`.

Here are specs:
--------------

| _ | _ |
|------------------|---|
_Target_ | Lowest end CPUs with tiny RAM, like ATtiny, ATmega 
_Min image size_ | 0
_Max image size_ | 256x256
_Impossible size_ | 255 px width or height (can be forced though)
_Min colors_ | 1+1 (corresponds to bits per pixel (bpp): 1)
_Max colors_ | 16+1 (corresponds to bits per pixel (bpp): ~4) or even more
_Palette_ | Not defined, should be separately handled at decoding [Note]
_Transparency_ | Not defined, should be separately handled at decoding, easily obtainable using user's palette support. [Note]
_Tolerance to empty Flash ROM_ | Filled with 0xFF's at the beginning or somewhere in the middle of file, meant end of data but not produce decoding overflows etc. That means none of bytes of file can be 0xFF.
_Stream interrupt_ | Supported and normal (not required to have all pixels to be defined in file).

>[!NOTE]
>For Gimp, palette considered to be grayscale.

File format:
-----------
Header and data flow. **FourCC** compliant.
* Header is 8 bytes:  `'RLE0'`(`w`)(`h`)(`c`)(`sf`), where:
  - `0` is format version;
  - `w` is 1-byte width (0 is 256 px),
  - `h` is 1-byte height (0 is 256 px),
  - `c` is 1-byte color quantity (2, 3, 5, 9, 17, maybe 33 or even 65),
  - `sf` is 1-byte `0bF0SSSSSS`,
    * `S`=start color (0..`c`-1), and
    * `F`=1 means it is the only color in image, it is filled with S color (or fully transparent). Then no data is followed. <br> Else (if `F`=0), following is...
* ...data flow of lengths of equal pixel lines, and entire flow is ending with `0xFF`. <br> For `c`=2 colors, and start color `S`=0, data flow `05 10 15 255` means 5 black pixels, 10 white, 15 black. Remaining image area considered transparent (no writing to videoRAM, skip). <br> No any byte is allowed to be `0xFF` before end, include header: so 255 px width or height is not possible. Line of 255 equal pixels (need to be `0xFF` in stream) replaced to something like `0xFE 0x00 0x01`, which is same at decoding. <br> For `c`=3 colors, data is flow of: `0bCLLLLLLL`: `L`=length 0..126, `C`=color delta. (again, `L`=127 is not possible and will be replaced with equivalent.) <br> For `c`=5 colors, data is flow of: `0bCCLLLLLL`; etc. <br> When length is 0, color delta is ignored.
* 
When something is not described here, use simplest stuff / KISS rule.

Efficiency
----------
This `.rle` format is most effective on **1-bit** (`c`=2) images.
Let's compare it with modern 1-bit `.gif` and `.png` formats.

Let's take some middle complexity image.
`mesh.png` is 1-bit, **1147** bytes image;

`.gif` is **808** bytes, 

and `.rle` is **755** bytes.

And it can be noted that, while `.png` and `.gif` are almost can not be (losslessly) compressed even more, but our `.rle` can be zipped to as tiny as **250** bytes. However, of course, we can't use this benefit, because we can't unzip with ATtiny (at least, should not to plan that). So it is more like fun fact, and somewhat answer to general
> Can images be losslessly compressed even more than popular .gif and .png offers?

question.

USAGE
-----
Our C code offers coder and decoder from (to) `.rle` file using GIMP. It will not run as standalone app. Please let me know if one need command-line coder or decoder.

Our code adds `.rle` format support to GIMP, so allows GIMP to open, save and convert from (to) other formats, like well known `.gif,` `.png`'s.

It should be as simple as

    gimptool-2.0 --install rle.c

Then use `gimp` to work with our test image, or convert other images to `.rle` using `Ctrl+Shift+e` then state `.rle` file extension. Note that user should be familiar with 1-bit or low-bit images, and know exactly what Gimp's `Image - Mode - Indexed - Use 1-bit palette` does, and what is the purpose of it.

CREDITS
-------
Our code based on examples from (sorry, the URLs itself are lost with years...):
Copyright David Neary, 2004; 
Copyright Nathan Osman, 2012.

LICENSE
-------
This instruction manual for `rle.c` is licensed under Creative Commons Attribution 4.0. You are welcome to contribute to the manual in order to improve it so long as your contributions are made available under this same license.

`rle.c` is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
