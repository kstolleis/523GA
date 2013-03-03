#ifndef _Utils
#define _Utils
#include <stdlib.h>
#include <math.h>

namespace utils {

    /*
     * Returns a random float in the range [0,x].
     */
    static inline float randomFloat(float x) {
        return ((float) rand() / ((long) RAND_MAX + 1))*x;
    }

    /*
     * Returns a sample from a normal distribution with mean m and standard deviation s.
     */
    static inline float randomNormal(float m, float s) {
        float u = randomFloat(1.);
        float v = randomFloat(1.);
        float x = sqrtf(-2 * logf(1.0 - u));

        if (roundf(randomFloat(1.)) == 0) {
            return x * cos(2 * M_PI * v) * s + m;
        }

        return x * sin(2 * M_PI * v) * s + m;
    }

    /*
     * Returns proper modulus of dividend and divisor
     */
    static inline float pmod(float dividend, float divisor) {
        float temp = fmod(dividend, divisor);
        while (temp < 0) {
            temp += divisor;
        }
        return temp;
    }
}
#endif
