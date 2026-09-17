# DestroyFX v0.2 clock sync

Clock inputs accept ordinary Eurorack/MetaModule gate clocks. Rising edges above about 1 V are used.
The working v0.1 time-domain DSP is retained; the clock layer only changes scheduling/time controls.

## Transverb

A dedicated **Clock** input has been added. Once two pulses establish the tempo:

- Buffer length follows the measured clock period (clamped to Transverb's safe 0.02-10 s range).
- Every clock edge also resets both moving heads to the write position for repeatable alignment.
- The Buffer knob/CV resumes normal free-time control as soon as Clock is unplugged.

For divisions or multiplications, feed Transverb a divided/multiplied clock from another module.

## Scrubby

A dedicated **Clock** input has been added:

- Every rising edge requests a new scrub jump.
- Automatic Jump Rate is disabled while Clock is patched, so jumps remain locked to the incoming clock.
- Jump Amount, Range, Reverse, Inertia and all other controls remain active.

## Buffer Override

The existing Clock, Clock Lock and Clock Ratio system is retained unchanged.

## Skidder

The existing Clock, Clock Lock and Clock Ratio system is retained unchanged.

## Polarizer

Polarizer remains sample-pattern based rather than tempo based, so it does not add a separate Clock jack.
Its existing Reset Trigger can already be fed a clock if you want the polarity pattern restarted rhythmically.
