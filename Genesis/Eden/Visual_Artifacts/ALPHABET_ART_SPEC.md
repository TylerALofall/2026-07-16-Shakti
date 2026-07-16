# LEVEL 2 ALPHABET VISUAL-ART SUBMISSION CONTRACT

This file governs supplementary alphabet pictures submitted to
`Genesis/Eden/Visual_Artifacts/`. It does not replace the frozen 8x8
`visual_text` glyph. Exact character identity remains the ASCII character and
its frozen `uint64_t`; these larger SVGs are a second visual experience.

## THREE COMPLETE SETS

| Set ID | Contents | Exact filename example | Count |
|---|---|---|---:|
| `block` | one upright uppercase letter | `lvl2_ABCs_block_A.svg` | 26 |
| `italic` | one italic lowercase letter | `lvl2_ABCs_italic_a.svg` | 26 |
| `pair` | uppercase and lowercase together | `lvl2_ABCs_pair_Aa.svg` | 26 |

Total when complete: 78 SVG files. `italic` is the current interpretation of
the founder's voice transcription "elastic." If the founder meant literal
elastic deformation or animation, stop: animated SVG is not accepted and a
different signed set name is required.

## EXACT CANVAS

- File type: plain SVG 1.1-compatible XML, UTF-8, LF line endings.
- Root size: `width="512" height="512" viewBox="0 0 512 512"`.
- Background: one full-canvas white rectangle, `fill="#FFFFFF"`.
- Foreground: near-black solid paths, `fill="#111111"`.
- Safe area: no visible part outside x=64..448 or y=64..448.
- Maximum file size: 262144 bytes per SVG.
- Single-letter set: one letter, optically centered, at least 320 pixels tall.
- Pair set: uppercase centered in the left half and lowercase in the right half;
  both share one visible baseline and neither touches the other.

The 512-pixel canvas is the exchange and inspection size. SVG remains scalable.
The caller may enlarge it for the founder without changing the stored file.

## CONTENT RULES

Each file must contain:

1. one `<title>` naming the set and character;
2. one `<desc>` stating that it is supplementary Level 2 visual art;
3. the white background rectangle; and
4. closed vector paths for the visible letter or pair.

Refused content:

- `<text>`, `font-family`, web fonts, or dependence on an installed font;
- scripts, event handlers, animation, CSS imports, external links, or network URLs;
- embedded raster images, base64 payloads, filters, masks, or clipping that hides
  additional content;
- a musical note, picture-word example, tone, or pronunciation inside the letter
  asset; those converge through their own exact channels;
- a filename that differs only by filesystem case from another filename.

Paths may be artist-built, but the submission must name the master used to
check letter identity and the license permitting publication. Model output is
not its own authority.

## NAMING

Names are case-sensitive and carry no date:

```text
lvl2_ABCs_block_<A-Z>.svg
lvl2_ABCs_italic_<a-z>.svg
lvl2_ABCs_pair_<A-Z><a-z>.svg
```

Examples:

```text
lvl2_ABCs_block_A.svg
lvl2_ABCs_italic_a.svg
lvl2_ABCs_pair_Aa.svg
lvl2_ABCs_block_Z.svg
lvl2_ABCs_italic_z.svg
lvl2_ABCs_pair_Zz.svg
```

## SUBMISSION

1. Create files to this contract without changing their final names.
2. Fill one row per file using `TEMPLATE_alphabet_submission.tsv`.
3. Submit the SVG files and completed TSV together.
4. Do not edit tablet XML or `INDEX_MASTER.xml` during art submission.
5. Review checks filename, byte size, root dimensions, prohibited elements,
   visible letter identity, master source, license, and exact SHA-256.
6. Only reviewed files are deposited and signed. A failed file is returned with
   one exact reason and does not enter the repository.

When audio is admitted later, tone and founder voice begin together at the
convergence beat. Their separate masters are preserved even when the build
produces one combined training WAV.
