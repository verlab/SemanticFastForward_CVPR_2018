%> @file check_paths.m
%>
%> Function to check existence and permission of paths.
%> 
%> Functional call:
%> @code
%> [ checked ] = check_paths( paths )
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
%> @brief Check the existence and permission of paths and files where the 
%> code will try read or write.
%> 
%> @param  paths   - @c struct  , struct with all paths to be checked.
%>
%> @retval checked - @c bool    , flag to indicate if all paths are okay.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 10/04/2018 
% ========================================================================
function [ checked ] = check_paths( paths )

    checked = check_paths_in(paths.in) && check_paths_out(paths.out);

end

% ========================================================================
%> @brief Check the existence of paths and files where the code will read.
%> 
%> @param  paths   - @c struct  , struct with all paths to be checked.
%>
%> @retval checked - @c bool    , flag to indicate if all paths are okay.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 10/04/2018 
% ========================================================================
function [ checked ] = check_paths_in( paths )

    checked = true;
    
    fields = fieldnames( paths );

    for i=1:numel(fields);
        
        fileName = paths.(fields{i});
        
        isDir = exist(fileName, 'dir');
        
        if isDir
            
            [fid,errmsg] = fopen( [fileName '/check_path.txt'] , 'w');
            if ~isempty(errmsg)
                message_handle('E', sprintf('%s: %s.',errmsg,fileName));
                checked = cheked && false;
            else 
                fclose(fid);
                delete( [fileName '/check_path.txt'] );
            end
            
        else
        
            if exist(fileName, 'file')
                [fid,errmsg] = fopen(fileName, 'r');
                if ~isempty(errmsg)
                    message_handle('E', sprintf('%s: %s.',errmsg,fileName));
                    checked = cheked && false;
                else 
                    fclose(fid);
                end
            else
                message_handle('E', sprintf('File does not existis: %s.',fileName));
                checked = checked && false;
            end
            
        end
        
    end

end

% ========================================================================
%> @brief Check the existence and permission of paths and files where the 
%> code will write.
%> 
%> @param  paths   - @c struct  , struct with all paths to be checked.
%>
%> @retval checked - @c bool    , flag to indicate if all paths are okay.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 10/04/2018 
% ========================================================================
function [ checked ] = check_paths_out( paths )

    checked = true;
    
    fields = fieldnames( paths );

    for i=1:numel(fields);
        
        fileName = paths.(fields{i});
        
        isDir = exist(fileName, 'dir');
        
        if isDir
            
            [fid,errmsg] = fopen( [fileName '/check_path.txt'] , 'w');
            if ~isempty(errmsg)
                message_handle('E', sprintf('%s: %s.',errmsg,fileName));
                checked = checked && false;
            else 
                fclose(fid);
                delete( [fileName '/check_path.txt'] );
            end
            
        else
        
            fileExists = exist(fileName, 'file');

            if fileExists
                [fid,errmsg] = fopen(fileName, 'r');
            else
                [fid,errmsg] = fopen(fileName, 'w');
            end

            if ~isempty(errmsg)
                message_handle('E', sprintf('%s: %s.',errmsg,fileName));
                checked = checked && false;                
            elseif fileExists 
                fclose(fid);
            else
                fclose(fid);
                delete(fileName);
            end
            
        end
        
    end

end
