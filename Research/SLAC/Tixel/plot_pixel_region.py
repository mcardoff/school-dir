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
import glob
import sys
top_level=f'{os.getcwd()}/'# point to the software folder
import argparse
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
    type     = float,
    required = False,
    default   = 158.6, #139.3016344,
    help     = "TDC reference clock frequency in MHz",
)

parser.add_argument(
    "--log",
    action   ="store_true",
    help     = "Log scale",
)

parser.add_argument(
    "--ignoreC",
    action   ="store_true",
    help     = "ignore coarse TOA part",
)

parser.add_argument(
    "--TWc",
    action   ="store_true",
    help     = "Apply Time-Walk Correction",
)

parser.add_argument(
    "--OffsetSub",
    action   ="store_true",
    help     = "Apply Time-Walk Correction",
)

parser.add_argument(
    "--TOTfilter",
    action   ="store_true",
    help     = "Apply ToT filter",
)

parser.add_argument(
    "--rowL",
    type     = int,
    required = False,
    choices  = range(176),
    default  = 125,
    help     = "Minimum Row Number to Analyze",
)

parser.add_argument(
    "--rowH",
    type     = int,
    required = False,
    choices  = range(176),
    default  = 133,
    help     = "Maximum Row Number to Analyze",
)

parser.add_argument(
    "--colL",
    type     = int,
    required = False,
    choices  = range(192),
    default  = 181,
    help     = "Minimum Col Number to Analyze",
)

parser.add_argument(
    "--colH",
    type     = int,
    required = False,
    choices  = range(192),
    default  = 189,
    help     = "Maximum Col Number to Analyze",
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


Offset = [[0 for x in range(192)] for y in range(176)]
for rowN in range(args.rowL,args.rowH + 1, 1):
    print( f'Processing pixel row: {rowN}' )    
    for colN in range(args.colL,args.colH + 1, 1):       
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
                TOT = ToT - ToA_C
                if args.TWc is True:
                    TOA = TOA - TWc/(TOT+1) #TOA corrected for time-walk
                y, x = np.histogram(TOA, bins=65536)
                Offset[rowN][colN] = int(x[np.argmax(y)])
                #print(Offset[rowN][colN])

Offset = np.array(Offset)
print(f"Offset dispersion pp = {np.max(Offset[np.nonzero(Offset)]) - np.min(Offset[np.nonzero(Offset)])} (ps)")

if len(np.array(ToA_F_data)):
    index_row = np.where(np.array(row_data) <= args.rowH)
    seqCnt_reg = np.array(seqCnt_data)[index_row[0]]
    col_reg = np.array(col_data)[index_row[0]]
    lane_reg = np.array(lane_data)[index_row[0]]
    row_reg = np.array(row_data)[index_row[0]]
    ToT_reg = np.array(ToT_data)[index_row[0]]
    ToA_C_reg = np.array(ToA_C_data)[index_row[0]]
    ToA_F_reg = np.array(ToA_F_data)[index_row[0]]
    chan_reg = np.array(chan_data)[index_row[0]]
    index_row = np.where(np.array(row_reg) >= args.rowL)
    seqCnt_reg = np.array(seqCnt_reg)[index_row[0]]
    col_reg = np.array(col_reg)[index_row[0]]
    lane_reg = np.array(lane_reg)[index_row[0]]
    row_reg = np.array(row_reg)[index_row[0]]
    ToT_reg = np.array(ToT_reg)[index_row[0]]
    ToA_C_reg = np.array(ToA_C_reg)[index_row[0]]
    ToA_F_reg = np.array(ToA_F_reg)[index_row[0]]
    chan_reg = np.array(chan_reg)[index_row[0]]
    index_col = np.where(np.array(col_reg) <= args.colH)
    seqCnt_reg = np.array(seqCnt_reg)[index_col[0]]
    col_reg = np.array(col_reg)[index_col[0]]
    lane_reg = np.array(lane_reg)[index_col[0]]
    row_reg = np.array(row_reg)[index_col[0]]
    ToT_reg = np.array(ToT_reg)[index_col[0]]
    ToA_C_reg = np.array(ToA_C_reg)[index_col[0]]
    ToA_F_reg = np.array(ToA_F_reg)[index_col[0]]
    chan_reg = np.array(chan_reg)[index_col[0]]
    index_col = np.where(np.array(col_reg) >= args.colL)
    seqCnt_reg = np.array(seqCnt_reg)[index_col[0]]
    col_reg = np.array(col_reg)[index_col[0]]
    lane_reg = np.array(lane_reg)[index_col[0]]
    row_reg = np.array(row_reg)[index_col[0]]
    ToT_reg = np.array(ToT_reg)[index_col[0]]
    ToA_C_reg = np.array(ToA_C_reg)[index_col[0]]
    ToA_F_reg = np.array(ToA_F_reg)[index_col[0]]
    chan_reg = np.array(chan_reg)[index_col[0]]

    if len(np.array(ToA_F_reg)):
        print(f"Total number of events for selected region: {len(np.array(ToA_F_reg))}")
        seqCnt = np.array(seqCnt_reg)
        col = np.array(col_reg)
        lane = np.array(lane_reg)
        row = np.array(row_reg)
        ToT = np.array(ToT_reg)
        ToA_C = np.array(ToA_C_reg)
        ToA_F = np.array(ToA_F_reg)

        if args.ignoreC is False:
            if args.OffsetSub is False:
                TOA = (ToA_C*256 + ToA_F)*LSB
            else:
                print(np.array(Offset)[row,col])
                TOA = (ToA_C*256 + ToA_F)*LSB - np.array(Offset)[row,col]
        else:
            TOA = (ToA_C*256 + ToA_F)*LSB
            TOA = TOA % (256*4*LSB)


        TOT = ToT - ToA_C
        TOT = np.where(TOT<256,TOT, ToT - ToA_C + 256)


        if args.TWc is True:
            TOA = TOA - TWc/(TOT+1) #TOA corrected for time-walk

        if args.TOTfilter is True:
            index = np.where(TOT > 47)
            TOA = TOA[index[0]]
            TOT = TOT[index[0]]


DataProc_Stop = time.perf_counter()
print(f"Data Processed in {(DataProc_Stop - DataProc_Start)/60:0.4f} minutes")

fig, (ax1, ax2) = plt.subplots(nrows = 2, ncols = 1, figsize=(14,7))
fig.tight_layout(pad=3)

ax1.hist(TOA, bins = range(np.min(TOA).astype('int32')-int(100*LSB),np.max(TOA).astype('int32')+int(100*LSB),int(LSB)*binW), align = 'left', edgecolor = 'k', color = 'royalblue')
if args.log is True:
    ax1.set_yscale('log')
ax1.set_xlim(xmin = np.min(TOA)-4*LSB, xmax = np.max(TOA)+4*LSB)
ax1.set_xlabel('Time [ps]' , fontsize = 10)
ax1.set_ylabel('Number of events', fontsize = 10)
ax1.set_title('TOA Histogram', fontsize = 11)
y, x = np.histogram(TOA, bins=1000000)
print(f"Peak = {int(x[np.argmax(y)])} (TOA counts; ps)")
print(f"Peak = {int(x[np.argmax(y)]/1000.0)} (ns)")

ax2.hist(TOT, bins = range(np.min(TOT).astype('int32')-int(20),np.max(TOT).astype('int32')+int(20),1), align = 'left', edgecolor = 'k', color = 'royalblue')
if args.log is True:
    ax2.set_yscale('log')
ax2.set_xlim(xmin = np.min(TOT)-4, xmax = np.max(TOT)+4)
ax2.set_xlabel('TOT Code [a.u.]' , fontsize = 10)
ax2.set_ylabel('Number of events', fontsize = 10)
ax2.set_title('TOT Histogram', fontsize = 11)

Second_plot = 0
if Second_plot is True:
    fig, (ax3, ax4) = plt.subplots(nrows = 2, ncols = 1, figsize=(14,7))
    fig.tight_layout(pad=3)

    y, x = np.histogram(TOA, bins = range(np.min(TOA).astype('int32')-int(100*LSB),np.max(TOA).astype('int32')+int(100*LSB),int(LSB)*binW))
    ax3.plot(x[0:len(y)], np.cumsum(y))
    ax3.set_xlim(xmin = np.min(TOA)-4*LSB, xmax = np.max(TOA)+4*LSB)
    ax3.set_xlabel('Time [ps]' , fontsize = 10)
    ax3.set_ylabel('Sum of the number of events', fontsize = 10)
    ax3.set_title('TOA Histogram Integral', fontsize = 11)

plt.tight_layout()
plt.show()
##############################################################################
###################################
