### 1. Before we start let's make sure the MIFF code is up to date and save some bash variables to make the paths shorter.

#### 1.1. Go to the project folder and run:
```bash
git submodule update --init --recursive

git submodule update --recursive --remote

SPARSE_SAMPLING=$PWD
```

#### 1.2. Now go to the folder you cloned and compiled Darknet repository and run:
```bash
DARKNET=$PWD
```

### 2. Download our example video (in Linux bash) and move it to a new folder "Example" in the project folder
```bash
cd $SPARSE_SAMPLING
mkdir Example
cd Example

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example.mp4
```

### 3. Extract the optical flow information. 

In the _Vid2OpticalFlowCSV folder, compile the code using the command
```
make
```

Then calculate the optical flow of the input video:

Usage: `optflow -v <video_file> -c <config_file> -o <output_file>`
```
optflow -v example.mp4 -c default-config.xml -o example.csv
```

The output file name must be the same name of the input video using the extesion ".csv". Save the output file in the same folder of the `example.mp4`.


### 4. Extract semantic information from video with "_SemanticFastForward_JVCI_2018/SemanticScripts/ExtractAndSave.m". Output file will be placed on the input video folder, with video file name, followed by the semantic extractor and the suffix "extracted.mat". Example: "example_face_extracted.mat".

On MATLAB console, go to the project folder and run the following commands:

Usage: `ExtractAndSave(< video_file_path >, < semantic_extractor >);`
```matlab
cd $SPARSE_SAMPLING
matlab -nodisplay

addpath('SemanticScripts');
ExtractAndSave('Example/example.mp4', 'face');
exit;
```

### Results for steps 3 (example.csv) and 4 (example_face_extracted.mat) for this example video are available for download. Run:

```bash
cd $SPARSE_SAMPLING/Example

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example.csv

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example_face_extracted.mat
```

### 5. Extract the Yolo Features.

On the terminal (Linux bash) go to the folder you cloned from Darknet repository and run the following commands:
% (if you cloned the Darknet repository in the project folder it should look like this)

```bash
cd $DARKNET

./darknet detector demo cfg/coco.data cfg/yolo.cfg yolo.weights $SPARSE_SAMPLING/Example/example.mp4 $SPARSE_SAMPLING/Example/example_yolo_raw.txt

cd $SPARSE_SAMPLING

python generate_yolo_descriptor.py Example/example.mp4 Example/example_yolo_raw.txt Example/example_yolo_desc.csv
```

### Results for step 4 (example_yolo_raw.txt and example_yolo_desc.csv) for this example video are available for download. Run:

```bash
cd $SPARSE_SAMPLING/Example

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example_yolo_raw.txt

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example_yolo_desc.csv
```

### 6. To generate the final hyperlapse video, use the "accelerate_video_LLC" function.

On MATLAB console, go to the project folder and run the following commands:

Usage: `accelerate_video_LLC( < video_path > , < semantic_extractor >, [Descriptor] [WeightMode] [CostsMode] [Speedup] [SpeedupFactor] [LoadVideoFeatures] [MultiImportance] [ShowFigures] [Verbose] [GenerateVideo] );`

```matlab
addpath('LLC');
accelerate_video_LLC( '<video_folder>/example.mp4' , 'face', 'GenerateVideo', false);
% The user may set the optional argument 'GenerateVideo' as false to avoid generate the output video during the search.
```

### Full script to generate the hyperlapse without generating all of the required files:

Bash:
```bash
## Go to project folder "2018-cvpr-silva-sparsesampling-code" ##

mkdir Example

cd Example

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example.mp4

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example.csv

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example_face_extracted.mat

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example_yolo_raw.txt

wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example_yolo_desc.csv
```

Matlab:
```matlab
%% Go to project folder 
addpath(LLC)
accelerate_video_LLC( 'Example/example.mp4' , 'face', 'GenerateVideo', false);
```

