#!/usr/bin/env python3
"""Add smpl chunks to WAV files to mark them as looping.

Usage: python3 patch_loop.py <wav_file> [wav_file ...]

This adds a 'smpl' chunk with a forward loop from sample 0 to the end
of the sample data. mmutil reads this chunk and sets repeat_mode=1
in the compiled soundbank.
"""

import struct, sys, os

def add_smpl_loop(wav_path):
    with open(wav_path, 'rb') as f:
        data = bytearray(f.read())

    riff = data[:4]
    if riff != b'RIFF':
        print(f"  SKIP: not a RIFF/WAV file")
        return False

    # Read the WAV to find sample rate and data size
    pos = 12  # skip RIFF header + size + WAVE
    sample_rate = 0
    data_size = 0
    fmt_found = False
    data_found = False
    channels = 1
    bits_per_sample = 8

    while pos + 8 <= len(data):
        chunk_id = data[pos:pos+4]
        chunk_size = struct.unpack_from('<I', data, pos+4)[0]

        if chunk_id == b'fmt ':
            fmt_found = True
            audio_format = struct.unpack_from('<H', data, pos+8)[0]
            channels = struct.unpack_from('<H', data, pos+10)[0]
            sample_rate = struct.unpack_from('<I', data, pos+12)[0]
            bits_per_sample = struct.unpack_from('<H', data, pos+22)[0]
        elif chunk_id == b'data':
            data_found = True
            data_size = chunk_size
            # Don't break - keep going to find any existing smpl chunk

        pos += 8 + chunk_size
        if chunk_size % 2:
            pos += 1  # padding byte

    if not fmt_found or not data_found:
        print(f"  SKIP: incomplete WAV header")
        return False

    # Calculate sample frames
    frame_size = channels * (bits_per_sample // 8)
    num_samples = data_size // frame_size

    # Build smpl chunk
    # Fields: manufacturer(4), product(4), sample_period(4),
    #         unity_note(4), pitch_frac(4), smpte_fmt(4), smpte_off(4),
    #         num_loops(4), sampler_data(4)
    # Loop: cue_id(4), loop_type(4), loop_start(4), loop_end(4),
    #        fraction(4), play_count(4)

    # Sample period in nanoseconds = 1e9 / sample_rate
    sample_period = int(1_000_000_000 / sample_rate) if sample_rate > 0 else 0

    smpl_data = struct.pack('<IIIIIIII',
        0,                    # manufacturer
        0,                    # product
        sample_period,        # sample period (ns)
        60,                   # MIDI unity note (C4)
        0,                    # MIDI pitch fraction
        0,                    # SMPTE format
        0,                    # SMPTE offset
        1,                    # num sample loops
    )
    smpl_data += struct.pack('<I', 0)  # sampler data
    smpl_data += struct.pack('<IIIIII',
        0,                    # cue point ID
        0,                    # loop type (0 = forward)
        0,                    # loop start
        num_samples,          # loop end
        0,                    # fraction
        0,                    # play count
    )

    # Remove existing smpl chunk if present
    pos = 12
    new_data = bytearray(data[:12])  # Keep RIFF header up to WAVE
    while pos + 8 <= len(data):
        chunk_id = data[pos:pos+4]
        chunk_size = struct.unpack_from('<I', data, pos+4)[0]
        chunk_end = pos + 8 + chunk_size
        if chunk_size % 2:
            chunk_end += 1  # padding

        if chunk_id != b'smpl':
            new_data.extend(data[pos:chunk_end])

        pos = chunk_end

    # Insert smpl chunk before the data chunk
    # Find where data chunk starts in new_data
    smpl_insert_pos = new_data.find(b'data', 12)
    if smpl_insert_pos < 0:
        print(f"  ERROR: data chunk not found")
        return False

    smpl_chunk = struct.pack('<4sI', b'smpl', len(smpl_data)) + smpl_data
    new_data[smpl_insert_pos:smpl_insert_pos] = smpl_chunk

    # Update RIFF size
    riff_size = len(new_data) - 8
    struct.pack_into('<I', new_data, 4, riff_size)

    # Write
    with open(wav_path, 'wb') as f:
        f.write(new_data)

    print(f"  PATCHED: {os.path.basename(wav_path)} ({num_samples} samples, {sample_rate}Hz, {channels}ch, {bits_per_sample}bit)")
    return True

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python3 patch_loop.py <wav_file> [wav_file ...]")
        sys.exit(1)

    for path in sys.argv[1:]:
        print(f"Processing {path}...")
        add_smpl_loop(path)
