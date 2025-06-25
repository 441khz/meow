import sys
import textwrap

import numpy as np
from PIL import Image, ImageOps

def pack_u16s(chunks):
    output = []
    for chunk in chunks:
        assert(len(chunk) == 16)

        out = 0
        for i in range(0, len(chunk)):
            out |= chunk[i] << (15 - i)

        output.append(out)
    return output

def split(l, size):
    assert(len(l) % size == 0)
    return [l[x:x + 16] for x in range(0, len(l), size)]

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f'usage: {sys.argv[0]} [input.bmp] [output.h]')
        exit(1)

    img = Image.open(sys.argv[1])

    # Convert to grayscale and calculate palette
    gs = ImageOps.grayscale(img)
    gs_2b = gs.quantize(colors = 4, method = Image.Quantize.LIBIMAGEQUANT)
    gs_uniq, gs_ind = np.unique(gs_2b.getpalette(), return_index = True)
    gs_sorted = np.argsort(gs_uniq[np.argsort(gs_ind)])
    assert(len(gs_sorted) == 4)

    def mapper(pixel):
        if pixel == gs_sorted[0]:
            return (1, 1) # black
        elif pixel == gs_sorted[1]:
            return (1, 0) # dark grey
        elif pixel == gs_sorted[2]:
            return (0, 1) # light grey
        else:
            return (0, 0) # white

    assert(gs_2b.height % 16 == 0)
    assert(gs_2b.width % 16 == 0)

    pixels = []
    # Reorganize so pixels are linear
    for y in range(0, gs_2b.height, 16):
        for x in range(0, gs_2b.width, 16):
            for i in range(0, 16):
                offset = x + ((i + y) * gs_2b.width)
                pixels += list(gs_2b.getdata())[offset:offset + 16]

    assert(len(pixels) % 16 == 0)

    # Convert to planes
    plane1, plane2 = map(list, zip(*[mapper(p) for p in pixels]))
    plane1_u16s = pack_u16s(split(plane1, 16))
    plane2_u16s = pack_u16s(split(plane2, 16))

    # Split again and interleave
    combined = zip(split(plane1_u16s, 16), split(plane2_u16s, 16))

    arrays = []
    for (l, r) in combined:
        plane1 = f"{{ {', '.join([f'0x{x:04x}' for x in l])} }}"
        plane2 = f"{{ {', '.join([f'0x{x:04x}' for x in r])} }}"

        formatted = f'''
        {{
            {plane1},
            {plane2}
        }},
        '''.lstrip()
        arrays.append(formatted)

    # Formatting fix
    arrays[-1] = arrays[-1].strip()

    # Emit header
    template = f'''
    #pragma once

    #include <stdint.h>

    const uint16_t {sys.argv[2].split('.')[0]}[{len(arrays)}][2][16] =
    {{
        {''.join(arrays)}
    }};
    '''

    with open(sys.argv[2], 'w', encoding = 'ascii') as fp:
        fp.write(textwrap.dedent(template).lstrip())
