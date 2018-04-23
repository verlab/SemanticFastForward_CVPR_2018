%> @file get_weights_distance.m
%>
%> Function to calculate the weight of a frames based on its distance.
%> 
%> Functional call:
%> @code
%> [ weights ] = get_weights_distance( numFrames , speedup )
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
%> @param  numFrames - @c int , Number of frames in the video.
%> @param  speedup   - @c int , Desired speed-up.
%>
%> @retval weights   - 1 x N @c double , Sparse Coding Weights based on the distance from the desired jumps.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 18/09/2017 
% ========================================================================
function [ weights ] = get_weights_distance( numFrames , speedup )

    weights = 1:speedup;
    weights = (1 - ( abs( mod(weights,speedup) - (speedup/2) ) / 100 ));
    weights = repmat(weights, 1, ceil(numFrames/speedup));
    weights = weights(1:numFrames);    

end