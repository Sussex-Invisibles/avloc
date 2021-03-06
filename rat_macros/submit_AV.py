#!/usr/bin/env python

import string
import os

scratch = "/mnt/lustre/epp_scratch/neutrino/ms711/avloc/avloc/rat_macros/Fibre33/"
template_macro = string.Template(file("AV_loc.mac", "r").read())
template_script = string.Template(file("script.sh", "r").read())

fibres = ["FT033A"]
disps = [0.0]

for fibre in fibres:
	for disp in disps:
		macro = template_macro.substitute(Disp = disp, Number = fibre, Events = 500)
		filename = os.path.join(scratch, "macros", "%.1d_Fibre_%s.mac" % (disp,fibre))
		fout = file(filename, "w")
		fout.write(macro)
		fout.close()
    
		for i in range(1,201):
        
			rootname = os.path.join(scratch, "outputs", "Fibre_%s_%.1d_%d" % (fibre, disp,  i))
			logname = os.path.join(scratch, "RATlogs", "Fibre_%s_%.1d_%d.log" % (fibre, disp, i))
			script = template_script.substitute(Macro = filename, Output = rootname, Log = logname)
			scriptname = os.path.join(scratch, "macros", "Fibre_%s_%.1d_%d.sh" % (fibre, disp, i))
			fout = file(scriptname, "w")
			fout.write(script)
			fout.close()
                        print "qsub %s" % scriptname
			os.system("qsub %s" % scriptname)

