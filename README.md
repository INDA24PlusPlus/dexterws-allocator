# Freelist Allocator
First Fit Freelist Allocator implementerad i C.

## API
Importera headerfilen 'freelist.h' för att använda allocatorn.

```c
void *malloc(int64_t size);
void free(void *ptr);
void *realloc(void *ptr, int64_t size);
```

## Limitationer
Allocatorn "free:ar" inte minnet på riktigt, all minne som har begärts från OS:et kommer att behållas tills programmet avslutas.

Allt minne fragmenteras också, vilket kan leda till att programmet kräver mer och mer minne
desto mer minne som allokerats (givet att olika storlekar allokeras och deallokeras). Jag hade lite
idéer på hur jag skulle defragmentera minnet, men jag hade dessvärre inte tid att implemetera detta just nu.

