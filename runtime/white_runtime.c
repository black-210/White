#include "../include/white/runtime.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

white_signal *white_signal_create(size_t length, double sr) {
    white_signal *s = calloc(1, sizeof(*s));
    if (!s) return NULL;
    s->samples = length ? calloc(length, sizeof(*s->samples)) : NULL;
    if (length && !s->samples) { free(s); return NULL; }
    s->length = length; s->sample_rate = sr; return s;
}
void white_signal_free(white_signal *s) { if (s) { free(s->samples); free(s); } }
white_signal *white_signal_load_iq(const char *filename, double sample_rate) {
    FILE *f = fopen(filename, "rb"); if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long end = ftell(f); if (end < 0) { fclose(f); return NULL; }
    rewind(f); size_t n = (size_t)end / sizeof(white_iq);
    white_signal *s = white_signal_create(n, sample_rate);
    if (s && n && fread(s->samples, sizeof(white_iq), n, f) != n) { white_signal_free(s); s = NULL; }
    fclose(f); return s;
}
white_spectrum *white_spectrum_create(size_t length, double sr) {
    if (!length) return NULL;
    white_spectrum *sp = calloc(1, sizeof(*sp)); if (!sp) return NULL;
    sp->magnitude = calloc(length, sizeof(double)); sp->phase = calloc(length, sizeof(double)); sp->frequency = calloc(length, sizeof(double));
    if (!sp->magnitude || !sp->phase || !sp->frequency) { white_spectrum_free(sp); return NULL; }
    sp->length = length; sp->sample_rate = sr;
    for (size_t i = 0; i < length; i++) sp->frequency[i] = (double)i * sr / (double)length;
    return sp;
}
void white_spectrum_free(white_spectrum *sp) { if (sp) { free(sp->magnitude); free(sp->phase); free(sp->frequency); free(sp); } }
static int is_power_of_two(size_t n) { return n && !(n & (n - 1)); }
static void dft(const white_signal *s, white_complex *out, size_t size) {
    for (size_t k = 0; k < size; k++) { white_complex sum = 0.0; for (size_t n = 0; n < size; n++) { white_complex x = n < s->length ? s->samples[n].i + s->samples[n].q * I : 0.0; double a = -2.0 * M_PI * (double)k * (double)n / (double)size; sum += x * (cos(a) + sin(a) * I); } out[k] = sum; }
}
static void fft_radix2(white_complex *a, size_t n) {
    for (size_t i = 1, j = 0; i < n; i++) { size_t bit = n >> 1; for (; j & bit; bit >>= 1) j ^= bit; j ^= bit; if (i < j) { white_complex t = a[i]; a[i] = a[j]; a[j] = t; } }
    for (size_t len = 2; len <= n; len <<= 1) { double base = -2.0 * M_PI / (double)len; for (size_t i = 0; i < n; i += len) for (size_t j = 0; j < len / 2; j++) { white_complex w = cos(base*j) + sin(base*j) * I; white_complex u = a[i+j], v = a[i+j+len/2] * w; a[i+j] = u + v; a[i+j+len/2] = u - v; } }
}
white_spectrum *white_fft(white_signal *s, size_t size) {
    if (!s || !size) return NULL;
    white_complex *out = calloc(size, sizeof(*out)); if (!out) return NULL;
    if (is_power_of_two(size)) { for (size_t i = 0; i < size && i < s->length; i++) out[i] = s->samples[i].i + s->samples[i].q * I; fft_radix2(out, size); } else dft(s, out, size);
    white_spectrum *sp = white_spectrum_create(size, s->sample_rate); if (!sp) { free(out); return NULL; }
    for (size_t i = 0; i < size; i++) { sp->magnitude[i] = cabs(out[i]); sp->phase[i] = carg(out[i]); }
    free(out); return sp;
}
white_spectrum *white_ifft(white_spectrum *sp) { (void)sp; return NULL; }
double white_spectrum_peak_frequency(white_spectrum *sp) { if (!sp || !sp->length) return 0; size_t p=0; for (size_t i=1;i<sp->length;i++) if(sp->magnitude[i]>sp->magnitude[p]) p=i; return sp->frequency[p]; }
double white_spectrum_bandwidth(white_spectrum *sp) { if (!sp || sp->length<2) return 0; double peak=0; for(size_t i=0;i<sp->length;i++) if(sp->magnitude[i]>peak) peak=sp->magnitude[i]; double cut=peak/sqrt(2.0); size_t l=0,r=sp->length-1; while(l<sp->length&&sp->magnitude[l]<cut)l++; while(r>l&&sp->magnitude[r]<cut)r--; return sp->frequency[r]-sp->frequency[l]; }
double white_spectrum_snr(white_spectrum *sp) { if(!sp||!sp->length)return 0; double peak=0,noise=0; size_t count=0; for(size_t i=0;i<sp->length;i++)if(sp->magnitude[i]>peak)peak=sp->magnitude[i]; for(size_t i=0;i<sp->length;i++)if(sp->magnitude[i]!=peak){noise+=sp->magnitude[i]*sp->magnitude[i];count++;} noise=count?noise/count:1e-12; return 10*log10((peak*peak)/fmax(noise,1e-12)); }
double white_spectrum_power(white_spectrum *sp) { if(!sp||!sp->length)return 0; double p=0; for(size_t i=0;i<sp->length;i++)p+=sp->magnitude[i]*sp->magnitude[i]; return p/(double)sp->length; }
white_complex white_complex_add(white_complex a,white_complex b){return a+b;} white_complex white_complex_mul(white_complex a,white_complex b){return a*b;} white_complex white_complex_conj(white_complex a){return conj(a);} double white_complex_magnitude(white_complex a){return cabs(a);} double white_complex_phase(white_complex a){return carg(a);} white_iq white_iq_from_complex(white_complex c){white_iq x={creal(c),cimag(c)};return x;} white_complex white_iq_to_complex(white_iq x){return x.i+x.q*I;}
void white_print_complex(white_complex c){printf("%.6f + %.6fi",creal(c),cimag(c));} void white_print_iq(white_iq x){printf("I=%.6f, Q=%.6f",x.i,x.q);} void white_print_signal(white_signal*s){if(!s){printf("NULL signal\n");return;}printf("Signal: %zu samples @ %.2f Hz\n",s->length,s->sample_rate);} void white_print_spectrum(white_spectrum*sp){if(!sp){printf("NULL spectrum\n");return;}printf("Spectrum: %zu bins @ %.2f Hz SR\n",sp->length,sp->sample_rate);}
