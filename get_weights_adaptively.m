%> @file get_weights_adaptively.m
%>
%> Function to calculate the weight of a frames based on its optical flow.
%> 
%> Functional call:
%> @code
%> [ weights ] = get_weights_adaptively( histogram )
%> @endcode

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
%> @brief Calculate weights based on the Optical Flow.
%> 
%> @param  histogram - F x N @c double , Histogram of magnitudes and orientation of the Optical Flow.
%>
%> @retval weights   - 1 x N @c double , Sparse Coding Weights
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 14/09/2017 
% ========================================================================
function [ weights ] = get_weights_adaptively( histogram )
    
    percentage = 0.1;

    framecount = size(histogram, 2);
    weights = zeros(framecount, 1);
    
    %% Calculate weights based on the magnitude and intesity.
    for j=1:framecount
        [argvalue, argmax] = max(histogram(1:50,  j));
        weights(j) = argmax*argvalue;
    end
    
    %% Smoothing the signal.
    sigma = 300; 
    sz = 120;
    gaussFilter = gausswin(sigma); 
    gaussFilter = gaussFilter((floor(sigma/2)-floor(sz/2)):(floor(sigma/2)+floor(sz/2)));
    weightsSmooth = conv(weights, gaussFilter, 'same');
   
    %% Normalize.
    weightsSmooth = weightsSmooth ./ max(weightsSmooth);
    
    %% Calculate the adaptative threshold.
    sigma = 4000; 
    sz = 1000;
    gaussFilter = gausswin(sigma); 
    gaussFilter = gaussFilter((floor(sigma/2)-floor(sz/2)):(floor(sigma/2)+floor(sz/2)));
    gaussFilterNormalize = gaussFilter / sum(gaussFilter); 
    thresholds = conv(weightsSmooth, gaussFilterNormalize, 'same');
    
    weights = weightsSmooth - thresholds;
    weights( weights < 0 ) = 0;
    weights = 1 - weights;
    weights = weights * percentage + ( 1 - percentage );  
    
end