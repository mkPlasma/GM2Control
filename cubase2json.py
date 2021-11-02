# Use this to convert a Cubase MIDI parse file to a GM2Control-compatible JSON tone list file

import sys
import json
import re
import os.path


# Check argument count
if(len(sys.argv) < 2):
	print("Usage: cubase2json.py [input file] [output file]")
	quit()


# Open input file
file_in = open(sys.argv[1], "r")
lines = file_in.readlines()

json_tones = {}

last_category = ""

for line in lines:
	
	# Category, match [gX] or [mode]
	result = re.search(r"(\[g(\d+)\]|\[mode\s*\d*\])([^\[\]\n]+)", line)
	
	if result:
		# Remove current category if there were no tones added
		if len(json_tones) > 0 and len(json_tones[last_category]) == 1:
			json_tones.pop(last_category)
		
		# Add category
		last_category = result.group(3)
		json_tones[last_category] = [{"index": len(json_tones)}]
		continue
	
	
	# Tone, match [pX, X, X, X]
	result = re.search(r"\[p\d+,\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\]([^\[\]\n]+)", line)
	
	# Add to current category
	if result and len(json_tones) > 0:
		msb = max(int(result.group(2)), 0)
		prefix = ""
		
		if msb == 80 or msb == 81:
			prefix = "[SP] "
		if msb == 96 or msb == 104:
			prefix = "[CL] "
		if msb == 97 or msb == 105:
			prefix = "[CT] "
		if msb == 98 or msb == 106:
			prefix = "[SL] "
		if msb == 99 or msb == 107:
			prefix = "[EN] "
		
		json_tones[last_category].append({
			"name": prefix + result.group(4),
			"pc": max(int(result.group(1)) + 1, 0),
			"msb": max(int(result.group(2)), 0),
			"lsb": max(int(result.group(3)), 0),
		})

# Save to output file
if(len(sys.argv) >= 3):
	file_out = open(sys.argv[2], "w+")
else:
	file_out = open(os.path.splitext(sys.argv[1])[0] + ".json", "w+")

json.dump(json_tones, file_out, indent=4)
