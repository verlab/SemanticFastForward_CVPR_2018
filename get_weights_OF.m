%> @file get_weights_OF.m
%>
%> Function to calculate the weight of a frames based on its optical flow.
%> 
%> Functional call:
%> @code
%> [ weights ] = get_weights_OF( descriptor )
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
%> As bigger is the optical flow magnitude, smaller is the associated weight to that frame.
%> 
%> @param histogram  - F x N @c double , Optical Flow histogram of magnitude and orientation.
%>
%> @retval weights   - 1 x N @c double , Sparse Coding Weights.
%>
%> @author Felipe C. Chamone (cadar@dcc.ufmg.br)
%>
%> @date 23/08/2017 
% ========================================================================
function [ weights ] = get_weights_OF( histogram )

    framecount = size(histogram, 2);
    weights = ones(framecount, 1);
    
    for i=1:framecount
        [argvalue, argmax] = max(histogram(1:50,  i));
        weights(i) = argmax*argvalue;
    end
    
    sigma = 30; 
    gaussFilter = gausswin(sigma); 
    gaussFilterNormalize = gaussFilter / sum(gaussFilter); 
    weightsFiltered = conv(weights, gaussFilterNormalize);
    weightsFiltered = weightsFiltered((1:framecount)+(floor(sigma/2)-1));

    threshold = multithresh(weightsFiltered);
    
    for i=1:framecount
        if weights(i) < threshold
            weights(i) = 0.1;
        else
            weights(i) = 1;
        end
    end
    
end