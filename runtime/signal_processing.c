#include "../include/white/runtime.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Signal creation and management */
white_signal *white_signal_create(size_t length, double sr) {
    white_signal *s = malloc(sizeof(white_signal));
    s->samples = calloc(length, sizeof(white_iq));
    s->length = length;
    s->sample_rate = sr;
    return s;
}

void white_signal_free(white_signal *s) {
    if (s) {
        if (s->samples) free(s->samples);
        free(s);
    }
}

white_signal *white_signal_load_iq(const char *filename, double sample_rate) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    size_t num_samples = size / sizeof(white_iq);
    white_signal *s = white_signal_create(num_samples, sample_rate);
    
    fread(s->samples, sizeof(white_iq), num_samples, f);
    fclose(f);
    
    return s;
}

/* Spectrum creation and management */
white_spectrum *white_spectrum_create(size_t length, double sr) {
    white_spectrum *sp = malloc(sizeof(white_spectrum));
    sp->magnitude = calloc(length, sizeof(double));
    sp->phase = calloc(length, sizeof(double));
    sp->frequency = calloc(length, sizeof(double));
    sp->length = length;
    sp->sample_rate = sr;
    
    /* Initialize frequency bins */
    for (size_t i = 0; i < length; i++) {
        sp->frequency[i] = (double)i * sr / length;
    }
    
    return sp;
}

void white_spectrum_free(white_spectrum *sp) {
    if (sp) {
        if (sp->magnitude) free(sp->magnitude);
        if (sp->phase) free(sp->phase);
        if (sp->frequency) free(sp->frequency);
        free(sp);
    }
}

/* DFT-based FFT (O(n²) for simplicity, suitable for n < 4096) */
white_spectrum *white_fft(white_signal *s, size_t size) {
    white_spectrum *sp = white_spectrum_create(size, s->sample_rate);
    
    size_t n = s->length < size ? s->length : size;
    
    for (size_t k = 0; k < size; k++) {
        double real = 0.0, imag = 0.0;
        
        for (size_t n_idx = 0; n_idx < n; n_idx++) {
            double angle = -2.0 * M_PI * k * n_idx / size;
            double cos_a = cos(angle);
            double sin_a = sin(angle);
            
            real += s->samples[n_idx].i * cos_a - s->samples[n_idx].q * sin_a;
            imag += s->samples[n_idx].i * sin_a + s->samples[n_idx].q * cos_a;
        }
        
        sp->magnitude[k] = sqrt(real * real + imag * imag);
        sp->phase[k] = atan2(imag, real);
    }
    
    return sp;
}

/* Inverse FFT */
white_spectrum *white_ifft(white_spectrum *sp) {
    /* Simplified placeholder - full IFFT would be implemented here */
    return sp;
}

/* Spectrum analysis functions */
double white_spectrum_peak_frequency(white_spectrum *sp) {
    if (!sp || sp->length == 0) return 0.0;
    
    size_t peak_idx = 0;
    double peak_mag = 0.0;
    
    for (size_t i = 0; i < sp->length; i++) {
        if (sp->magnitude[i] > peak_mag) {
            peak_mag = sp->magnitude[i];
            peak_idx = i;
        }
    }
    
    return sp->frequency[peak_idx];
}

double white_spectrum_bandwidth(white_spectrum *sp) {
    if (!sp || sp->length == 0) return 0.0;
    
    double peak_mag = 0.0;
    size_t peak_idx = 0;
    
    /* Find peak */
    for (size_t i = 0; i < sp->length; i++) {
        if (sp->magnitude[i] > peak_mag) {
            peak_mag = sp->magnitude[i];
            peak_idx = i;
        }
    }
    
    /* Find -3dB points */
    double half_power = peak_mag / sqrt(2.0);
    
    int left_idx = peak_idx;
    while (left_idx > 0 && sp->magnitude[left_idx] > half_power) {
        left_idx--;
    }
    
    int right_idx = peak_idx;
    while (right_idx < (int)sp->length - 1 && sp->magnitude[right_idx] > half_power) {
        right_idx++;
    }
    
    return sp->frequency[right_idx] - sp->frequency[left_idx];
}

double white_spectrum_snr(white_spectrum *sp) {
    if (!sp || sp->length < 2) return 0.0;
    
    double peak_power = 0.0;
    double noise_floor = 0.0;
    size_t peak_idx = 0;
    
    /* Find peak */
    for (size_t i = 0; i < sp->length; i++) {
        if (sp->magnitude[i] > peak_power) {
            peak_power = sp->magnitude[i] * sp->magnitude[i];
            peak_idx = i;
        }
    }
    
    /* Estimate noise floor from 10% lowest values */
    size_t low_count = sp->length / 10;
    for (size_t i = 0; i < low_count && i < sp->length; i++) {
        if (i != peak_idx) {
            noise_floor += sp->magnitude[i] * sp->magnitude[i];
        }
    }
    
    if (low_count > 0) {
        noise_floor /= low_count;
    }
    
    if (noise_floor <= 0.0) noise_floor = 1e-12;
    
    return 10.0 * log10(peak_power / noise_floor);
}

double white_spectrum_power(white_spectrum *sp) {
    if (!sp || sp->length == 0) return 0.0;
    
    double total_power = 0.0;
    for (size_t i = 0; i < sp->length; i++) {
        total_power += sp->magnitude[i] * sp->magnitude[i];
    }
    
    return 10.0 * log10(total_power / sp->length + 1e-12);
}

/* Complex arithmetic */
white_complex white_complex_add(white_complex a, white_complex b) {
    return a + b;
}

white_complex white_complex_mul(white_complex a, white_complex b) {
    return a * b;
}

white_complex white_complex_conj(white_complex a) {
    return conj(a);
}

double white_complex_magnitude(white_complex a) {
    return cabs(a);
}

double white_complex_phase(white_complex a) {
    return carg(a);
}

/* IQ conversion */
white_iq white_iq_from_complex(white_complex c) {
    white_iq iq;
    iq.i = creal(c);
    iq.q = cimag(c);
    return iq;
}

white_complex white_iq_to_complex(white_iq iq) {
    return (double complex)(iq.i + iq.q * I);
}

/* Print utilities */
void white_print_complex(white_complex c) {
    printf("(%f + %fi)", creal(c), cimag(c));
}

void white_print_iq(white_iq iq) {
    printf("(I=%f, Q=%f)", iq.i, iq.q);
}

void white_print_signal(white_signal *s) {
    if (!s) {
        printf("(null signal)");
        return;
    }
    printf("signal { samples=%zu, sr=%.0f Hz }", s->length, s->sample_rate);
}

void white_print_spectrum(white_spectrum *sp) {
    if (!sp) {
        printf("(null spectrum)");
        return;
    }
    printf("spectrum { bins=%zu, sr=%.0f Hz, peak=%.0f Hz }", 
           sp->length, sp->sample_rate, white_spectrum_peak_frequency(sp));
}
