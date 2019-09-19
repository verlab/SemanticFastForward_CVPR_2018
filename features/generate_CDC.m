%> @file generate_CDC.m
%>
%> Function to generate the Cumulative Displacements Curves of the Optical Flow.
%> 
%> Functional call:
%> @code
%> [ dx, ddx ] = generate_CDC( OF )
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
%> @brief Generate the Cumulative Displacements Curves of the Optical Flow <a href="http://ieeexplore.ieee.org/document/6909721/">(Poleg et al 2014)</a> [1].
%> 
%> [1] Y. Poleg, C. Arora and S. Peleg, **Temporal Segmentation of Egocentric Videos**, 2014 IEEE Conference on Computer Vision and Pattern Recognition, Columbus, OH, 2014, pp. 2537-2544. doi: 10.1109/CVPR.2014.325
%> 
%> @param  OF   - N x 50 @c double , Matrix of Optical Flow (25 fx and 25fy) of the 5x5 windowed N frames.
%>
%> @retval dx   - 25 x N   @c double , Cumulative Displacement curves related to the X axis.
%> @retval ddx  - 25 x N-1 @c double , Derivate of Cumulative Displacement curves related to the X axis.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @todo This function is hardcoded to run in a 5x5 windowed image. Make this code generic for any windowed image setup.
%>
%> @date 20/10/2017 
% ========================================================================
function [ dx, ddx ] = generate_CDC( OF )
        
    %OpticalFlow
    OFx = OF(:, 1:25);
    
    %Displacements
    dx = cumsum(OFx);
    
    %Derivates
    ddx = diff(dx,1,1);

end

