%> @file save_accelerated_video.m
%>
%> Function to save in disk the accelerated video.
%> 
%> Functional call:
%> @code
%> save_accelerated_video(input_video_filename, output_video_filename, frames)
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
%> @brief Save in disk the selected frames to a video file.
%>
%> After saving the file, the function try to change the file permission.
%> 
%> @param inputVideoFilename   - @c string     , complete path, filename and extension to the MP4 file with the original video.
%> @param outputVideoFilename  - @c string     , complete path, filename and extension to the MP4 file where the selected frames will be written.
%> @param selectedFrames       - 1 x SF @c int , vector with the SF selected frames.
%>
%> @warning The path until the file must exist. 
%>
%> @authors Felipe C. Chamone (cadar@dcc.ufmg.br) , Michel M. Silva (michelms@dcc.ufmg.br)
%> 
%> @date 24/08/2017
%>
%> @see file_management.m
%>
% ========================================================================
%% Create a new video file with the selected frames
function save_accelerated_video(inputVideoFilename, outputVideoFilename, selectedFrames)

    outputVideoFilename = [outputVideoFilename '.avi'];

    %% Reading input video
    message_handle('I','Reading input video...');
    reader = VideoReader(inputVideoFilename);
    reader.NumberOfFrames;
    message_handle('I','Video loaded...');
    
    %% Video writer stuff
    writer = VideoWriter(outputVideoFilename);
    writer.FrameRate = reader.FrameRate;
    writer.open();
    
    numFrames = size(selectedFrames,2);
    
    message_handle('S',sprintf('Writing %s frames to %s at %s fps...', num2str(numFrames), outputVideoFilename, num2str(reader.FrameRate)));
    
    for i=1:numFrames
        frame = read(reader,selectedFrames(i));
        writeVideo(writer, frame);
        progress_bar(i, numFrames)
    end
    
    writer.close();    
    file_management( outputVideoFilename );
    
    message_handle('S','Saving done.');
end
