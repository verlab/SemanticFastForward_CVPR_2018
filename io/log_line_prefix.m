%> @file log_line_prefix.m
%>
%> Function to print a formatted date information.
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
%> @brief Print a formatted date information.
%>
%> Example of call:
%> 
%> @code 
%> log_line_prefix 
%> @endcode
%> 
%> Output:
%>
%>@verbatim [22-Aug-2017 11:07:27] - @endverbatim
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 20/03/2017 
% ========================================================================
function str = log_line_prefix
    str = sprintf('[%s] - ',datestr(now));
end