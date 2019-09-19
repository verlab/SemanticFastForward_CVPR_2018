%> @file add_frames_by_cost_matrix.m
%>
%> Function to select the best frame to minimize the transition cost between two frames.
%> 
%> Functional call:
%> @code
%> [ selectedFrames ] = add_frames_by_cost_matrix ( preselectedFrames, costsMatrixFilename, totalFrames , showImages  );
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
%> @brief Select the best frame to minimize the transition cost between two frames.
%>
%> @param preselectedFrames     - 1 x F @c int  , selected frames to insert transitions.
%> @param range                 - 4 x 1 @c int  , matrix of dimension 4 and 1.
%> @param costs                 - costs matrix
%> @param totalFrames           - @c int        , desired final number of frames.
%> @param showImages            - @c boolean    , 
%>
%> @retval selectedFrames       - 1 x M @c int  , vector with the M selected frames.
%>
%> @author Felipe Cadar Chamone (cadar@dcc.ufmg.br)
%> @date 19/09/2019
% ========================================================================
function [ selectedFrames ] = add_frames_by_cost_matrix ( preselectedFrames, range, costs, totalFrames , showImages )

    %Add frames to use cost matrix
    selectedFrames = preselectedFrames;
    i = 1;
    
    while i < length(selectedFrames)
        if selectedFrames(i+1) - selectedFrames(i) < 100
            i = i+1;            
        else
            from = selectedFrames(i);
            to   = selectedFrames(i+1);
            
            newFrame = floor((from + to)/2);
            
            selectedFrames = [selectedFrames(1:i) newFrame selectedFrames(i+1: end)];      
        end
        
    end
    
    if showImages == true
        figure(10); plot(selectedFrames, '.r');title('Selected Frames Before Using Costs');xlabel('Accelerated Video Frames');ylabel('Original Video Frames');
        figure(20); plot(selectedFrames(2:end) - selectedFrames(1:end-1), 'r');title('Distance Between Frames Before Using Costs');xlabel('Frame Index');ylabel('Distances');
    end
        
    % Frames to achieve desired speedUp
    frameCredit = totalFrames - length(selectedFrames);
%     message_handle('I', sprintf('Adding %i frames\n', frameCredit));
    
    costsMatrix = costs.appearance_cost;
    
    if showImages == true
        costsList = get_transition_costs(selectedFrames, costsMatrix, range);
        figure(50); plot(selectedFrames(1:end-1),costsList,'r'); title('Costs Between Frames'); xlabel('Frames');ylabel('Costs');
    end
       
    idx = 0;
    % Select Frames By cost
    
    while frameCredit > 0

        %% 1
        costsList = get_transition_costs(selectedFrames, costsMatrix, range);
        from = 0;
        to =1; 
        
        while from + 1 == to
            [ ~, argmax ] = max(costsList);
            costsList(argmax) = -Inf;
            from = selectedFrames(argmax);
            to = selectedFrames(argmax+1);
            
            % if all the frames of this segment are consecutive we cant improve any transition
            if(sum((costsList ~= -Inf) - (isnan(costsList))) == 0)  % detect if the vector is composed only with -Inf and Nan
                return;
            end
            
        end
        

        if(to >= size(costsMatrix, 1) || from > size(costsMatrix, 1))
           return; 
        end
        
        newFrame = find_best_transition(from, to, costsMatrix, range(3));

        selectedFrames = [selectedFrames(1:argmax) newFrame selectedFrames(argmax+1:end)];
        frameCredit = frameCredit - 1;
        idx = idx + 2;

    end
    
    if showImages == true
        costsList = get_transition_costs(selectedFrames, costsMatrix, range);
        figure(30); plot(selectedFrames, '.b');title('Selected Frames Using Costs');xlabel('Accelerated Video Frames');ylabel('Original Video Frames');
        figure(40); plot(selectedFrames(2:end) - selectedFrames(1:end-1), 'b');title('Distance Between Frames Using Costs');xlabel('Frame Index');ylabel('Distances');
        figure(50); hold on; plot(selectedFrames(1:end-1),costsList, '.b'); 
    end
           
end

%% --- Calculate transition costs -----------------------------------------------------------------------------------

% ===============================================================================
%> @brief Function to calculate the transition costs between the selected frames.
%> 
%> @param selectedFrames - F x 1   @c int   , Selected frames.
%> @param costs          - F x 100 @c float , Matrix with transition costs.
%> @param range          - 4 x 1 @c int  , matrix of dimension 4 and 1.
%>
%> @retval costsList     - F-1 x 1 @c int  , array with transition costs.
%>
%> @author Felipe C. Chamone (cadar@dcc.ufmg.br)
%>
%> @date 20/09/2017 
% ==============================================================================

function [ costsList ] = get_transition_costs(selectedFrames, costs, range)
    costsList = zeros(1,length(selectedFrames)-1);
    
    for i=1:(length(selectedFrames)-1)
        if ~(selectedFrames(i) > size(costs, 1))
           costsList(i) = costs(selectedFrames(i), selectedFrames(i+1)-selectedFrames(i)) * ((selectedFrames(i+1)-selectedFrames(i)- range(3) ) ) ;
        end
    end
    
end

%% --- Find transition frame ------------------------------------------------------------------------------------

% ===============================================================================
%> @brief Function to find the frame that minimizes the transition cost between two frames.
%> 
%> @param frame1    - @c int           , source frame.
%> @param frame2    - @c int           , destination frame.
%> @param costs     - F x 100 @c float , Matrix with transition costs.
%> @param speedup   - @c int           , desired speed-up value for the segment.
%>
%> @retval frame    - @c int           , Optimal transition frame.
%>
%> @author Felipe C. Chamone (cadar@dcc.ufmg.br)
%>
%> @date 20/09/2017 
% ==============================================================================
function [ frame ] = find_best_transition(frame1, frame2, costs, speedup)
    transition = [];
    speedup_weight = [];
    for i=frame1+1:frame2-1
        transition = [transition costs(frame1, i-frame1)^2 +  costs(i, frame2-i)^2];
        speedup_weight = [speedup_weight  (max( (speedup - (i - frame1)) , 0) + max( (speedup - (frame2 - i) ) , 0)) ];
    end
    
    transition = transition/max(transition);
    speedup_weight = speedup_weight/max(speedup_weight);
       
    transition = transition + (speedup_weight/2);
    
    [~, argmax] = min(transition);
    frame = argmax + frame1;
    
end
