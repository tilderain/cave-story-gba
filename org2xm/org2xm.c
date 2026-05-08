/*
 * Org2XM v4.0
 *
 * Converts Org songs from Cave Story to XM modules.
 * Based on Org2XM by Jan "Rrrola" Kadlec (Public Domain).
 * Modifications and fixes by third party.
 *
 * Changes from v3.3:
 *   - Removed 8-channel allocator/crusher entirely.
 *     One XM channel per Org track (up to 16).
 *   - Fixed inverted memcmp version checks.
 *   - Fixed SBKload drum length accumulation bug (tmp not reset per iter).
 *   - Fixed relativeKey per octave split to tune each sample to its
 *     2-octave midpoint (notes 12, 36, 60, 84).
 *   - Removed dead oct_wave entries (32, 8) that didn't match the 4-split
 *     design, replaced with correct sizes: 256, 128, 64, 16.
 *   - Kept per-octave resampling and delta encoding of melody samples.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PACKED __attribute__((packed))

#define read(to, bytes)   fread(to,   1, bytes, f)
#define write(from, bytes) fwrite(from, 1, bytes, g)

#define VOL      51   /* default XM volume */
#define MAX_BARS 2048 /* safe song length limit */

/* 4 x 2-octave splits.
 * wave_size: number of samples in the resampled loop.
 * The original 256-sample wave is resampled to this length.
 * Each halving in size raises the pitch one octave when looped at the
 * same sample rate, matching the original Organya engine's behaviour. */
typedef struct { int wave_size; } OCTWAVE;
static const OCTWAVE oct_wave[4] = {
    { 256 }, /* split 0: notes  0-23  (oct 0-1) */
    { 128 }, /* split 1: notes 24-47  (oct 2-3) */
    {  64 }, /* split 2: notes 48-71  (oct 4-5) */
    {  16 }, /* split 3: notes 72-95  (oct 6-7) */
};

/*
 * relativeKey values so each split plays in tune at the midpoint of its
 * 2-octave range.
 *
 * XM maps middle-C (C-5) to relative key 48 at the base sample rate.
 * The original Org player uses a 256-sample loop at sample-rate proportional
 * to the note frequency.  After resampling to wave_size samples the loop is
 * shorter by log2(256/wave_size) octaves, so we must raise the relative key
 * by that many semitones to compensate, then centre on the split's midpoint.
 *
 * Midpoint notes (0-indexed semitones from C-0): 12, 36, 60, 84
 *   split 0 (256 samples): shift =  0  → relKey = 48 - 0  - (48-12) = 12
 *   Actually the formula used by Rrrola's original is relativeKey=48 for the
 *   256-sample case, meaning note 48 (C-4) plays at 8363 Hz.  We keep that
 *   anchor and derive the rest from it.
 *
 *   relativeKey[o] = 48 - (octave_shift_in_semitones_vs_split_0)
 *                  = 48 - 12*log2(256 / wave_size[o])
 *   split 0: 48 - 12*log2(256/256) = 48 - 0  = 48
 *   split 1: 48 - 12*log2(256/128) = 48 - 12 = 36
 *   split 2: 48 - 12*log2(256/64)  = 48 - 24 = 24
 *   split 3: 48 - 12*log2(256/16)  = 48 - 48 =  0
 */
static const int8_t split_relkey[4] = { 48, 36, 24, 0 };

/* ------------------------------------------------------------------ Input */

#pragma pack(push, 1)
struct OrgHeader {
    uint8_t  magic[6];
    uint16_t msPerBeat;
    uint8_t  measuresPerBar;
    uint8_t  beatsPerMeasure;
    uint32_t loopStart;
    uint32_t loopEnd;
} PACKED header;
#pragma pack(pop)

struct Note {
    uint32_t start;
    uint8_t  len;
    uint8_t  key;
    uint8_t  vol;
    uint8_t  pan;
} *note[16];

/* ----------------------------------------- Per-track instrument metadata */

#pragma pack(push, 1)
struct Instrument {
    uint16_t freqShift;
    uint8_t  sample;
    uint8_t  noLoop;
    uint16_t notes;
    uint8_t  drum;
    uint8_t  instrument; /* XM instrument base id (1-indexed) */
    int8_t   finetune;
    uint8_t  _pad[3];
} PACKED t[16];
#pragma pack(pop)

/* Per-channel encode state (one per output XM channel = one per Org track) */
struct ChannelState {
    int8_t  lastPan;
    uint8_t lastVol;
    uint8_t played;
} chState[16];

/* Decoded per-row note data */
struct Track {
    float   freq;
    uint8_t vol;
    int8_t  pan;
} *n[16];

/* --------------------------------------------------------- XM structures */

#pragma pack(push, 1)
struct XMHeader {
    uint8_t  id[17];
    uint8_t  moduleName[20];
    uint8_t  eof;
    uint8_t  trackerName[20];
    uint16_t version;
    uint32_t headerSize;
    uint16_t songLength;
    uint16_t restartPosition;
    uint16_t channels;
    uint16_t patterns;
    uint16_t instruments;
    uint16_t flags;
    uint16_t tempo;
    uint16_t bpm;
    uint8_t  patternOrder[256];
} PACKED xmh = {
    "Extended Module: ", "", 0x1A,
    "Org2XM v4.0     ", 0x104, 0x114
};

struct XMInstrument {
    uint32_t size;
    uint8_t  instrumentName[22];
    uint8_t  zero;
    uint16_t samples;
    uint32_t sampleHeaderSize;
    uint8_t  misc[230];
    uint32_t sampleLength;
    uint32_t loopStart;
    uint32_t loopLength;
    uint8_t  volume;
    int8_t   finetune;
    uint8_t  type;
    uint8_t  panning;
    int8_t   relativeKey;
    uint8_t  reserved;
    uint8_t  sampleName[22];
} PACKED smp = { 0x107, "Melody-00", 0, 1, 0x28, {}, 256, 0, 256, VOL, 0, 1, 128, 48, 0, "" };
#pragma pack(pop)

/* ---------------------------------------------------- Sound bank loading */

struct SoundBank {
    uint8_t  magic[6];
    uint8_t  verbank;
    uint8_t  verorg;
    uint8_t  snumMelo;
    uint8_t  snumDrum;
    uint16_t lenMelo;
    uint32_t *tblLenDrum;
    uint32_t *tblOffDrum;
    char     (*tblNameDrum)[22];
    int8_t   *melody; /* linear PCM */
    int8_t   *drums;  /* will be delta-encoded after load */
    uint32_t  lenAllDrm;
} sbank;

static int SBKload(char *path) {
    FILE *f;
    uint32_t i, j;

    if (!(f = fopen(path, "rb"))) {
        printf("Couldn't open samplebank: %s\n", path);
        return 10;
    }

    read(&sbank.magic, 6);
    if (memcmp(sbank.magic, "ORGBNK", 6)) {
        printf("Invalid samplebank header!\n");
        fclose(f);
        return 11;
    }

    read(&sbank.verbank, 1);
    read(&sbank.verorg,  1);
    read(&sbank.snumMelo, 1);
    read(&sbank.snumDrum, 1);

    /* melody sample length — big-endian u16 */
    {
        uint8_t x[2];
        read(x, 2);
        sbank.lenMelo = ((uint16_t)x[0] << 8) | x[1];
    }

    sbank.tblLenDrum = malloc(sbank.snumDrum * sizeof(uint32_t));
    sbank.tblOffDrum = malloc(sbank.snumDrum * sizeof(uint32_t));
    sbank.lenAllDrm  = 0;

    for (i = 0; i < sbank.snumDrum; i++) {
        /* drum sample length — big-endian u32.
         * BUG FIX: tmp must be reset to 0 for every entry. */
        uint32_t tmp = 0;
        uint8_t  x;
        read(&x, 1); tmp = (tmp << 8) | x;
        read(&x, 1); tmp = (tmp << 8) | x;
        read(&x, 1); tmp = (tmp << 8) | x;
        read(&x, 1); tmp = (tmp << 8) | x;
        sbank.tblLenDrum[i]  = tmp;
        sbank.tblOffDrum[i]  = sbank.lenAllDrm;
        sbank.lenAllDrm     += tmp;
    }

#define MAXSTR 23
    sbank.tblNameDrum = malloc(sbank.snumDrum * sizeof(char[MAXSTR]));
    for (i = 0; i < sbank.snumDrum; i++) {
        for (j = 0; j < MAXSTR; j++) {
            char c = (char)fgetc(f);
            sbank.tblNameDrum[i][j] = (j == MAXSTR - 1) ? '\0' : c;
            if (c == '\0') break;
        }
    }

    sbank.melody = malloc(sbank.snumMelo * sbank.lenMelo);
    read(sbank.melody, sbank.snumMelo * sbank.lenMelo);

    sbank.drums = malloc(sbank.lenAllDrm);
    read(sbank.drums, sbank.lenAllDrm);

    /* Delta-encode drum samples for XM (melody stays linear). */


    fclose(f);
    return 0;
}

/* ----------------------------------------------- XM pattern encode state */

static int compatibility = 0;
static int verorg        = 0;

static int     key, finetune;
static uint8_t wKey, wInst, wVol, wFine, wPan, wPanVol, wSkip;

static int tracks;
static int bars, rows, barLen, loop;
static int instruments;

static void encode(int i, int ch, int j) {
    void resetPanVol(void) {
        wPan = (n[i][j].pan != (wInst ? 0            : chState[ch].lastPan));
        wVol = (n[i][j].vol != (wInst ? (uint8_t)VOL : chState[ch].lastVol));
    }

    if (!j) {
        chState[ch].lastPan = chState[ch].lastVol = 0;
        chState[ch].played  = 0;
    }
    if (j % barLen == 0) chState[ch].played = 0;

    wKey = wInst = wVol = wFine = wPan = wPanVol = 0;
    wSkip = (j == (int)header.loopEnd - 1 && ch == tracks - 1);

    if (j == (int)header.loopStart && !t[i].noLoop)
        chState[ch].lastVol = (uint8_t)-1;

    if (n[i][j].freq) {
        wKey = 1;
        finetune = (int)((log2(n[i][j].freq / 8363.0) * 12
                         - t[i].finetune / 128.0
                         + t[i].drum * 36) * 8 + 0.5);
        key      = (finetune + 4) / 8;
        finetune -= key * 8;

        wInst = !chState[ch].played;
        if (compatibility) wInst = 1;
        wFine = !!finetune;
        resetPanVol();

        if (wPan && !n[i][j].pan)                                           { wInst = 1; resetPanVol(); }
        if (wVol && n[i][j].vol == VOL &&
            (n[i][j].pan != chState[ch].lastPan || !n[i][j].pan))          { wInst = 1; resetPanVol(); }
        if (wPan && (wFine || wSkip) && !wVol)                              { wPan = 0; wPanVol = 1; }
    } else {
        if (n[i][j].vol != chState[ch].lastVol) wVol = 1;
        if (n[i][j].vol && n[i][j].pan != chState[ch].lastPan) wPan = 1;

        if (wVol && !n[i][j].vol) {
            wVol = wPan = wFine = 0;
            if (!t[i].noLoop) { wKey = 1; key = 0x60; }
            chState[ch].lastVol = 0;
        }
    }

    if (wInst) { chState[ch].lastPan = 0; chState[ch].lastVol = VOL; chState[ch].played = 1; }
    if (wVol)  { wPanVol = 0; chState[ch].lastVol = n[i][j].vol; }
    if (wPanVol || wPan) chState[ch].lastPan = n[i][j].pan;
    if (wSkip) wPan = wFine = 0;
    if (wPan)  wFine = 0;
}

/* ------------------------------------------------------------------ Main */

/* [track][split 0..3] -> 1-based XM instrument id */
static int waveInstMap[16][4];

int main(int argc, char **argv) {
    int i, j, k;
    FILE *f, *g;

    if (argc < 2) {
        printf("Usage: org2xm input.org [ORGxxxyy.DAT] [c]\n");
        return 1;
    }
    compatibility = 1;

    int ret = SBKload(argc < 3 ? "ORG210EN.DAT" : argv[2]);
    if (ret) return ret;

    if (!(f = fopen(argv[1], "rb"))) {
        printf("Couldn't open \"%s\"\n", argv[1]);
        return 2;
    }

    read(&header, sizeof(struct OrgHeader));

    /* BUG FIX: was memcmp (non-zero on mismatch) instead of !memcmp. */
    if      (!memcmp(header.magic, "Org-02", 6)) { verorg = 2; }
    else if (!memcmp(header.magic, "Org-03", 6)) {
        verorg = 3;
        if (sbank.verorg == 0xFF) { printf("Bank version smaller than org version!\n"); return 6; }
    } else {
        printf("Invalid org header!\n");
        return 3;
    }
    if (sbank.verorg < verorg) { printf("Bank version smaller than org version!\n"); return 6; }

    /* Read track headers (6 bytes each). */
    for (i = 0; i < 16; ++i) read(&t[i], 6);

    /* Read notes and find song length. */
    rows = 0;
    for (i = 0; i < 16; ++i) {
        note[i] = malloc(t[i].notes * sizeof(struct Note));
        if (i >= 8) { t[i].sample += 100; t[i].noLoop = 1; t[i].drum = 1; }
        else          t[i].drum = 0;

        for (j = 0; j < t[i].notes; ++j) read(&note[i][j].start, 4);
        for (j = 0; j < t[i].notes; ++j) read(&note[i][j].key,   1);
        for (j = 0; j < t[i].notes; ++j) read(&note[i][j].len,   1);
        for (j = 0; j < t[i].notes; ++j) read(&note[i][j].vol,   1);
        for (j = 0; j < t[i].notes; ++j) read(&note[i][j].pan,   1);

        for (j = 0; j < t[i].notes; ++j)
            if (rows < (int)(note[i][j].start + note[i][j].len + 1))
                rows = note[i][j].start + note[i][j].len + 1;
    }

    barLen = header.measuresPerBar * header.beatsPerMeasure;
    if ((int)header.loopStart < rows) {
        loop = 1;
        bars = header.loopEnd / barLen;
    } else {
        loop = 0;
        bars = (rows + barLen - 1) / barLen;
    }
    rows = bars * barLen;

    if (bars > MAX_BARS) {
        printf("Warning: song too long (%d bars), truncating to %d.\n", bars, MAX_BARS);
        bars = MAX_BARS;
        rows = bars * barLen;
    }
    fclose(f);

    /* Decode notes into per-row Track arrays. */
    for (i = tracks = 0; i < 16; ++i) {
        if (!t[i].notes) continue;

        n[tracks] = malloc(rows * sizeof(struct Track));
        memset(n[tracks], 0, rows * sizeof(struct Track));

        for (j = 0; j < t[i].notes; ++j) {
            k = (int)note[i][j].start;
            if (k >= rows) continue;

            int vol = note[i][j].vol;
            int pan = note[i][j].pan;
            vol = (vol == 0xff) ? VOL : (int)((vol / 255.0) * 56.5 + 8.499);
            pan = (pan == 0xff) ? 0   : (pan - 6) * 127 / 6;

            if (note[i][j].key != 0xff) {
                n[tracks][k].freq = (float)(t[i].freqShift - 1000);
                if (t[i].drum)
                    n[tracks][k].freq += 800 * note[i][j].key + 100;
                else
                    n[tracks][k].freq += (float)(8363 * pow(2, note[i][j].key / 12.0));

                int len = note[i][j].len;
                if (t[i].noLoop) len = 16;
                do {
                    n[tracks][k].vol = (uint8_t)vol;
                    n[tracks][k].pan = (int8_t)pan;
                } while (--len && ++k < rows && !(n[tracks][k].freq));
            } else {
                for (; k < rows && n[tracks][k].vol && !(n[tracks][k].freq); ++k) {
                    if (note[i][j].vol != 0xff) n[tracks][k].vol = (uint8_t)vol;
                    if (note[i][j].pan != 0xff) n[tracks][k].pan = (int8_t)pan;
                }
            }

            t[tracks] = t[i]; /* carry instrument info forward */
        }
        ++tracks;
        free(note[i]);
    }

    /* Find best XM tempo/BPM pair. */
    unsigned bestTempo = 0, bestBPM = 0;
    unsigned bestE = (unsigned)-1;
    for (i = 1; i < 32; ++i) {
        unsigned bpm = 2500 * i / header.msPerBeat;
        if (bpm > 31 && bpm < 256) {
            unsigned e = (unsigned)abs((int)(2500000 * i / bpm) - (int)(header.msPerBeat * 1000));
            if (bestE > e || (bestE == e && abs((int)bestBPM - 125) > abs((int)bpm - 125))) {
                bestTempo = i; bestBPM = bpm; bestE = e;
            }
        }
    }
    if (!bestTempo) { printf("Speed out of XM range!\n"); return 5; }

    /* Find best finetune per track. */
    for (i = 0; i < tracks; ++i) {
        double bestFE = 1e30;
        int8_t bestFinetune = 0;
        for (k = -64; k < 64; k++) {
            double e = 0;
            t[i].finetune = (int8_t)k;
            for (j = 0; j < rows; j++) {
                encode(i, i, j);
                if (wKey && key != 0x60) {
                    int ft2 = finetune;
                    if (!wFine) ft2 = 0;
                    double logfreq = log2(8363.0)
                        + (key + ft2 / 8.0 + t[i].finetune / 128.0 - t[i].drum * 36) / 12.0;
                    double d = log2(n[i][j].freq) - logfreq;
                    e += d * d + (ft2 ? 1e-8 : 0);
                }
                if (e > bestFE) goto nextk;
            }
            bestFinetune = (int8_t)k;
            bestFE = e;
            nextk: continue;
        }
        t[i].finetune = bestFinetune;
    }

    /* Assign XM instrument IDs.
     * Drums get 1 instrument.  Melody tracks get 4 (one per octave split).
     * Tracks sharing the same sample+finetune+noLoop share instruments. */
    for (i = 0; i < tracks; ++i) t[i].instrument = (uint8_t)i;

    for (i = 0; i < tracks; ++i) if (t[i].instrument == i)
        for (j = i + 1; j < tracks; ++j)
            if (t[j].sample == t[i].sample &&
                t[j].finetune == t[i].finetune &&
                t[j].noLoop == t[i].noLoop)
                t[j].instrument = (uint8_t)i;

    memset(waveInstMap, 0, sizeof(waveInstMap));
    instruments = 0;

    for (i = 0; i < tracks; ++i) {
        if (t[i].instrument != i) continue;
        if (t[i].drum) {
            ++instruments;
            for (int o = 0; o < 4; o++) waveInstMap[i][o] = instruments;
        } else {
            for (int o = 0; o < 4; o++) {
                ++instruments;
                waveInstMap[i][o] = instruments;
            }
        }
    }
    /* Propagate shared instrument ids. */
    for (i = 0; i < tracks; ++i) {
        if (t[i].instrument != i) {
            int m = t[i].instrument;
            for (int o = 0; o < 4; o++) waveInstMap[i][o] = waveInstMap[m][o];
        }
    }

    /* Build XM patterns (one bar per pattern slot, dedup later). */
    uint8_t *pat[MAX_BARS];
    int      patLen[MAX_BARS];
    memset(chState, 0, sizeof(chState));

    for (k = 0; k < bars; ++k) {
        int len;
        uint8_t *buf = pat[k] = malloc(5 * barLen * tracks + 9);
        memset(buf, 0, 5 * barLen * tracks + 9);
        *(uint32_t *)&buf[0] = len = 9;
        *(uint16_t *)&buf[5] = (uint16_t)barLen;

        for (j = k * barLen; j < (k + 1) * barLen; ++j)
        for (i = 0; i < tracks; ++i) {
            encode(i, i, j);

            /* Pick the right octave-split instrument for this note. */
            int final_inst = 0;
            if (wInst && wKey && key != 0x60) {
                int oct;
                if (t[i].drum) {
                    oct = 0;
                } else {
                    oct = key / 24;
                    if (oct < 0) oct = 0;
                    if (oct > 3) oct = 3;
                }
                final_inst = waveInstMap[i][oct];
            } else if (wInst) {
                final_inst = waveInstMap[i][0];
            }

            uint8_t p = 0x80
                | wKey
                | (wInst * 2)
                | ((wVol || wPanVol) * 4)
                | ((wPan || wSkip || wFine) * 24);
            if (p != 0x9F) buf[len++] = p;

            if (wKey)              buf[len++] = (uint8_t)(key + 1);
            if (wInst)             buf[len++] = (uint8_t)final_inst;
            if (wVol)              buf[len++] = (uint8_t)(0x10 + n[i][j].vol);
            else if (wPanVol)      buf[len++] = (uint8_t)(0xC0 + (n[i][j].pan > 0x77 ? 0xF : (n[i][j].pan + 0x88) >> 4));

            if      (wSkip) { buf[len++] = 0x0B; buf[len++] = (uint8_t)(header.loopStart / barLen); }
            else if (wPan)  { buf[len++] = 0x08; buf[len++] = (uint8_t)(n[i][j].pan + 0x80); }
            else if (wFine) { buf[len++] = 0x0E; buf[len++] = (uint8_t)(0x58 + finetune); }
        }
        *(uint16_t *)&buf[7] = (uint16_t)(len - 9);
        patLen[k] = len;
    }

    /* Deduplicate patterns. */
    int pattern_map[MAX_BARS];
    int source_bars[MAX_BARS];
    int unique_patterns = 0;
    memset(pattern_map, -1, sizeof(pattern_map));

    for (i = 0; i < bars; i++) {
        int found = -1;
        for (int p = 0; p < unique_patterns; p++) {
            int src = source_bars[p];
            if (patLen[i] == patLen[src] && !memcmp(pat[i], pat[src], patLen[i])) {
                found = p; break;
            }
        }
        if (found != -1) {
            pattern_map[i] = found;
        } else {
            pattern_map[i]          = unique_patterns;
            source_bars[unique_patterns] = i;
            unique_patterns++;
        }
    }

    /* Open output file: replace last 3 chars of argv[1] with "xm". */
    size_t alen = strlen(argv[1]);
    argv[1][alen - 3] = 'x';
    argv[1][alen - 2] = 'm';
    argv[1][alen - 1] = '\0';
    if (!(g = fopen(argv[1], "wb"))) {
        printf("Couldn't open output \"%s\"\n", argv[1]);
        return 2;
    }

    /* Module name from filename (strip path). */
    int ni = (int)strlen(argv[1]);
    while (ni > 0 && argv[1][ni - 1] != '\\' && argv[1][ni - 1] != '/') --ni;
    argv[1][strlen(argv[1]) - 3] = '\0'; /* strip ".xm" for display */
    size_t namelen = strlen(&argv[1][ni]);
    memcpy(xmh.moduleName, &argv[1][ni], namelen > 20 ? 20 : namelen);

    xmh.songLength      = (uint16_t)bars;
    xmh.restartPosition = (uint16_t)(header.loopStart / barLen);
    xmh.channels        = (uint16_t)tracks;
    xmh.patterns        = (uint16_t)unique_patterns;
    xmh.instruments     = (uint16_t)instruments;
    xmh.flags           = 1;
    xmh.tempo           = (uint16_t)bestTempo;
    xmh.bpm             = (uint16_t)bestBPM;

    memset(xmh.patternOrder, 0, 256);
    int orders = bars < 256 ? bars : 256;
    for (i = 0; i < orders; i++)
        xmh.patternOrder[i] = (uint8_t)pattern_map[i];

    write(&xmh, sizeof(struct XMHeader));

    for (i = 0; i < unique_patterns; ++i)
        write(pat[source_bars[i]], patLen[source_bars[i]]);

    /* Write instruments. */
    for (i = 0; i < tracks; ++i) {
        if (t[i].instrument != i) continue;

if (t[i].drum) {
    uint8_t dsmp = t[i].sample - 100;
    int8_t *src  = &sbank.drums[sbank.tblOffDrum[dsmp]];
    uint32_t len = sbank.tblLenDrum[dsmp];

    int8_t *delta = malloc(len);
    for (uint32_t d = len - 1; d > 0; --d)
        delta[d] = src[d] - src[d - 1];
    delta[0] = src[0] ^ 0x80;

    smp.type         = 0;
    smp.loopStart    = 0;
    smp.loopLength   = 0;
    smp.sampleLength = len;
    smp.finetune     = t[i].finetune;
    smp.relativeKey  = 12;
    smp.volume       = VOL;
    memset(smp.instrumentName, 0, 22);
    strncpy((char *)smp.instrumentName, sbank.tblNameDrum[dsmp], 21);

    write(&smp, sizeof(struct XMInstrument));
    write(delta, len);
    free(delta);
        } else {
            /* Write 4 octave-split samples for this melody instrument. */
            int8_t *src_pcm = &sbank.melody[t[i].sample * sbank.lenMelo];

            for (int o = 0; o < 4; o++) {
                int wave_size = oct_wave[o].wave_size;

                /* Resample the 256-sample linear PCM source to wave_size samples,
                 * then delta-encode for XM. */
                int8_t *dst = malloc(wave_size);
                unsigned long step   = 0x100 / (unsigned)wave_size; /* always integer */
                unsigned long wav_tp = 0;
                int8_t prev = 0;

                for (int x = 0; x < wave_size; x++) {
                    int8_t val = src_pcm[wav_tp & 0xFF];
                    dst[x]     = val - prev;
                    prev       = val;
                    wav_tp    += step;
                }

                smp.type         = 1; /* forward loop */
                smp.loopStart    = 0;
                smp.sampleLength = (uint32_t)wave_size;
                smp.loopLength   = (uint32_t)wave_size;
                smp.finetune     = t[i].finetune;
                smp.relativeKey  = split_relkey[o];
                smp.volume       = VOL;
                memset(smp.instrumentName, 0, 22);
                snprintf((char *)smp.instrumentName, 22, "Mel%02d-Oct%d", t[i].sample, o * 2);

                write(&smp, sizeof(struct XMInstrument));
                write(dst,   wave_size);
                free(dst);
            }
        }
    }

    /* Cleanup. */
    fclose(g);
    for (i = 0; i < tracks; ++i) free(n[i]);
    for (k = 0; k < bars;   ++k) free(pat[k]);

    printf("Done. %d tracks, %d instruments, %d unique patterns.\n",
           tracks, instruments, unique_patterns);
    return 0;
}
