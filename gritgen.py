import os, os.path, subprocess

def bytes_to_int(bytes):
    result = 0
    for b in bytes:
        result = (result << 8) + int(b)
    return result

def gen_mdt(fn, offset):
	with open(fn, "rb") as f:
		f.read(16)
		width = bytes_to_int(f.read(4)) >> 3
		height = bytes_to_int(f.read(4)) >> 3
	outfn = fn[:-4] + ".mdt"
	with open(outfn, "w") as f:
		f.write("input ../" + fn + "\n")
		f.write("output ../" + fn[:-4] + ".pat" + "\n")
		f.write("output2 ../" + fn[:-4] + ".map" + "\n")
		f.write("offset " + str(offset) + "\n")
		f.write("map 0 0 " + str(width) + " " + str(height) + "\n")

if __name__ == '__main__':
	folder = "../res/Stage/"
	folders = ['White/', 'Mimi/', 'Maze/', 'Hell/', 'Eggs/']
	wd = os.getcwd()
	os.chdir(folder)
	for fn in os.listdir("."):
		if fn.startswith("Prt") and fn.endswith(".png"):
			print(subprocess.run(['grit', fn, '-gt', '-gB4', '-ftb', '-m!', '-fh!']))
	for f in folders:
		os.chdir(wd)
		os.chdir(folder + f)
		for fn in os.listdir("."):
			if fn.startswith("Prt") and fn.endswith(".png"):
				print(subprocess.run(['grit', fn, '-gt', '-gB4', '-ftb', '-m!', '-fh!']))
