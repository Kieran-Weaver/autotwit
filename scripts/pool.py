import os
import sys
# Do multiple downloads in parallel
from concurrent.futures import ThreadPoolExecutor

# Constants
MAX_WORKERS = 4
ZSTD_LEVEL = 22

cmds = []
YEAR = ""
MONTH = ""

if len(sys.argv) > 2:
	YEAR = str(sys.argv[-2])
	MONTH = str(sys.argv[-1]).zfill(2)
else:
	raise ValueError("USAGE: python3 scripts/pool.py YEAR MONTH")

URLYEAR = YEAR
if int(YEAR) == 2019 and int(MONTH) > 9:
	URLYEAR = 2018 # Months 10, 11, and 12 of the 2019 archive are stored in the wrong place

cmdprefix = f"wget -qO - https://archive.org/download/archiveteam-twitter-stream-{URLYEAR}-{MONTH}/"

if int(YEAR) == 2019 and int(MONTH) == 9: # Special case 2019-09
	cmdprefix = f"wget -qO - https://archive.org/download/archiveteam-twitter-stream-{URLYEAR}-08/"

for i in range(1, 32):
	cmdsuffix = f" | tar -xOf - | lbzcat -n 2 | ./tools/wayback | sort -u | zstd -z -T4 -{ZSTD_LEVEL} -q -o {YEAR}_{MONTH}_{str(i).zfill(2)}.csv.zstd"
	if int(YEAR) == 2018:
		if int(MONTH) < 4:
			raise ValueError("For 2018 Jan, Feb, and Mar, archive.org provides the entire month as a single tarball. Extract the single large tarball manually")
		elif int(MONTH) < 11:
			cmds.append(cmdprefix + f"twitter-{YEAR}-{MONTH}-{str(i).zfill(2)}.tar" + cmdsuffix)
		else:
			cmds.append(cmdprefix + f"twitter_stream_{YEAR}_{MONTH}_{str(i).zfill(2)}.tar" + cmdsuffix)
	elif int(YEAR) == 2019:
		cmds.append(cmdprefix + f"twitter_stream_{YEAR}_{MONTH}_{str(i).zfill(2)}.tar" + cmdsuffix)
	elif int(YEAR) == 2020:
		if int(MONTH) <= 6:
			cmds.append(cmdprefix + f"twitter_stream_{YEAR}_{MONTH}_{str(i).zfill(2)}.tar" + cmdsuffix)
		else:
			raise ValueError(f"{YEAR}-{MONTH} is distributed as zip files")
	else:
		raise ValueError(f"Invalid Year: {YEAR}")

with ThreadPoolExecutor(max_workers = MAX_WORKERS) as e:
	e.map(os.system, cmds)

if int(YEAR) == 2018 and int(MONTH) == 4:
	raise ValueError("April 2018 is split into a single large tarball and smaller tarballs. Extract the single large tarball manually")
