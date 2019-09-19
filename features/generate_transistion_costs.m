%% ATTENTION: This code generates only the Appearance and the Semantic Costs from the MIFF paper.
% Please go to the MIFF code (_SemanticFastForward_JVCI_2018) if you need
% to generate all the costs.
function generate_transistion_costs(inputVideo, speedupRate, semantic_extractor)
addpath(genpath('_SemanticFastForward_JVCI_2018'));

[video_dir, fname, ~] = fileparts(inputVideo);

if strcmp(video_dir, '') ~= 0
    video_dir = '.';
    inputVideo = ['./' inputVideo];
end

fname_costs = [video_dir '/' fname '_Costs_' num2str(speedupRate) 'x.mat'];
fname_csv_cost = [video_dir '/' fname '_SemanticCosts.csv'];

if (exist(fname_costs, 'file') == 2 )
    fprintf('%sThe Transition Costs file already exists.\n', log_line_prefix);
elseif (exist(fname_csv_cost, 'file') == 2 )
    fprintf('%sThe Semantic Costs CSV file already exists.\n', log_line_prefix);
else
    fprintf('%sReading input video...\n', log_line_prefix);
    reader = VideoReader(inputVideo);
    num_frames = reader.NumberOfFrames;
    
    %% Preparing preprocessing data
    fpstereo = [video_dir '/' fname '_fpstereo.mat'];
    if ~exist(fpstereo, 'file')
        fprintf('%sPreprocess file not found...\n', log_line_prefix);
        optical_flow_csv = [video_dir '/' fname '.csv'];
        if exist(optical_flow_csv, 'file') == 0
            fprintf('%sOptical Flow CSV file not found...\n', log_line_prefix);
            error('Please run Vid2OpticalFlowCSV first!');
        end
        fprintf('%sPreprocessing optical flow data...\n', log_line_prefix);
        Util.PrepreocessSequences(inputVideo, '');
        fprintf('%sDone preprocessing optical flow data...\n', log_line_prefix);
    end

    % Semantic Costs
    semantic_filename = [video_dir '/' fname '_' semantic_extractor '_extracted.mat'];

    fprintf('%sAttaching the semantic extraction information...\n', log_line_prefix);
    if exist(semantic_filename, 'file')
        load(semantic_filename);
        SemanticData = Rects;
        clear Rects;
        clear total_values;
    else
        error('Please, run ExtractAndSave with the extractor ''%s'' to extract the semantic information first!', semantic_extractor);
    end

    sd = Util.LoadVidDataFromMat(inputVideo,'fpstereo','returnonly');
    cfg = ConfigWrapper({'inputVideoFileName', inputVideo;
        'startInd', 1;
        'endInd', num_frames-3;
        'FOE_Reference','Absolute';
        'maxTemporalDist', 100;
        'VelocityTarget','CumulativeMean';
        'AppearanceCostFunction','ColorHistogram';
        'SemanticFunction', 'NormScore';
        'SemanticCostFunction','Semantic';
        'SemanticTermWeight',1;
        'SemanticEpsilon', 1;});

    %% Calculate batches of cost functions
    startInd = cfg.get('startInd');
    endInd = cfg.get('endInd');
    tempDist = 100; %Tau number (max temporal skip in the Silva et. al's graph strategy).

    lastSparseInd =1;
    batchSize = min(cfg.get('maxBatchMemory')/tempDist, endInd-startInd+1);
    numberOfBatches = ceil((endInd-startInd)/batchSize);

    %% Loading classes
    OFEstimator = CumulativeLKEstimator(sd, cfg);
    appearanceCostEstimator = ColorHistogramCost(sd, cfg);
    semanticCostEstimator = SemanticCost(sd, cfg, SemanticData);

    %% Running code
    Appearance_cost     = zeros(endInd-startInd,tempDist);
    Semantic_cost       = zeros(endInd-startInd,tempDist);

    for batch = 1:numberOfBatches

        startBatch = startInd + batchSize*(batch-1);
        endBatch = min(startBatch + batchSize-1, endInd);
        fprintf('\n%sStarting batch No. %d from frame %d to frame %d\n',log_line_prefix, batch, startBatch, endBatch);

        fprintf('%sEstimating optical flow between pairs of frames...\n',log_line_prefix);
        [OF_X,OF_Y,OF_ind_source_batch,OF_ind_dest_batch] = OFEstimator.estimateBatch(startBatch, endBatch);

        fprintf('%sCalculating appearance cost...\n',log_line_prefix);
        [Appearance_cost_batch] = appearanceCostEstimator.calculateCostBatch(OF_X, OF_Y, startBatch, endBatch);

        fprintf('%sCalculating semantic cost...\n',log_line_prefix);
        [Semantic_cost_batch] = semanticCostEstimator.calculateCostBatch (OF_X, OF_Y, startBatch, endBatch);

        numSparseInd = numel(OF_ind_source_batch);
        obj.OF_ind_source(lastSparseInd : lastSparseInd + numSparseInd-1) = OF_ind_source_batch + startBatch-1;
        obj.OF_ind_dest   (lastSparseInd : lastSparseInd + numSparseInd-1) = OF_ind_dest_batch + startBatch-1;
        lastSparseInd = lastSparseInd + numSparseInd;

        Appearance_cost(startBatch-startInd+1 : endBatch-startInd+1, :) = Appearance_cost_batch;               
        Semantic_cost(startBatch-startInd+1 : endBatch-startInd+1, :) = Semantic_cost_batch;

    end
    fprintf('%sSaving Transition Costs to "%s"...\n',log_line_prefix, fname_costs);

    %This assignment is necessary for further processing: Do NOT remove!
    appearance_cost = Appearance_cost;
    semantic_cost = Semantic_cost;

    save(fname_costs, '-v7.3','appearance_cost', 'semantic_cost');
    
    fprintf('%sSaving Semantic Costs to "%s"...\n',log_line_prefix, fname_csv_cost);
    dlmwrite(fname_csv_cost,startInd);
    dlmwrite(fname_csv_cost,semantic_cost,'-append');

    fprintf('%sCosts file saved to "%s"...\n',log_line_prefix, fname_costs);
end
end
