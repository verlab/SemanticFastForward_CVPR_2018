Vid2OpticalFlowCSV
------------------
This utility reads a video sequence and estimates the optical flow in a grid of WxH blocks. The results are dumped to a CSV file, which is then used by other software to analyse motion patterns in the video.


Building on Windows using Visual Studio
---------------------------------------
This package is built and tested with Visual Studio 2013.

The only external library required is OpenCV. This project is pre-configured to use OpenCV 2.4.9 with dynamic linking.

Take care to update the Include and Library directories to point to your local OpenCV directories. Make sure the linker is pointing to the correct version of the OpenCV's .lib files. 


Building on other environments
-----------------------------------
Good luck.


XML Configuration File
----------------------
The configuration of Vid2OpticalFlowCSV is given in an XML file. A recommended configuration is found in 'default-config.xml'. The configuration file name is passed as an argument to the Vid2OpticalFlowCSV executable.

	Configuration parameters
	------------------------
	NUM_BLOCKS_X, NUM_BLOCKS_Y - Number of blocks in X and Y axis.
	BLOCK_WIDTH_IMG_WIDTH_RATIO, BLOCK_HEIGHT_IMG_HEIGHT_RATIO - Block size expressed as ratio. Meaning, BlockWidth=1/BLOCK_WIDTH_IMG_WIDTH_RATIO and BlockHeight=1/BLOCK_HEIGHT_IMG_WIDTH_RATIO. 
																 If BLOCK_WIDTH_IMG_WIDTH_RATIO > NUM_BLOCKS_X, then the blocks will overlap. 
																 If BLOCK_WIDTH_IMG_WIDTH_RATIO == NUM_BLOCKS_X, then the blocks will not overlap (recommended).
																 If BLOCK_WIDTH_IMG_WIDTH_RATIO < NUM_BLOCKS_X, not all image areas will be covered.
																 
	START_FRAME - Index of the first frame processed.

	END_FRAME - Index of the last frame processed.

	FRAME_SKIP - Number of frames to skip between processed frames (temporal downsampling).

	LK_FRAME_RESCALE - Downsample factor. Applied before estimating the optical flow using LK (improves run-time). Value of 0.5 will half the image size. 

	OUT_CANNY_COUNT - Estimate canny edges pixel count. 

	OUT_GFTT_COUNT - Estimate GFTT feature count.

	OUT_BACK_WARP_ERROR - Estimate re-projection error.

	DISPLAY_LK_OUTPUT - Show video playback while estimating optical flow.


Output CSV File Format
----------------------
CSV files produced by Vid2OpticalFlowCSV contain frame-to-frame block-level optical flow information, as well as meta-data on the video sequence.

The first row of the CSV file contains the meta-data and the parameters supplied to Vid2OpticalFlowCSV. The rest of the file contain the optical 
flow estimations and other parameters, one row per two consecutive frames. 

	Meta-data Fields
	---------------
	FILE_NAME - Name of the video file processed.
	START_FRAME - Index of the first frame processed.
	END_FRAME - Index of the last frame processed.
	IMG_WIDTH, IMG_HEIGHT - Dimensions of the original video
	PROCESSING_WIDTH, PROCESSING_HEIGHT - Dimensions of the processed video, after downsampling (see 'LK_FRAME_RESCALE' field in the configuration XML file).
	NUM_BLOCKS_X, NUM_BLOCKS_Y - Number of blocks in X and Y axis (see 'NUM_BLOCKS_X' and 'NUM_BLOCKS_Y' fields in the configuration XML file).
	BLOCK_WIDTH, BLOCK_HEIGHT - Size of each block. Note that BLOCK_WIDTH might be different than IMG_WIDTH/NUM_BLOCK_X in case that the value of the configuration field 'BLOCK_WIDTH_IMG_WIDTH_RATIO' is different than NUM_BLOCK_X. Same goes for the vertical axis.
	FRAME_SKIP - Number of frames skipped between processed frames. A value of 0 means that all frames in the video are processed.
	FPS - Frame rate of the original video.


	Data Rows
	---------
	Each row contains the optical flow estimation as well as other technical details for all the blocks (total of N=NUM_BLOCKS_X*NUM_BLOCKS_Y).
	The data for each block is given as a 6-tuple. The data for all the blocks is concatenated, starting from the top-left block and then block-by-block row-wise. In all, each row contains 6*N columns.
	Each tuple contains the following fields: ValidBit, OpticalFlowX, OpticalFlowY, NumCannyEdges, NumFeaturePoints, ReProjErr

	Fields Details
	--------------
	ValidBit - Set to 1 if LK converged and produced a valid result. Set to 0 if LK did not converge. If 0, the next two fields are invalid.
	OpticalFlowX, OpticalFlowY - The estimated optical flow over the entire block, estimated using LK pure translational model, in pixels (incl. sub-pixel fractions).
	NumCannyEdges - Number of edge pixels detect by canny edge detector over this block. This helps to understand the amount of texture we have in the block. Not used by our algorithm. 
	NumFeaturePoints - Number of GFTT feature points detected in this block. This helps to understand the amount of texture we have in the block. Not used by our algorithm. 
	ReProjErr - Estimated re-projection error. Helps to estimate the quality of the Optical Flow estimation.

	Parsing CSV Files in Matlab
	---------------------------
	Please see FPVUtil.m in this package.
	
	
If you are using this code, please cite the following paper:
"Y. Poleg, C. Arora, S. Peleg, Temporal Segmentation of Egocentric Videos, to appear in CVPR, 2014."	
	