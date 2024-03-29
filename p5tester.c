#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
#define PGSIZE 4096
#define KERNBASE 0x80000000 
#define VPN(va)         ((uint)(va) >> 12)

void print_ptentry(struct pt_entry entry) {
      
      printf(1, "PDX: %d, PTX: %d, PPN: %x, Present: %d, Writable: %d, Encrypted: %d, User: %d, Ref: %d\n", 
        entry.pdx, entry.ptx, entry.ppage, entry.present, entry.writable, entry.encrypted, entry.user, entry.ref);
}

static int 
err(char *msg, ...) {
    printf(1, "XV6_TEST_OUTPUT %s\n", msg);
    exit();
}

int
main(int argc, char **argv)
{
    const uint PAGES_NUM = 1;
    char *buffer = sbrk(PGSIZE * sizeof(char));
    while (buffer != (char*)0x6000) {
        buffer = sbrk(PGSIZE * sizeof(char));
    }
    // Allocate one pages of space
    char *ptr = sbrk(PAGES_NUM * PGSIZE);
    
    struct pt_entry pt_entries[PAGES_NUM];

    // Initialize the pages
    for (int i = 0; i < PAGES_NUM * PGSIZE; i++)
        ptr[i] = 0xAA;

    // Call the mencrypt with len = 0
    if (mencrypt((char *)KERNBASE, 0) != 0)
        err("mencrypt return non-zero value when len equals 0\n");
    
    // Call the mencrypt on the kernel pages
    if (mencrypt((char *)ptr, -1) != -1)
        err("mencrypt doesn't return -1 value when negative length is given\n");

    // Call the mencrypt on the kernel pages
    if (mencrypt((char *)KERNBASE, 1) != -1)
        err("mencrypt doesn't return -1 value when trying to encrypt kernel page\n");

    // Call the mencrypt on the kernel pages
    if (mencrypt((char *)((uint)-1), 1) != -1)
        err("mencrypt doesn't return -1 value when 0xFFFFFFFF is given as virtual page\n");
    

    if (getpgtable(pt_entries, PAGES_NUM, 0) >= 0){
        for (int i = 0; i < PAGES_NUM; i++) {
            printf(1, "XV6_TEST_OUTPUT Index %d: pdx: 0x%x, ptx: 0x%x, ppage: 0x%x, present: %d, writable: %d, encrypted: %d\n", 
                i,
                pt_entries[i].pdx,
                pt_entries[i].ptx,
                pt_entries[i].ppage,
                pt_entries[i].present,
                pt_entries[i].writable,
                pt_entries[i].encrypted
            );

            if (dump_rawphymem((uint)(pt_entries[i].ppage * PGSIZE), buffer) != 0)
                err("dump_rawphymem return non-zero value\n");
            
            uint expected = 0xAA;
            uint is_failed = 0;
            for (int j = 0; j < PGSIZE; j ++) {
                if (buffer[j] != (char)expected) {
                    is_failed = 1;
                    break;
                }
            }
            if (is_failed) {
                printf(1, "XV6_TEST_OUTPUT wrong content at physical page 0x%x\n", pt_entries[i].ppage * PGSIZE);
                for (int j = 0; j < PGSIZE; j +=64) {
                    printf(1, "XV6_TEST_OUTPUT ");
                    for (int k = 0; k < 64; k ++) {
                        if (k < 63) {
                            printf(1, "0x%x ", (uint)buffer[j + k] & 0xFF);
                        } else {
                            printf(1, "0x%x\n", (uint)buffer[j + k] & 0xFF);
                        }
                    }
                }
                err("None of the pages should be encrypted when len equals to 0\n");
            }

        }
    }
    exit();



















  /*
  int * tester = malloc(2250*sizeof(int));

  uint vpn = 0;
  int vpnCount = 0;
  for (int i = 0; i < 2250; i++) {
    if (PPN(tester + i) != vpn) {
      //printf(1, "Tester vpn=%d at i=%d, pointer %p\n", PPN(tester+i), i, tester+i);
      vpn = PPN(tester + i);
      vpnCount += 1;
    }
    tester[i] = 0x55;
  }
  printf(1, "Tester was %x\n", tester[0]);
  mencrypt((char*)tester, vpnCount);
  //printf(1, "tester is now %p\n", tester);
  //printf(1, "Tester vpn is %d, pointer %p\n", PPN(tester), tester);
  //printf(1, "Tester is now %x\n", tester[0]);
  //printf(1, "Tester 300 vpn is %d\n", PPN(tester+300));
  //printf(1, "Tester 300 is now %x\n", tester[300]);
  //printf(1, "Tester 2000 vpn is %d\n", PPN(tester+2000));
  //printf(1, "Tester 2000 is now %x\n", tester[2000]);

  struct pt_entry mystructs[5];
  int retval = getpgtable(mystructs, 5);
  printf(2, "retval is %d\n", retval);
  char * buffer = malloc(2*PGSIZE);
  for (int i = 0; i < retval; i++) {
    print_ptentry(mystructs[i]);
    printf(2, "Physical address is %p\n", mystructs[i].ppage * PGSIZE);
    if (dump_rawphymem((char *)(mystructs[i].ppage * PGSIZE), buffer) != 0) {
      printf(2, "Error in dumprawphymem\n");
    }
    else {
      printf(2, "yeet\n");
    }

  }

  exit();*/
}
