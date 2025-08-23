#include <stdint.h>

typedef struct pcg_state {
    uint64_t state;   // RNG state.  All values are possible.
    uint64_t inc;     // Controls which RNG sequence (stream) is
                      // selected. Must *always* be odd.
} pcg_state;

void pcg_seed(pcg_state* rng, uint64_t initstate, uint64_t initseq);
uint32_t pcg_random(pcg_state* rng);
