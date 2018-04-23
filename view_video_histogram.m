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

function view_video_histogram( videoFilename , histogram )

    message_handle('S','Reading Video...');
    v=VideoReader(videoFilename);
    message_handle('S','Start display');
    
    currAxes = axes;
    i = 0;
    max_mh = max(max(histogram(1:50, :)));
    min_mh = min(min(histogram(1:50, :)));
    max_oh = max(max(histogram(51:122, :)));
    min_oh = min(min(histogram(51:122, :)));
    
    while hasFrame(v)
        i = i +1;
        
        figure(1);
        vidFrame = readFrame(v);
        image(vidFrame, 'Parent', currAxes);
        currAxes.Visible = 'off';
        
        figure(2); bar(histogram(1:50,  i)); ylim([min_mh max_mh]);
        figure(3); bar(histogram(51:122,  i)); ylim([min_oh max_oh]);
        pause(0.0001);
    end

end

