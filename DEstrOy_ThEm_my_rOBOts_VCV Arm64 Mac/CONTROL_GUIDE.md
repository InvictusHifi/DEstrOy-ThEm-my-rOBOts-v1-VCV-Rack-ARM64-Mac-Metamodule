# DestroyFX MetaModule Clocked v0.2 control guide

## Common behaviour

All five modules are stereo. Patch only **Audio L** for mono-to-stereo normalisation, or patch both inputs for true stereo.

Every continuous parameter has:

- a manual base control;
- a dedicated CV jack;
- a matching **Amt** attenuverter.

The attenuverter range is -100% to +100%. A 5 V CV at +100% traverses the full normalised control range. Gate and trigger inputs switch at approximately 1 V.

## Transverb

A dual moving-head tape-loop-style delay that produces shifting, reversing, reverb-like and broken-buffer textures.

| Control | Range / action |
|---|---|
| Buffer | 0.02-10 seconds, curved response |
| Speed A/B | -4x to +4x playback speed |
| Delay A/B | 0-100% of the active loop |
| Level A/B | 0-125% |
| Feedback | 0-98.5%, internally soft-clipped |
| Cross FB | Cross-couples left/right feedback |
| Tomsound | Adds rough reads and intermittent buffer jumps |
| Spread | Moves from centred summing toward separated heads |
| Mix | Dry/wet |
| Freeze | Stops writing while the read heads continue |
| Reset | Repositions both heads at the current write point |
| Clock | Syncs buffer length to measured clock period and resets heads on each rising edge |

Safe first patch: Buffer 2 s, Speed A +1x, Speed B -1x, Feedback 0, Tomsound 0, Mix 50%.

## Scrubby

A live-buffer robot-DJ scrubber. It continuously reads a moving point in the recent audio and can make automatic or triggered jumps.

| Control | Range / action |
|---|---|
| Buffer | 0.02-10 seconds, curved response |
| Speed | -4x to +4x |
| Seek | Centre position within the live buffer |
| Range | Maximum distance available to automatic jumps |
| Jump Rate | 0-30 Hz, curved response |
| Jump Amt | Jump distance |
| Inertia | Slews changes in playback speed |
| Quantize | Free / chromatic / octaves / fifths |
| Reverse | Probability that a new jump reverses direction |
| Feedback | 0-97%, internally soft-clipped |
| Spread | Expands stereo difference in the scrubbed signal |
| Mix | Dry/wet |
| Freeze | Stops recording new audio into the buffer |
| Jump | Manual jump trigger |
| Reset | Returns playback to a stable starting point |
| Clock | Triggers one scrub jump per rising edge; automatic Jump Rate pauses while patched |

Safe first patch: Speed +1x, Jump Rate low, Jump Amt 25%, Inertia 40%, Feedback 0.

## Buffer Override

A clockable minibuffer repeater. It captures a region, divides it into a smaller repeated fragment and optionally randomises, reverses and decays successive repeats.

| Control | Range / action |
|---|---|
| Buffer | 0.005-10 seconds, curved response |
| Divisor | 1-64 subdivisions |
| Duty | Portion of each repeat that remains audible |
| Smooth | Edge crossfade to reduce or exaggerate clicks |
| Decay | Repeat-level retention |
| Random | Random variation in minibuffer size |
| Reverse | Probability of reverse playback for each fragment |
| Feedback | 0-97%, internally soft-clipped |
| Clock Ratio | 1/8, 1/4, 1/2, 1, 2, 4 or 8 times the measured clock |
| Mix | Dry/wet |
| Clock Lock | Uses external clock period for capture length |
| Freeze | Stops writing new input |
| Capture | Immediately takes a new capture |

Safe first patch: Divisor 8, Duty 100%, Smooth 12%, Decay 100%, Random/Reverse/Feedback 0.

## Skidder

A regular or random stereo amplitude gate.

| Control | Range / action |
|---|---|
| Rate | 0.02-50 Hz, curved response |
| Width | 1-99% on-time |
| Slope | Fade length at the edges of each open section |
| Floor | Level while the gate is nominally closed |
| Random | Randomises rate and width from cycle to cycle |
| Stereo | Phase offset between left and right gates |
| Clock Ratio | 1/8, 1/4, 1/2, 1, 2, 4 or 8 |
| Mix | Dry/wet |
| Clock Lock | Derives cycle duration from external clock |
| Reset | Restarts the gate cycle |

Safe first patch: Rate 4 Hz, Width 50%, Slope 15%, Floor 0, Random 0.

## Polarizer

A sample-pattern polarity processor. Instead of reducing bit depth, it selectively flips the sign of samples, producing crisp digital sidebands and noise-like textures.

| Control | Range / action |
|---|---|
| Interval | 1-512 samples between pattern restarts |
| Burst | Number of affected samples within each interval |
| Probability | Chance of applying the current inversion event |
| Mode | Three inversion-pattern variants |
| Stereo | Offsets the right-channel pattern |
| Drive | Adds bounded pre/post waveshaping |
| Mix | Dry/wet |
| Reset | Restarts the sample pattern |

Safe first patch: Interval 8, Burst 1, Probability 100%, Mode 0, Drive 0, Mix 50%.


See `CLOCK_SYNC_GUIDE.md` for exact external-clock behaviour.
