%> @file accelerate_video_LLC.m
%>
%> Function to start the spams library, prepare the variables and invoke the adaptative_frame_selection_LLC.m
%> 
%> Functional call:
%> @code
%> [ selectedFrames ] = accelerated_video_LLC( experiment , semanticExtractor, varargin )
%> @endcode
%>
%> Possible values to varargin argument:
%> - \b Speedup             - @c integer      , speed-up rate desired to accelerate the video.
%> - \b SpeedupFactor       - @c integer      , speed-up rate factor used to multiply the desired accelerate the video while calling the speed-up video function.
%> - \b ShowFigures         - @c boolean      , flag to show imagens during the Sparse Coding processing.
%> - \b Verbose             - @c boolean      , flag to display processing debug messages.
%> - \b GenerateVideo       - @c boolean      , flag to save the accelerated video in disk.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%   This file is part of SparseSampling@CVPR2018.
%
%    SparseSampling@CVPR2018 is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    SparseSampling@CVPR2018 is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with SparseSampling@CVPR2018.  If not, see <http://www.gnu.org/licenses/>.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% ========================================================================
%> @brief Function to exectute the following steps to call the Adaptive frame selection via Sparse Coding:
%> - start spams libraty
%> - load video features
%> - calculate speed-up rates and ranges
%>
%> after the frame sampling, based on showFigures value, it saves the video.
%>
%> @param inputVideo        - @c string       , complete path to input video.
%> @param semanticExtractor - @c string       , extractor type [ 'pedestrian' | 'face' ]
%> @param varargin          - 1 X A @c string , list of A optional arguments. 
%> - \b Speedup             - @c integer      , speed-up rate desired to accelerate the video.
%> - \b SpeedupFactor       - @c integer      , speed-up rate factor used to multiply the desired accelerate the video while calling the speed-up video function.
%> - \b ShowFigures         - @c boolean      , flag to show imagens during the Sparse Coding processing.
%> - \b Verbose             - @c boolean      , flag to display processing debug messages.
%> - \b GenerateVideo       - @c boolean      , flag to save the accelerated video in disk.
%>
%> @retval selectedFrames   - 1 x N @c int    , vector containing the N selected frames  
%>
%> @warning If any of the paths set read or write do not exist, the **program will be killed**.
%>
%> @authors Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 19/09/2019 
%>
%> @see adaptative_frame_selection_LLC.m
% ========================================================================
function [ selectedFrames ] = accelerate_video_LLC( inputVideo , semanticExtractor, varargin )

    addpath('io','etc','features')

    %% Input parse.
    p = inputParser;

    validSemanticExtractor  = {'face', 'pedestrian'};
    checkSemanticExtractor  = @(x) any(validatestring(x,validSemanticExtractor));
    
    addRequired( p , 'semanticExtractor'  , checkSemanticExtractor);

    addOptional( p , 'Speedup'            , 10    , @isnumeric );   
    addOptional( p , 'SpeedupFactor'      , 2     , @isnumeric );    

    addOptional( p , 'ShowFigures'        , false , @islogical );
    addOptional( p , 'Verbose'            , true  , @islogical );
    addOptional( p , 'GenerateVideo'      , false , @islogical );
    
    parse(p, semanticExtractor, varargin{:});
        
    %%
    [videoFolder, fname, ~] = fileparts(inputVideo);
    prefix = [videoFolder '/' fname];
    
    %% Experiment ID and computer name.k
    [ id.computer , id.experiment ] = get_experiment_info();
    message_handle('',sprintf(' Computer ID: %s | Experiment %s : %s', id.computer, num2str(id.experiment), fname),'ExtraSpace',true,'TopLine',true,'BottomLine',true);
    
    %% Path Settings.
    
    % In.
    path.in.MIFFcode          = '_SemanticFastForward_JVCI_2018/';
    path.in.semanticExtracted  = [prefix '_' semanticExtractor '_extracted.mat'];
    path.in.video              = inputVideo;
    path.in.opticalFlowCSV     = [prefix '.csv'];
    path.in.YoloDesc           = [prefix '_yolo_desc.csv' ];
    
    %Out
    path.out.outputDir          = videoFolder;
    path.out.videoFeatures      = [prefix '_video_features_whitening.mat'];
    path.out.costsMatrix        = [prefix '_Costs_' num2str(p.Results.Speedup) 'x.mat'];
    path.out.generalResults     = [ path.out.outputDir '/' 'General_Results.csv' ];
    path.out.selectedFrames     = [ path.out.outputDir '/' fname '_LLC_EXP_' id.computer '_' num2str(id.experiment, '%04d') '_selected_frames.csv'];
    path.out.outputVideo        = [ path.out.outputDir '/' fname '_LLC_EXP_' id.computer '_' num2str(id.experiment, '%04d') ];
    
    % Checking
    if ~check_paths(path);
        message_handle('E', 'Exiting due to an error during the path checking.');
        return;
    end

    %% Load libraries.
    addpath( path.in.MIFFcode );
    addpath( [ path.in.MIFFcode 'SemanticScripts' ] );
    addpath( [ path.in.MIFFcode 'PSOScripts' ] );
    addpath( [ path.in.MIFFcode 'Util' ] );

    %% Video features.
    if exist(path.out.videoFeatures, 'file') == 2
        myFile = load(path.out.videoFeatures);
        videoFeaturesFile = myFile.videoFeaturesFile;
    else
        [ videoFeaturesFile.descriptors, videoFeaturesFile.OF ] = video_features_frames(path.in.video, path.in.opticalFlowCSV, path.in.semanticExtracted, path.in.YoloDesc);
        save_video_features(videoFeaturesFile, path.out.videoFeatures);
    end

    videoFeatures = videoFeaturesFile.descriptors;
    OF = videoFeaturesFile.OF;
    nFrames = size(videoFeatures,2);
    
    %% Calculating semantic and non-semantic speed-up using PSO.
    % ***JVCI Code***.
    [speedupRates, ~, rangesAndSpeedups, ~, ~, ~] = CalculateSpeedupRates(path.in.semanticExtracted, p.Results.Speedup, 'MultipleThresholds', true);
    ranges = AddNonSemanticRanges(rangesAndSpeedups, speedupRates, 1, nFrames, 150);
    % ***JVCI Code***.
    
    %% Costs
    generate_transistion_costs(inputVideo, p.Results.Speedup, semanticExtractor)
    
    %% Descriptor.
    distanceOneHot = time_features(nFrames, 100 );
    videoFeatures = whitening_matrix( [videoFeatures;distanceOneHot]' )';                
    
    %% Calculate weight.
    weights = get_weights_CDC( OF' );
    
    %% Frame Selection.
     selectedFrames = adaptative_frame_selection_LLC(videoFeatures, ranges, path.out.costsMatrix , 'ShowFigures', p.Results.ShowFigures, ...
        'Verbose', p.Results.Verbose, 'Weights', weights, 'SpeedupFactor',p.Results.SpeedupFactor);
    
    message_handle('S', sprintf('Final Video Speed-up: %f',nFrames/size(selectedFrames,2)),'ExtraSpace',true);
    
    %% Output view.
    if p.Results.ShowFigures
        pause; 
        close all;
    end
        
    write_selected_frames(path.out.selectedFrames, ranges, selectedFrames);
    write_general_results(path.out.generalResults, id, fname, 'LLC', semanticExtractor, p);

    %% Save final video.
    if p.Results.GenerateVideo
        save_accelerated_video( path.in.video , path.out.outputVideo , selectedFrames );
    end
    
    rmpath('io','etc','features', path.in.MIFFcode, [ path.in.MIFFcode 'SemanticScripts' ], [ path.in.MIFFcode 'PSOScripts' ], [ path.in.MIFFcode 'Util' ] );
    
end
