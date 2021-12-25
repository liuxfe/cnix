#ifndef _CNIX_KERNEL_H
#define _CNIX_KERNEL_H

#define PHYOFF		0xFFFF800000000000

#define __p2v(a)	((a) + PHYOFF)
#define __v2p(a)	((a) - PHYOFF)

#endif
