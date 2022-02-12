import os
import re
import sys
import subprocess as sp

_exec = '.\\x64\\Release\\BandB.exe'

assert (len(sys.argv) > 1), "An argument must be given"

## instdir informations

instdir = sys.argv[1]

def list_files_recursive(path):
    """
    Function that receives as a parameter a directory path
    :return list_: File List and Its Absolute Paths
    """
    files = []

    # r = root, d = directories, f = files
    for r, d, f in os.walk(path):
        for file in f:
            files.append(os.path.join(r, file))

    lst = [file for file in files]
    return lst

dats = list_files_recursive(instdir)
# header
print('n','dL','dU','corrLevel','num','obj','time')
for dat in dats:
	if dat.find('.dat') != -1:
		n,dL,dU,corrLevel,num = re.search('([0-9]+)orders_dL_(.*)_dU_(.*)_(.*)_(.*).dat',os.path.basename(dat)).groups(0)
		cmd = _exec+" "+dat
		
		p = sp.Popen(cmd, shell=True, stdout=sp.PIPE)

		output = p.stdout.read().decode('utf-8').replace('\n','')
		print(n+' '+dL+' '+dU+' '+corrLevel+' '+num+' '+output)
