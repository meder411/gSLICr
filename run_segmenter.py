import os.path as osp
import os
import argparse

PROJECT_BIN_DIR = 'bin'

######################
# PARSE ARGUMENTS
######################
parser = argparse.ArgumentParser()
parser.add_argument('-i', '--input', required=True, help='Input image directory')
parser.add_argument('-o', '--output', required=True, help='Output image directory')
parser.add_argument('-e', '--ext', required=True, help='Extension of image files to segment (WITH leading period)')

rec_group = parser.add_mutually_exclusive_group()
rec_group.add_argument('-ls', '--large-scale', action='store_true', help='Fully recursive traveral of file tree (faster for large-scale datasets')
rec_group.add_argument('-r', '--recursive', action='store_true', help='Simple recursive traversal of file tree (for small datasets)')

seg_group = parser.add_mutually_exclusive_group(required=True)
seg_group.add_argument('--num', help='Specify number of segments to determine the segmentations (DEFAULT)', nargs='?', const=128, type=int)
seg_group.add_argument('--size', help='Specify size of segments in pixels to determine the segmentations', nargs='?', const=256, type=int)

space_group = parser.add_mutually_exclusive_group(required=True)
space_group.add_argument('--XYZ', action='store_true', help='Use XYZ space for clustering')
space_group.add_argument('--RGB', action='store_true', help='Use RGB space for clustering')
space_group.add_argument('--LAB', action='store_true', help='Use CIELAB space for clustering')

scale_group = parser.add_mutually_exclusive_group(required=False)
scale_group.add_argument('-s', '--scale', help='Scale to resize images')
scale_group.add_argument('--sidelen', help='Maximum side-length to resize \
	images to')

parser.add_argument('--itt', help='Number of iterations of SLIC to run')
parser.add_argument('--coh', help='Coherence weight (float in range [0,1])')
parser.add_argument('--no-enforce', action='store_false', help='Don\'t enforce connectivity within each superpixel')
parser.add_argument('-v', '--verbose', action='store_true', help='Verbose output')

args = parser.parse_args()

# REQUIRED ARGUMENTS
INPUT = args.input
OUTPUT_DIR = args.output
EXT = args.ext

# OPTIONAL RECURSION
SIMPLE_RECURSIVE = args.recursive
LARGE_SCALE = args.large_scale

# SEGMENTATION TYPE
SPIXEL_SIZE = args.size
NUM_SEGS = args.num

# MUTUALLY EXLUSIVE REQUIRED COLOR SPACE ARGS
XYZ = args.XYZ
RGB = args.RGB
LAB = args.LAB

# OPTIONAL PARAMETERS WITH DEFAULTS
NUM_ITER = args.itt
COH_WEIGHT = args.coh
ENFORCE = args.no_enforce # Default to True
VERBOSE = args.verbose
SCALE = args.scale # Default to 1.0
SIDELEN = args.sidelen # Default to 480

########################
# CALL EXECUTABLE
########################
# Compose command call
cmd = osp.join(PROJECT_BIN_DIR, 'slic_image_segmenter') + \
	' --input_path ' + INPUT + \
	' --output_path ' + OUTPUT_DIR + \
	' --ext ' + EXT

# Segmentation size parameters
if NUM_SEGS is not None:
	cmd += ' --num_segs ' + str(NUM_SEGS)
elif SPIXEL_SIZE is not None:
	cmd += ' --spixel_size ' + str(SPIXEL_SIZE)

# Color space parameters
if XYZ:
	cmd += ' --color_space XYZ'
elif RGB:
	cmd += ' --color_space RGB'
elif LAB:
	cmd += ' --color_space CIELAB'

# File tree recursion type
if SIMPLE_RECURSIVE:
	cmd += ' --recursive'
if LARGE_SCALE:
	cmd += ' --large_scale'

# Optional parameters
if NUM_ITER is not None:
	cmd += ' --num_iters ' + str(NUM_ITER)
if COH_WEIGHT is not None:
	cmd += ' --coh_weight ' + str(COH_WEIGHT)
if not ENFORCE:
	cmd += ' --no_enforce'
if VERBOSE:
	cmd += ' --verbose'
if SCALE is not None:
	cmd += ' --scale ' + str(SCALE)
elif SIDELEN is not None:
	cmd += ' --max_sidelen ' + str(SIDELEN)

print cmd
os.system(cmd)

