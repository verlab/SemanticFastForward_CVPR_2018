%> @file time_features.m
%>
%> Function to create the One Hot video frames representation based on the frame time.
%> 
%> Functional call:
%> @code
%> [ weights ] = [ oneHotDescriptor ] = time_features( numFrames, numDescriptors )( descriptor )
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
%> @brief Create the one hot descriptor based on the frame sequence position. 
%>
%> For each column, only the element in the position _ceil(i,descriptorSize)_ of the vector is activated.
%> 
%> @param numFrames         - @c int , number of frames of the video.
%> @param descriptorSize    - @c int , size of the one hot descriptor.
%>
%> @retval oneHotDescriptor - descriptorSize x numFrames @c int , One hot vector descriptor.
%>
%> Example of call:
%> 
%> @code 
%> time_features( 8, 3 )
%> @endcode
%> 
%> Output:
%>
%>@verbatim
%>ans =
%>
%>    0.3333    0.3333    0.3333         0         0         0         0         0
%>         0         0         0    0.3333    0.3333    0.3333         0         0
%>         0         0         0         0         0         0    0.3333    0.3333
%>@endverbatim
%> @author Michel M. Silva(michelms@dcc.ufmg.br)
%>
%> @date 13/09/2017 
% ========================================================================
function [ oneHotDescriptor ] = time_features( numFrames, descriptorSize )

    oneHotDescriptor = zeros (descriptorSize, numFrames);
    step = ceil(numFrames / descriptorSize);

    for i=1:descriptorSize
        column = zeros(descriptorSize,1);
        column(i) = 1/step;
        column = repmat(column,1,step);
        oneHotDescriptor(:,(i-1)*step+1:i*step) = column;
    end

    oneHotDescriptor = oneHotDescriptor(:,1:numFrames);

end