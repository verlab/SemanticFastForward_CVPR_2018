%> @file calculate_speedup_ranges.m
%>
%> Functions to calculate the speed-up rates over the segments.
%> 
%> Function call:
%> @code
%> [ speedup ] = calculate_speedup_ranges( ranges )
%> @endcode
%>
%> Possible values to varargin argument:
%> - \b Verbose             - @c boolean      , flag to display processing debug messages.

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
%> @brief Calculate the expected speedup rate using ranges.
%> 
%> @param  ranges  - 4 x N @c int , ranges in form of a matrix with dimensions 4 by N ( number of segments ).
%> @param varargin          - 1 X A @c string , list of A optional arguments. 
%> - \b Verbose             - @c boolean      , flag to display processing debug messages.
%>
%> @retval speedup - @c double , speed-up rate expected.
%>
%> @author Felipe C. Chamone (cadar@dcc.ufmg.br) 
%> 
%> @date 21/08/2017     
% ========================================================================
function [ speedup ] = calculate_speedup_ranges( ranges , varargin )

    p = inputParser;
    
    addOptional( p , 'Verbose'            , false  , @islogical );
    
    parse(p, varargin{:});

    totalframes = ranges(2, end);
    selectedframes = 0;

    for i=1:size(ranges, 2)

        segspeedup = ranges(3, i);
        frames = ranges(2, i) - ranges(1, i);
        semantic = ranges(4, i); 

        selectedframes = selectedframes + round(frames/segspeedup);

        if ( p.Results.Verbose )
            message_handle('L',sprintf('Seg: %3d | Speedup %2d | Size %6d | Semantic %d', i, segspeedup, frames, semantic) );
        end
    end

    speedup = totalframes / selectedframes;
    message_handle('I', sprintf('Final speedup: %2.2d | Selected Frames:  %6d | Original frames: %6d.', speedup, selectedframes, ranges(2,end)), 'ExtraSpace',true);
   
end

