.global put32
put32:
    // 將 W1 寫入 X0 指向的記憶體地址
    str w1, [x0]
    ret

.global get32
get32:
    // w0 = get32(x0)
    ldr w0, [x0]
    ret

.globl delay
delay:
	subs x0, x0, #1
	bne delay
	ret

// w : 32 bit