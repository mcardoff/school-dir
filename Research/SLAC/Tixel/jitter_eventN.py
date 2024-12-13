import setupLibPaths
#-----------------------------------------------------------------------------
# This file is part of the 'EPIX HR Firmware'. It is subject to
# the license terms in the LICENSE.txt file found in the top-level directory
# of this distribution and at:
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
# No part of the 'EPIX HR Firmware', including this file, may be
# copied, modified, propagated, or distributed except according to the terms
# contained in the LICENSE.txt file.
#-----------------------------------------------------------------------------

##############################################################################

# Python libraries
import os
import sys
top_level=f'{os.getcwd()}/'# point to the software folder
import argparse
import pyrogue as pr
import pyrogue.utilities.prbs
import pyrogue.utilities.fileio
import pyrogue.interfaces.simulation
import epix_hr_single_sparkPixT as sparkPixT
import pyrogue.gui
import rogue.protocols
import pyrogue.pydm
import rogue
import rogue.utilities.fileio
# import ePixFpga as fpga
import numpy as np
import statistics
import math
import time
from pathlib import Path
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import csv
from datetime import datetime
import click
from tkinter import filedialog as fd
from itertools import repeat
import multiprocessing as mp
import pickle

# Set the argument parser
parser = argparse.ArgumentParser()

parser.add_argument(
    "--datafilePath",
    type     = str,
    required = False,
    help     = "Path for the data file (overrides the GUI)",
)

parser.add_argument(
    "--last",
    action   = "store_true",
    help     = "If set, script will analyze the last .pkl file entry of the current directory",
)

parser.add_argument(
    "--col",
    type     = int,
    required = False,
    choices = range(192),
    help     = "Pixel column number",
)

parser.add_argument(
    "--row",
    type     = int,
    required = False,
    choices = range(176),
    help     = "Pixel row number",
)

parser.add_argument(
    "--TDCclk",
    type     = int,
    required = False,
    default   = 158.6, #139.3016344,
    help     = "TDC reference clock frequency in MHz",
)

##############################################################################
# Get the arguments
args = parser.parse_args()
##############################################################################

DataProc_Start = time.perf_counter()

# some global stuff
LSB = (1000000/args.TDCclk)/64
binW = 1

# Time-walk correction paramiter
TWc = 14000 

if args.datafilePath is None and args.last is False:
    from tkinter import filedialog as fd
    filenameData = fd.askopenfilename(title='Open data file', initialdir='',)
elif args.datafilePath is not None and args.last is False:
    filenameData = args.datafilePath
else:
    pickles = glob.glob('./*.pkl') # all .pkl file of *this* directory
    filenameData = max(pickles, key=os.path.getctime)

with open(filenameData, 'rb') as f:
    loadedDict = pickle.load(f)

seqCnt_data = np.array(loadedDict['seqCnt_data'])
chan_data   = np.array(loadedDict['chan_data'])
trig_data   = np.array(loadedDict['trig_data'])
col_data    = np.array(loadedDict['col_data'])
lane_data   = np.array(loadedDict['lane_data'])
row_data    = np.array(loadedDict['row_data'])
ToT_data    = np.array(loadedDict['ToT_data'])
ToA_C_data  = np.array(loadedDict['ToA_C_data'])
ToA_F_data  = np.array(loadedDict['ToA_F_data'])
timing_data = np.array(loadedDict['timing_data'])

    
Jitter = [[0 for x in range(192)] for y in range(176)]
EventN = [[0 for x in range(192)] for y in range(176)]   
TOA_mean = [[0 for x in range(192)] for y in range(176)] 
TOT_mean = [[0 for x in range(192)] for y in range(176)]
Tpeak = [[0 for x in range(192)] for y in range(176)]
for rowN in range(121,171, 1):
    print( f'Processing pixel row: {rowN}' )    
    for colN in range(142,192, 1):       
        if len(np.array(ToA_F_data)):
            index_row = np.where(np.array(row_data) == rowN)
            col_pix = np.array(col_data)[index_row[0]]
            lane_pix = np.array(lane_data)[index_row[0]]
            row_pix = np.array(row_data)[index_row[0]]
            ToT_pix = np.array(ToT_data)[index_row[0]]
            ToA_C_pix = np.array(ToA_C_data)[index_row[0]]
            ToA_F_pix = np.array(ToA_F_data)[index_row[0]]
            if len(np.array(ToA_F_pix)):
                index_col = np.where(np.array(col_pix) == colN)
                col_pix = np.array(col_pix)[index_col[0]]
                lane_pix = np.array(lane_pix)[index_col[0]]
                row_pix = np.array(row_pix)[index_col[0]]
                ToT_pix = np.array(ToT_pix)[index_col[0]]
                ToA_C_pix = np.array(ToA_C_pix)[index_col[0]]
                ToA_F_pix = np.array(ToA_F_pix)[index_col[0]]
            if len(np.array(ToA_F_pix)):
                col = np.array(col_pix)
                lane = np.array(lane_pix)
                row = np.array(row_pix)
                ToT = np.array(ToT_pix)
                ToA_C = np.array(ToA_C_pix)
                ToA_F = np.array(ToA_F_pix)
                TOA = (ToA_C*256 + ToA_F)*LSB
                TOT = ToT   
                y, x = np.histogram(TOA, bins=1000000)
                TOA_mean[rowN][colN] = np.mean(TOA)
                TOT_mean[rowN][colN] = np.mean(TOT)
                EventN[rowN][colN] = len(np.array(ToA_F_pix))
                print(f"Total number of events for selected pixel: {EventN[rowN][colN]}")        
                Jitter[rowN][colN] = math.sqrt(math.pow(np.std(TOA, dtype=np.float64),2)+LSB*LSB/12)
                Tpeak[rowN][colN] = int(x[np.argmax(y)])

DataProc_Stop = time.perf_counter()
print(f"Data Processed in {(DataProc_Stop - DataProc_Start)/60:0.4f} minutes")

fig1, (ax7, ax8) = plt.subplots(nrows = 1, ncols = 2, figsize=(14,5))

#shw7 = ax7.imshow(Jitter, cmap='inferno', vmin=np.max([np.min(Jitter)-50, 0]), vmax=np.max(Jitter)+50)
shw7 = ax7.imshow(Jitter, cmap='inferno', vmin=np.max([np.min(Jitter)-50, 0]), vmax=300)
bar7 = plt.colorbar(shw7, ax=ax7)
bar7.set_label('Jitter [ps]')

#shw8 = ax8.imshow(EventN, cmap='inferno', vmin=np.max([np.min(EventN)-100, 0]), vmax=np.max(EventN))
shw8 = ax8.imshow(EventN, cmap='inferno', vmin=np.max([np.min(EventN)-100, 0]), vmax=np.min([np.max(EventN),700]))
bar8 = plt.colorbar(shw8, ax=ax8)
bar8.set_label('Number of Events')
print('Total Number of Hits: %d' % np.sum(EventN))

fig2, (ax9, ax10) = plt.subplots(nrows = 1, ncols = 2, figsize=(14,5))
shw9 = ax9.imshow(TOA_mean, cmap='inferno',vmin=225000, vmax=325000)
bar9 = plt.colorbar(shw9, ax=ax9)
bar9.set_label('TOA mean')

shw10 = ax10.imshow(TOT_mean, cmap='inferno')
bar10 = plt.colorbar(shw10, ax=ax10)
bar10.set_label('TOT mean')

fig2, (ax11, ax12) = plt.subplots(nrows = 1, ncols = 2, figsize=(14,5))
shw11 = ax11.imshow(Tpeak, cmap='inferno',vmin=225000, vmax=325000)
bar11 = plt.colorbar(shw11, ax=ax11)
bar11.set_label('TOA Peak')

plt.show()
##############################################################################
###################################
