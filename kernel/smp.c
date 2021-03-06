#include <cnix/config.h>
#include <cnix/kernel.h>
#include <cnix/lapic.h>

struct mp {				// floating pointer
	uint8_t		signature[4];	// "_MP_"
	uint32_t 	physaddr;	// phys addr of MP config table
	uint8_t		length;		// 1
	uint8_t		version;	// [14]
	uint8_t		checksum;	// all bytes must add up to 0
	uint8_t		type;		// MP system config type
	uint8_t		imcrp;
	uint8_t		reserved[3];
};

struct mpconf {				// configuration table header
	uint8_t		signature[4];	// "PCMP"
	uint16_t	length;		// total table length
	uint8_t 	version;	// [14]
	uint8_t 	checksum;	// all bytes must add up to 0
	uint8_t 	product[20];	// product id
	uint32_t	oemtable;	// OEM table pointer
	uint16_t	oemlength;	// OEM table length
	uint16_t	entrycnt;	// entry count
	uint32_t	lapicaddr;	// address of local APIC
	uint16_t	xlength;	// extended table length
	uint8_t 	xchecksum;	// extended table checksum
	uint8_t 	reserved;
};

// Table entry types
#define T_PROC    0x00  // One per processor
#define T_BUS     0x01  // One per bus
#define T_IOAPIC  0x02  // One per I/O APIC
#define T_IOINTR  0x03  // One per bus interrupt source
#define T_LINTR   0x04  // One per system interrupt source

struct mpproc {				// processor table entry
	uint8_t		type;		// entry type (0)
	uint8_t		apicid;		// local APIC id
	uint8_t		version;	// local APIC verison
	uint8_t		flags;		// CPU flags, 0x02 is bsp
	uint8_t		signature[4];	// CPU signature
	uint32_t	feature;	// feature flags from CPUID instruction
	uint8_t		reserved[8];
};

struct mpioapic {			// I/O APIC table entry
	uint8_t		type;		// entry type (2)
	uint8_t		apicno;		// I/O APIC id
	uint8_t		version;	// I/O APIC version
	uint8_t		flags;		// I/O APIC flags
  	uint32_t	addr;		// I/O APIC address
};

static __init int sum(uint8_t * addr, int len)
{
	uint8_t sum = 0;
	for(int i=0; i<len; i++)
		sum+= *addr++;
	return sum;
}

static __init struct mp* mpsearch()
{
	uint8_t* start = (uint8_t*)__p2v(0xF0000);
	uint8_t* end =   (uint8_t*)__p2v(0xFFFFF);
	uint8_t* p;

	for(p = start; p<end; p+=sizeof(struct mp)){
		if(*p=='_'&&*(p+1)=='M'&& *(p+2)=='P'&&*(p+3)=='_'
			&& sum(p, sizeof(struct mp)) == 0)
			return (struct mp*)p;
	}
	return NULL;
}

static __init void checkmc(struct mpconf* mc)
{
	if(mc->signature[0]!='P' || mc->signature[1]!='C' ||
		mc->signature[2]!='M' || mc->signature[3]!='P'){
		printk("Bad PCMP");
		//return;
	}
	if(mc->version != 1 && mc->version != 4){
		printk("Bad version");
		//return;
	}
	if(sum((uint8_t*)mc, mc->length)){
		printk("Bad sum");
	}
}

int NR_CPUS=0;

extern void* ioapic;

static struct mp * __mp = NULL;
static struct mpconf* __mc = NULL;

void __init setup_smp()
{
	//struct mpproc* proc;
	struct mpioapic* _ioapic;

	__mp= mpsearch();
	if(!__mp){
		printk("_MP_  not found !");
		while(1){};
	}

	__mc = (struct mpconf *)__p2v(__mp->physaddr);
	checkmc(__mc);
	//printk("MP Conf : entry count=%d\n", mc->entrycnt);
	//printk("MP Conf : lapicaddr=%x\n", mc->lapicaddr);
	__lapic =(int32_t*) __p2v(__mc->lapicaddr); // set Local APIC address

	uint8_t* p = (uint8_t*)__mc + sizeof(struct mpconf);
	for(int i=0;i<__mc->entrycnt;i++){
		switch(*p){
		    case T_PROC:
			//proc = (struct mpproc*)p;
			//printk("CPU apicid: %d\n", proc->apicid);
			NR_CPUS++;
			p+=sizeof(struct mpproc);
			continue;
		    case T_BUS:
			p+=8; continue;
		    case T_IOAPIC:
			_ioapic=(struct mpioapic*)p;
			//printk("IOAPIC id:%d, addr: %x\n", _ioapic->apicno, _ioapic->addr);
			ioapic = (void*)__p2v(_ioapic->addr);
			p+=sizeof(struct mpioapic);
			continue;
		    case T_IOINTR:
			p+=8; continue;
		    case T_LINTR:
			p+=8; continue;
		    default:
			printk("Unkown type");break;
		}
	}
	//printk("%d CPUS found\n", NR_CPUS);
}
