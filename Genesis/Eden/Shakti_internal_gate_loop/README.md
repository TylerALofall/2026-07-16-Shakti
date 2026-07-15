# Shakti_internal_gate_loop

Complete, law-clean C17 runtime gate code and build-time conversion tools belong
here. `converter/` holds the tested legacy staging converter. It never writes to
the tablet master or admits unresolved data. `dissector/` holds the tested exact
number-first router; `dissector/lexicon/` holds the fixed-buffer build converter
and malloc-free resident lookup for the exact four-column word matrix. They
separate math candidates, ordinary number context, and language before deeper
word dissection. NO PYTHON.
