%> @file get_weights_by_histogram.m
%>
%> Function to calculate weights based on the histogram of magnitudes over the Optical Flow estimations.
%> 
%> Functional call:
%> @code
%> [ weights ] = get_weights_by_histogram( histogram )
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
%> @brief Calculate weights based on the histogram of magnitudes over the Optical Flow estimations.
%> 
%> @param histogram  - F x N @c double , Optical Flow
%>
%> @retval weights   - 1 x N @c double , Sparse Coding Weights
%>
%> @author Felipe C. Chamone (cadar@dcc.ufmg.br) and Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 20/09/2017 
% ========================================================================
function [ weights ] = get_weights_by_histogram( histogram )
        
    framecount = size(histogram, 2);
    weights = ones(framecount, 1);
    signal = zeros(framecount, 1);
   
    for j=1:framecount
        [argvalue, argmax] = max(histogram(1:50,  j));
            signal(j) = argmax*argvalue;
    end
    
    sigma = 300;
    sz = 120;    % length of gaussFilter vector
    gaussFilter = gausswin(sigma); 
    gaussFilter = gaussFilter((floor(sigma/2)-floor(sz/2)):(floor(sigma/2)+floor(sz/2)));
    gaussFilter = gaussFilter / sum (gaussFilter); % normalize
    signal = conv(signal, gaussFilter, 'same');
    
    signal = log(signal);
    signal = signal / max(signal);
    
    sigma = 2000;
    sz = 450;    % length of gaussFilter vector
    gaussFilter = gausswin(sigma); 
    gaussFilter = gaussFilter((floor(sigma/2)-floor(sz/2)):(floor(sigma/2)+floor(sz/2)));
    gaussFilter = gaussFilter / sum (gaussFilter); % normalize
    thresholds = conv(signal, gaussFilter, 'same');
    
    weights( signal > thresholds ) =  0.1;    
        
end

