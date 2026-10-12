"""Reconstruct edited Mario animation arrays from their original US ROM tracks."""
import struct


def reconstruct_animation_array(rom, spec):
    """Return big-endian bytes; spec contains source ID, array kind and timing edits."""
    table = 0x4EC000
    entry_count, unused = struct.unpack_from('>2I', rom, table)
    source_id = spec['animation_id']
    if not 0 <= source_id < entry_count:
        raise ValueError('Animation ID outside original ROM table')
    relative, length = struct.unpack_from('>2I', rom, table + 8 + 8 * source_id)
    address = table + relative
    header = struct.unpack_from('>6h3I', rom, address)
    channel_count = (header[5] + 1) * 3
    native_indices = struct.unpack_from('>' + str(channel_count * 2) + 'H',
                                        rom, address + header[7])
    native_value_count = max(count + offset for count, offset in
                             zip(native_indices[::2], native_indices[1::2]))
    native_values = struct.unpack_from('>' + str(native_value_count) + 'H',
                                       rom, address + header[6])
    indices = []
    values = []
    remapped = {}
    tracks = [(offset, count, channel) for channel, (count, offset) in
              enumerate(zip(native_indices[::2], native_indices[1::2]))]
    sequential = spec.get('storage_order') == 'channels'
    if not sequential:
        tracks = sorted({offset: (offset, count, channel)
                         for offset, count, channel in tracks}.values())
    for offset, count, channel in tracks:
        samples = list(native_values[offset:offset + count])
        if channel == 1 and 'root_y_translation' in spec:
            samples = [(value + spec['root_y_translation']) & 65535 for value in samples]
        if count > 1:
            if spec.get('insert_midpoints'):
                expanded = []
                for first, second in zip(samples, samples[1:]):
                    if channel < 3:
                        first_signed = (first + 32768) % 65536 - 32768
                        second_signed = (second + 32768) % 65536 - 32768
                        delta = second_signed - first_signed
                    else:
                        delta = (second - first + 32768) % 65536 - 32768
                    expanded.extend((first, (first + delta // 2) & 65535))
                samples = expanded + samples[-1:]
            elif 'frames' in spec:
                samples = [samples[min(frame, count - 1)] for frame in spec['frames']]
        remapped[offset] = (len(samples), len(values))
        if sequential:
            indices.extend(remapped[offset])
        values.extend(samples)
    if not sequential:
        for offset in native_indices[1::2]:
            indices.extend(remapped[offset])
    result = indices if spec['array'] == 'indices' else values
    return struct.pack('>' + str(len(result)) + 'H', *result)
