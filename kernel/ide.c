#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/lapic.h>
#include <cnix/traps.h>
#include <cnix/desc.h>

#define HD_DATA		0x1f0	/* _CTL when writing */
#define HD_ERROR	0x1f1	/* see err-bits */
#define HD_NR		0x1f2	/* nr of sectors to read/write */
#define HD_SECTOR	0x1f3	/* starting sector */
#define HD_LCYL		0x1f4	/* starting cylinder */
#define HD_HCYL		0x1f5	/* high byte of starting cyl */
#define HD_CURRENT	0x1f6	/* 101dhhhh , d=drive, hhhh=head */
#define HD_STATUS	0x1f7	/* see status-bits */
#define HD_COMMAND  HD_STATUS	/* same address, read=status, write=cmd */

/* Bits of HD_STATUS */
#define STAT_BUSY	0x80
#define STAT_DRDY	0x40
#define STAT_DF		0x20
#define STAT_DRQ	0x08
#define STAT_ERR	0x01

/* Values for HD_COMMAND */
#define CMD_READ	0x20
#define CMD_WRITE	0x30
#define CMD_IDENTIFY	0xEC

static void ide_wait_not_busy(short control)
{
	volatile uint8_t status;
	do{
		status = inb(HD_STATUS);
	} while(status & STAT_BUSY);
}

static void ide_wait_drdy(short control)
{
	volatile uint8_t status;
	do{
		status = inb(HD_STATUS);
	} while(!(status & STAT_DRDY));
}

static void ide_wait_drq(short control)
{
	volatile uint8_t status;
	do{
		status = inb(HD_STATUS);
	} while(!(status & STAT_DRQ));
}

static void ide_out_cmd(short ctl, char dev, char cmd, char nr, long lba)
{
	outb(0x3f6,0);
	ide_wait_not_busy(ctl);
	//outb(0x1f1, 0);
	outb(0x1f2, nr);
	outb(0x1f3, lba & 0xff);
	outb(0x1f4, (lba >> 8) & 0xff);
	outb(0x1f5, (lba >> 16) & 0xff);
	outb(0x1f6, ((lba >> 24) & 0x0f) | ((dev & 1) << 4) |0xe0);
	ide_wait_drdy(ctl);
	outb(0x1f7, cmd);
}

short w_buf[512] = { 0 };
void try_write()
{
	memset(w_buf, 0x6f, 512);
	ide_out_cmd(0x1f0, 0, CMD_WRITE, 1, 0);
	ide_wait_drq(0x1f0);
	outsw(HD_DATA, w_buf, 512);
}

void try_read()
{
	ide_out_cmd(0x1f0, 0, CMD_READ, 3, 0);
}

extern void int_ide0();
volatile unsigned short buf[512] = { 0 };
void setup_ide()
{
	ide_out_cmd(0x1f0, 0, CMD_IDENTIFY, 0, 0);

	if (inb(HD_STATUS) == 0 || inb(HD_STATUS) == 0xff) {
		printk("HD Control not exsit");
		return;
	}

	ide_wait_drq(0x1f0);
	insw(HD_DATA, buf, 256);

	unsigned long nr_sector = buf[60] | buf[61] << 16;
	printk("HD sectors=%d\n", nr_sector);

	set_intr_gate(T_IDE0, (long)int_ide0);
	ioapic_enable(T_IDE0);
}

void __attribute__((optimize("O0"))) do_ide0()
{
	lapic_eoi();
	insw(HD_DATA, buf, 256);

	for(int i=0; i<32;i++)
		printk("%0#6x\t", buf[i]);
	printk("\n");
}
