%> @file progress_bar.m
%>
%> Function to print a progress bar on the default output.
%> 
%> Functional call:
%> @code
%> progress_bar(iterator, total)
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
%> @brief Print a progress bar. 
%>
%> The default output will be replaced with a bar indicating the progress of the iterator over the total.
%>
%> @param iterator - @c int , actual iterator position.
%> @param total    - @c int , total number of steps need to complete the processing.
%>
%> Example of call:
%> 
%> @code 
%> progress_bar(48, 100) 
%> @endcode
%> 
%> Output:
%>
%>@verbatim
%>[22-Aug-2017 11:07:27] - [I] --> 48% complete. 
%>
%>[████████████████████████--------------------------] 
%>@endverbatim
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 20/03/2017 
% ========================================================================
function progress_bar(iterator, total)
    percentage = round(100*(iterator/(2*total)));
    factor = 0.02;

    if iterator == 1
        fprintf(' %s[I] --> 0%% complete. \n\n', log_line_prefix);
        fprintf([' [' repmat('-', 1, 50) '] \n\n']);
    end

    if mod(iterator, 20) == 0
        n = length([' ' log_line_prefix '[I] --> ' num2str(factor*percentage*100) '% complete. \n\n'])+length([' [' repmat('-', 1, 50) ']']);
        if (factor*percentage*100)==10 || (factor*percentage*100)==100
            fprintf(repmat('\b',1,n));
        else
            fprintf(repmat('\b',1,n+1));
        end
        fprintf([' %s[I] --> ' num2str(factor*percentage*100) '%% complete. \n\n'], log_line_prefix);
        fprintf([' [' repmat('█', 1, percentage) repmat('-', 1, 50-percentage) '] \n\n']);
    end
end