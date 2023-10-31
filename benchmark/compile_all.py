import os
import glob
from pathlib import Path

algos = glob.glob("algorithms/*.c")
for algo in algos:
    algo = Path(algo)
    #print( os.popen(f'make PROG_NAME={algo.stem} clean').read() )
    print( os.popen(f'make PROG_NAME={algo.stem}').read() )
    print(f"Compiled {algo.stem}")