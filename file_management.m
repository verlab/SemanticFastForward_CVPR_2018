%> @file file_management.m
%>
%> Function to manage disk in disk.
%>
%> Function call:
%> @code
%> [ success ] = file_management( filename )
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
%> @brief Manage files in disk.
%>
%> Change file permission to 774 ( -rwxrwxr-- ) mode.
%>
%> @param filename          - @c string , complete path, with filename and extension to the file to be modified.
%> 
%> @retval sucess           - @c bool   , return the status of the changing file permission.
%> 
%> @warning The file permission used into this function is compatible with UNIX systems. If used in Windows Operational System, change the permissions attributes.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 24/08/2017 
% ========================================================================
function [ success ] = file_management( filename )

    [ success , msg , ~ ] = fileattrib(filename,'+w +x','u');
    if ~success
        message_handle('E', sprintf('Error during changing file permissions: (%s)| File: %s',msg, filename));
    end

    [ success , msg , ~ ] = fileattrib(filename,'+w +x','g');
    if ~success
        message_handle('E', sprintf('Error during changing file permissions: (%s) | File: %s',msg, filename));
    end

end
