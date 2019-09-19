%> @file get_weights_CDC.m
%>
%> Function to calculate weights based on the Cumulative Displacements Curves.
%> 
%> Functional call:
%> @code
%> [ weights ] = get_weights_CDC( OF )
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
%> @brief Calculate weights based on the Cumulative Displacements Curves <a href="http://ieeexplore.ieee.org/document/6909721/">(Poleg et al 2014)</a> [1].
%> 
%> [1] Y. Poleg, C. Arora and S. Peleg, **Temporal Segmentation of Egocentric Videos**, 2014 IEEE Conference on Computer Vision and Pattern Recognition, Columbus, OH, 2014, pp. 2537-2544. doi: 10.1109/CVPR.2014.325
%> 
%> @param  OF           - N x 50 @c double , Matrix of Optical Flow (25 fx and 25fy) of the 5x5 windowed N frames.
%> @param varargin      - 1 X A @c string , list of A optional arguments. 
%> - \b ShowFigures     - @c boolean      , flag to show imagens during the weight calculation processing.
%>
%> @retval weights      - 1 x N @c double , Sparse Coding Weights
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 20/10/2017 
%>
%> @todo This function is hardcoded to run in a 5x5 windowed image. Make this code generic for any windowed image setup.
%>
%> @see generate_CDC.m
% ========================================================================
function [ weights ] = get_weights_CDC( OF , varargin )

    %% Input parse.
    p = inputParser;
    
    addOptional( p , 'ShowFigures' , false , @islogical );
    
    parse(p, varargin{:});

    [ dx, ddx ] = generate_CDC( OF );

    framecount = size(dx, 1);
    
    weights = zeros(framecount, 1);
    minCurves = 23;
    %thr = 0.015;
    thr = 0.03;
    
%     for i=1:framecount-1
%         if( sum(ddx(i, :) > thr) > minCurves || sum(ddx(i, :) < (-1*thr)) > ( minCurves) )
%             weights(i) = 1;
%         end
%     end
     
    weights( sum((ddx > thr),2) > minCurves | sum((ddx < -thr),2) >  minCurves ) = 1;

     %Filter Signal
    sigma = 80;
    sz = 30;    
    x = linspace(-sz / 2, sz / 2, sz);
    gaussFilter = exp(-x .^ 2 / (2 * sigma ^ 2));
    gaussFilter = gaussFilter / sum (gaussFilter);
    
    weights = conv(weights, gaussFilter, 'same');
    weights(weights > 0) = 1;
          
    if ( p.Results.ShowFigures )
        
        points   = diff(weights');
        initials = find(points >0 );
        finals   = find(points <0 );
        
        figure(); plot(dx, 'LineWidth', 1.5); hold on;

        minDx = min(min(dx));
        maxDx = max(max(dx));

        if ~isempty(initials)
        
            if ( finals(1) < initials(1) )
                initials = [0 , initials];
            end

            if ( finals(end) < initials(end) )
               finals = [finals framecount]; 
            end

            %% Plot 
            % for i=1:length(initials)
            %     rectangle('Position', [inits(i), bott, (finals(i) - inits(i)), top - bott]);
            %     plot( [initials(i), initials(i)], [minDx, maxDx], 'g', 'LineStyle', '--', 'LineWidth', 2.0);
            %     plot( [finals(i), finals(i)], [minDx, maxDx], 'r', 'LineStyle', '--', 'LineWidth', 2.0);
            % end
            
        end
        
        new_weights(weights == 1) = 0.1;    
        new_weights(weights == 0) = 1;
        
        %plot(new_weights*maxDx, '--', 'LineWidth', 2); hold off;
        
    end
    
    weights(weights == 1) = 0.1;    
    weights(weights == 0) = 1;
        
end
