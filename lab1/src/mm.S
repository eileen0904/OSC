.globl memzero

memzero:
    str xzr, [x0], #8  // 存入 0 到 [X0]，然後 X0 += 8
    sub x1, x1, #8     // 減少已清除的大小
    b.gt memzero
    ret 