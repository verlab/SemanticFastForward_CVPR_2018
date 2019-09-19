%> @file write_general_results.m
%>
%> Function to print in a CSV file the informations about the experiments.
%> 
%> Functional call:
%> @code
%> checked = write_general_results ( filename , id, experiment, semanticExtractor, inputParse )
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
%> @brief Print the formatted informations information about the experiments into a CSV file.
%>
%> @param   filename          - @c string , complete path and name with extension to the file.
%> @param   id                - @c struct , struct with the fields experiments and computer.
%> @param   experiment        - @c string , experiment name.
%> @param   experimentType    - @c string , experiment type [ 'LLC' ].
%> @param   semanticExtractor - @c string , semantic used to extract the information.
%> @param   inputParse        - @c struct , struct with all input arguments received during the program calling.
%>
%> @retval  checked           - @c bool   , indicates if the file was correctly created and the permissions were changed.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 19/09/2019
% ========================================================================
function checked = write_general_results ( filename , id, experiment, experimentType, semanticExtractor, inputParse )

    fileExists = exist(filename, 'file');
    fid = fopen(filename,'a');
        
    if ~fileExists 
        header = 'Experiment_ID,Experiment,Experiment_Type,Semantic_Extractor,Costs_Mode,Multi_Importance,Computer,Speed_up,Speed_up_factor,Date\n';
        fprintf(fid, header);
        file_management(filename);
    end
    
    LogicalStr = {'false', 'true'};
    date = log_line_prefix;
    
    newEntry = [num2str(id.experiment) ',' experiment ',' experimentType ',' semanticExtractor ',' 'Appearance' ',' ...
        'true' ',' id.computer ',' num2str(inputParse.Results.Speedup) ',' num2str(inputParse.Results.SpeedupFactor) ',' date(2:end-4) '\n'];
    
    fprintf(fid, newEntry);
    checked = ~fclose(fid);
end
