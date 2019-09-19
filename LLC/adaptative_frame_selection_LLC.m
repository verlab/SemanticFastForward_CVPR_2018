%> @file adaptative_frame_selection_LLC.m
%>
%> Function to adaptatively select the frames using Locality-constrained Linear Coding methodology.
%> 
%> Functional call:
%> @code
%> [ selectedFrames , errors ] = adaptative_frame_selection_LLC( videoFeatures, ranges, costsMatrixFilename, varargin)
%> @endcode
%>
%> Possible values to varargin argument:
%> - \b Weights           -  1 x N @c double , vector with the weights to each of the N frames related to their Optical Flow Magnitude.
%> - \b ShowFigures       - @c boolean       , flag to show imagens during the Sparse Coding processing.
%> - \b Verbose           - @c boolean       , flag to display processing debug messages.
%> - \b SpeedupFactor       - @c integer      , speed-up rate factor used to multiply the desired accelerate the video while calling the speed-up video function.

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
%> @brief Adaptive frame selection via Locality-constrained Linear Coding.
%> 
%> @param videoFeatures         - VF x W @c double , features of each frame of the video in a matrix format of dimensions VF (number of features) by W (num. frames).
%> @param ranges                -  4 x N @c int    , matrix of dimension 4 and N ranges of semantic segments.
%> @param costsMatrixFilename   -  1 X A @c string , complete path and filename with extension to the transitions CostsMatrix MATLAB file.
%> @param varargin              -  1 X A @c string , list of A optional arguments:
%> - \b Weights                 -  1 x N @c double , vector with the weights to each of the N frames related to their Optical Flow Magnitude.
%> - \b ShowFigures             - @c boolean       , flag to show imagens during the Sparse Coding processing.
%> - \b Verbose                 - @c boolean       , flag to display processing debug messages.
%> - \b SpeedupFactor           - @c integer      , speed-up rate factor used to multiply the desired accelerate the video while calling the speed-up video function.
%>
%> @retval selectedFrames       - 1 x M @c int     , vector cointaning the M selected frames.
%> @retval erros                - 1 x N @c int     , vector cointaning the errors associate to each segment. 
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 19/09/2019
% ========================================================================
function [ selectedFrames , errors ] = adaptative_frame_selection_LLC( videoFeatures, ranges, costsMatrixFilename, varargin)

    p = inputParser;

    addOptional( p , 'Weights'              , []);
    addOptional( p , 'ShowFigures'          , true  , @islogical);
    addOptional( p , 'Verbose'              , true  , @islogical);
    addOptional( p , 'SpeedupFactor'        , 2     , @isnumeric ); 
    
    parse(p, varargin{:});

    if (size(videoFeatures,1) > size(videoFeatures,2))
        message_handle('E', 'Number of samples smaller than the number of dimensions.');
        return;
    end
    
    if isempty(ranges)        
        message_handle('E', 'The ranges vector is empty.');
        return;
    else

        selectedFrames = [];
        errors = zeros(1,size(ranges,2));
                
        %Load Cost Matrix
        costs = load(costsMatrixFilename);

        
        %% process each segment.
        for i=1:size(ranges,2)

            %% Setting values
            initial = ranges(1,i);
            final = ranges(2,i);
            speedup = ranges(3,i) * p.Results.SpeedupFactor;
            knn = min((final - initial) , round( (final-initial)/speedup ) * 2) ;
            desired = round((final-initial)/(speedup));
            
            %% Dictionary (D) and Representation (X) 
            D = videoFeatures(:, initial:final);
            X = sum( D , 2 );               
            
            %% Data normalization
            D=D./repmat(sqrt(sum(D.^2)),[size(D,1) 1]);
            X=X./repmat(sqrt(sum(X.^2)),[size(X,1) 1]);
                        
            if isempty(p.Results.Weights)            
                W = [];
            else
                W = p.Results.Weights(initial:final, :);
            end
            
            [ lambda , coeff ] = automatic_lambda_adjustment(desired, X, D, W, knn, i, p.Results.Verbose);
            [ totalError, reconstructionError , localityTerm ] = calculate_errors_LLC (D, X, coeff, lambda, W);
            selectedFramesActual = find(coeff>0) + initial - 1;

            if p.Results.Verbose
                message_handle( 'L' , sprintf('Total error: %d', totalError));
                message_handle( 'L' , sprintf('Reconstruct term: %d', reconstructionError));
                message_handle( 'L' , sprintf('Locality term: %d', localityTerm));
                message_handle( 'L' , sprintf('lambda: %d', lambda));              
                message_handle( 'I' , sprintf('Segment %2.d | %4.d -> %4.d | Speed-up: %2.d | KNN: %4.d | Selected Frames: %4.d | Reconstruction error: %f | Locality Term: %f', ...
                    i, initial, final, speedup, knn, size(selectedFramesActual,2), reconstructionError, localityTerm) );
            end
            
            if p.Results.ShowFigures
                figure(); plot(selectedFramesActual);
                figureTitle = ['Segment #' num2str(i)];
                figureError = ['Reconstruction Error: ' num2str(reconstructionError)];
                figureLegend = ['# Selected Frames: ' num2str(length(selectedFramesActual))];
                xlabel(figureError);
                title(figureTitle);
                legend(figureLegend);
            end
            
            desired = round((final-initial)/ranges(3,i));
            selectedFramesActual = add_frames_by_cost_matrix(selectedFramesActual, ranges(:,i), costs, desired, p.Results.ShowFigures);
            
            selectedFrames = [ selectedFrames selectedFramesActual];
            errors(i) = reconstructionError;

        end
    
    end
    
    if p.Results.ShowFigures
        figure;
        plot(selectedFrames);
        title('Final Video')
        figureError = ['Reconstruction Error: ' num2str(sum(errors))];
        xlabel(figureError);
        figureLegend = ['# Selected Frames: ' num2str(length(selectedFrames))];
        legend(figureLegend);
    end
    
end

% ========================================================================
%> @brief Automatic lambda adjustment for the LLC algorithm.
%> 
%> @param desired           - @c int            , desired number of frames.
%> @param X                 - M x N @c double   , video representarion.
%> @param D                 - M x P @c double   , dictionary.
%> @param W                 - P x N @c double   , LLC weights.
%> @param knn               - @c int            , number of centroids used into LLC.
%> @param segmentID         - @c int            , segment number.
%> @param verbose           - @c boolean        , flag to print messages.
%>
%> @retval lambda           - @c double         , best lambda value found.
%> @retval coeff            - H x 1 @c int      , vector of coefficients when solving the LLC problem using the best lambda value.
%>  
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 20/09/2017 
% ========================================================================
function [ lambda, coeff ] = automatic_lambda_adjustment(desired, X, D, W, knn, segmentID, verbose)
        
    if verbose
        message_handle('','Lambda Adjustment...','ExtraSpace',true);
    end
    
    smallerFrameError = Inf;
    
    for i = 1 : 10

        lambdaIter = 10^(-i);
        
        if isempty(W)
            coeffIter = LLC_coding_appr(D', X', knn, lambdaIter);
        else
            coeffIter = LLC_weighted_coding_appr(D', X', W, knn, lambdaIter);
        end

        selectedFrames = length(find(coeffIter>0));        
        frameError = abs(desired - selectedFrames);
        
        if frameError < smallerFrameError
            lambda = lambdaIter;
            smallerFrameError = frameError;
            coeff = coeffIter;
        end

        if verbose
            message_handle('L',sprintf('Segment %2.1d | Frame error: %4.1d | lambda: %1.10f', segmentID, frameError, lambdaIter));
        end

    end

end


