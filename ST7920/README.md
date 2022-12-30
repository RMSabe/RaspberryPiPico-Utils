These are some codes I made to control the ST7920 Graphic Display Driver.
I made the same code in 3 versions:

Dynamically Allocated Buffer: each display has its own buffer. Buffers can be allocated and released to optimize memory space.

Statically Allocated Multi-Buffer: each display has its own buffer. Buffers cannot be released, memory usage is constant.

Statically Allocated Single-Buffer: same buffer is shared for all displays.

Usage suggestion: 
If you're running RTOS, I suggest you use the dynamically allocated buffer code.
If you're running baremetal with lots of memory, I suggest you use the statically allocated multi-buffer code.
If you're running baremetal with little memory, I suggest you use the statically allocated single-buffer code.

I'm not a professional developer. I made these codes just for fun. Don't expect professional performance from them.

Author: Rafael Sabe
Email: rafaelmsabe@gmail.com
