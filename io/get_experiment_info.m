%> @file get_experiment_info.m
%>
%> Function to get the computer name and experiment ID.
%> 
%> Functional call:
%> @code
%> [ computerID , experimentID ] = get_experiment_info()
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
%> @brief Get informations about the host computer experiment id.
%> 
%> @retval computerID   - @c string , computer host name. If that information is unavailable, the function will return 'unknow' instead.
%> @retval experimentID - @c int    , experiment number used to avoid overwritten over old files.
%> 
%> @warning This function will create the file '.ID_MANAGER' to manage the. experiment identifier, in case of erasing this file, the id will restart from 1.
%> 
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 23/08/2017 
% ========================================================================
function [ computerID , experimentID ] = get_experiment_info()
       
    if exist('.ID_MANAGER','file')
        fileID = fopen('.ID_MANAGER', 'r');
        experimentID = fscanf(fileID,'%d');
        fclose(fileID);
    else
        experimentID = 1;
    end
    
    fileID = fopen('.ID_MANAGER', 'w+');
    fprintf(fileID,'%d',experimentID+1);
    fclose(fileID);
    
    [ status , cmdout] = system('hostname');   
    if status
        message_handle('E', sprintf('Unable to get information about the : (%s)',msg));
        computerID = 'unknown';
    else
        computerID = cmdout(1:end-1);
    end
    
end