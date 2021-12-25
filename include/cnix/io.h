#ifndef _CNIX_IO_H
#define _CNIX_IO_H

static inline char inb(short p)
{
        char r;
        __asm__ __volatile__ (
                "inb  %%dx, %%al \n\t" :"=a"(r) :"d"(p)
        );
        return r;
}
#define outb(_p,_v)                     \
        __asm__ __volatile__ (          \
                "outb  %%al, %%dx \n\t" \
                "mfence           \n\t" \
                ::"a"(_v),"d"(_p)       \
        );
#define inw(_r,_p)                      \
        __asm__ __volatile__ (          \
                "inw  %%dx, %%ax \n\t"  \
                "mfence          \n\t"  \
                :"=a"(_r)               \
                :"d"(_p)                \
        );
#define inl(_r,_p)                      \
        __asm__ __volatile__ (          \
                "inl  %%dx, %%eax \n\t" \
                "mfence           \n\t" \
                :"=a"(_r)               \
                :"d"(_p)                \
        );

#define outw(_p,_v)                     \
        __asm__ __volatile__ (          \
                "outw  %%ax, %%dx \n\t" \
                "mfence           \n\t" \
                ::"a"(_v),"d"(_p)       \
        );
#define outl(_p,_v)                     \
        __asm__ __volatile__ (          \
                "outl  %%eax, %%dx \n\t"\
                "mfence            \n\t"\
                ::"a"(_v),"d"(_p)       \
        );
#define insb(_p,_b,_c)                  \
        __asm__ __volatile__ (          \
                "cld        \n\t"       \
                "rep insb   \n\t"       \
                "mfence     \n\t"       \
                :                       \
                :"d"(_p),"D"(_b),"c"(_c)\
        );
#define insw(_p,_b,_c)                  \
        __asm__ __volatile__ (          \
                "cld        \n\t"       \
                "rep insw   \n\t"       \
                "mfence     \n\t"       \
                :                       \
                :"d"(_p),"D"(_b),"c"(_c>>1)\
        );
#define insl(_p,_b,_c)                  \
        __asm__ __volatile__ (          \
                "cld        \n\t"       \
                "rep insl   \n\t"       \
                "mfence     \n\t"       \
                :                       \
                :"d"(_p),"D"(_b),"c"(_c>>2)\
        );
#define outsb(_p,_b,_c)                 \
        __asm__ __volatile__ (          \
                "cld        \n\t"       \
                "rep outsb  \n\t"       \
                "mfence     \n\t"       \
                :                       \
                :"d"(_p),"S"(_b),"c"(_c)\
        );
#define outsw(_p,_b,_c)                 \
        __asm__ __volatile__ (          \
                "cld        \n\t"       \
                "rep outsw  \n\t"       \
                "mfence     \n\t"       \
                :                       \
                :"d"(_p),"S"(_b),"c"(_c>>1)\
        );
#define outsl(_p,_b,_c)                 \
        __asm__ __volatile__ (          \
                "cld        \n\t"       \
                "rep outsl  \n\t"       \
                "mfence     \n\t"       \
                :                       \
                :"d"(_p),"S"(_b),"c"(_c>>2)\
        );

#endif
