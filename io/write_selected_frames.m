%> @file write_selected_frames.m
%>
%> Function to save the selected frames and speed-ups in disk.
%> 
%> Functional call:
%> @code
%> write_selected_frames(filename, ranges, selectedFrames)
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
%> @brief Write the selected frames to a CSV file in a specific path.
%>
%> After saving the file, the function try to change the file permission.
%> 
%> @param filename       - @c string     , complete path, filename and extension to the CSV file where the selected frames will be written.
%> @param ranges         - 4 x N  @c int , ranges in form of a matrix with dimensions 4 by N ( number of segments ).
%> @param selectedFrames - 1 x SF @c int , vector with the SF selected frames.
%>
%> @warning The path until the file must exist. 
%>
%> @authors Felipe C. Chamone (cadar@dcc.ufmg.br) , Michel M. Silva (michelms@dcc.ufmg.br)
%> 
%> @date 24/08/2017
%>
%> @see file_management.m
% ========================================================================
function write_selected_frames(filename, ranges, selectedFrames)

    selectedFrames_and_speedup = [];
    j = 1;
    for i=1:length(selectedFrames)
        if selectedFrames(i) > ranges(2, j)
            j = j + 1;
        end
        selectedFrames_and_speedup = [selectedFrames_and_speedup;[ selectedFrames(i) ranges(3,j)] ];
    end

    csvwrite(filename, selectedFrames_and_speedup);
    file_management(filename);
end