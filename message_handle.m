%> @file message_handle.m
%>
%> Function to print formatted string at the standard output.
%> 
%> Functional call:
%> @code
%> message_handle( status , message , varargin)
%> @endcode
%>
%> Use the following status:
%> - 'E' : Error
%> - 'S' : Status
%> - 'I' : Information
%> - 'L' : Log
%> - ''  : Default text
%>
%> Possible values to the optional arguments varargin:
%> - \b TopLine    - @c boolean      , print a top line.
%> - \b BottomLine - @c boolean      , print a bottom line.
%> - \b ExtraSpace - @c boolean      , print a line break before and after.

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
%> @brief Function to print a formatted string on the standard output.
%> 
%> Use the following status:
%> - 'E' : Error
%> - 'S' : Status
%> - 'I' : Information
%> - 'L' : Log
%> - ''  : Default text
%>
%> @param status   - @c string       , status id.
%> @param message  - @c string       , message to be printed.
%> @param varargin - 1 x S @c string , vector of S string with optional arguments.
%> - \b TopLine    - @c boolean      , print a top line.
%> - \b BottomLine - @c boolean      , print a bottom line.
%> - \b ExtraSpace - @c boolean      , print a line break before and after.
%>
%> @authors Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 24/08/2017 
% ========================================================================
function message_handle ( status, message, varargin)

    p = inputParser;

    addOptional(p, 'TopLine', false, @islogical);    
    addOptional(p, 'BottomLine', false, @islogical);
    addOptional(p, 'ExtraSpace', false, @islogical);
    
    parse(p, varargin{:});

    topLine = p.Results.TopLine;
    bottomLine = p.Results.BottomLine;
    extraSpace = p.Results.ExtraSpace;
    
    if extraSpace
        fprintf('\n');
    end
            
    if topLine
        fprintf('------------------------------------------------------------------------------------------------------------------------\n');
    end    
    
    switch status
        case 'E'            
            fprintf('\n');
            cprintf('*Errors',' [E] --> ' );
            cprintf('Errors',message );
            fprintf('\n');
        case 'S'
            cprintf('*Keyword',' [S] --> ' );
            cprintf('Keyword',message );
        case 'I'
            cprintf('*[0,0.7,0]',sprintf(' [I] --> '));
            cprintf('[0,0.7,0]', message );
        case 'L'
            cprintf('Text',sprintf(' [L] --> %s', message) );
        case ''
            fprintf( message );
        otherwise
            fprintf('[%s] --> %s ', status, message);
    end
    
    if bottomLine
        fprintf('\n------------------------------------------------------------------------------------------------------------------------\n');
    elseif extraSpace
        fprintf('\n');
    end
    
    fprintf('\n');
    
end

