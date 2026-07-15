# Eden legacy tablet staging converter

This build-time C17 tool extracts recoverable facts from the old tablet XML.
It never writes into `Tablet_Training_Artifacts_Master` and it never admits a
row to the resident floor.

## Programs

- `eden_import_old.c` reads XML with a fixed-buffer `fgetc` state machine. It
  has no `malloc`, accepts multiline tags, decodes XML entities, sorts exact
  identities, refuses malformed XML and duplicate identities, and writes a
  table plus receipt.
- `eden_factor_color.c` proves that a color/object pair table is a complete
  Cartesian product, then emits the unique color and object tables. It refuses
  duplicates, missing combinations, and unsorted input.

Supported legacy domains are `counting`, `alphabet`, `colors`, `shapes`, and
`color_example`. Unresolved legacy facts remain explicitly unresolved. For
example, the old color tablet has empty swatches, so it cannot be admitted.

## Build

macOS, Linux, or a POSIX shell with a C17 compiler:

```text
sh build_c.sh
```

Windows with Clang in `PATH`:

```text
build_c.bat
```

Both builds write binaries to a temporary directory by default, not the repo.
Set `CC` to select another C17 compiler and pass an output directory if needed.

## Use

```text
eden_import_old OLD_TABLET.xml STAGING.tbl
eden_factor_color COLOR_OBJECTS.tbl COLORS.tbl OBJECTS.tbl RECEIPT.txt
```

Generated tables are staging evidence only. A later admission step must resolve
required visual/audio facts, generate the four fixed load slots, validate every
path, and freeze the approved tables into C.
