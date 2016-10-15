#!/usr/bin/env python2

import footgen

f = footgen.Footgen("EQFP144", output_format="geda")
f.pitch = 0.5
f.pins = 144
f.width = 20
f.padheight = 0.27
f.padwidth = 2.0
f.qfn()
f.silk_crop(20, pin1="circle")
f.add_pad("GND", 0, 0, 5.4, 5.4)
f.finish()

f = footgen.Footgen("PQFP100", output_format="geda")
f.pitch = 0.635
f.pins = 100
f.width = 19.05
f.padheight = 0.40
f.padwidth = 2.0
f.qfn()
f.silk_crop(19.05, pin1="circle")
f.finish()

f = footgen.Footgen("PSOP48", output_format="geda")
f.pins = 48
f.pitch = 0.5
f.width = 6.0
f.padheight = 0.3
f.padwidth = 2
f.so()
f.silk_crop(6, 13, pin1="circle")
f.finish()

f = footgen.Footgen("TSOP44", output_format="geda")
f.pins = 44
f.pitch = 0.8
f.width = 10.106
f.padheight = 0.4
f.padwidth = 2
f.so()
f.silk_crop(11, 20, pin1="circle")
f.finish()
