%> @file calculate_errors_LLC.m
%>
%> Functions to compute the Linear Locality Constrained reconstruction error and sparsity term.
%> 
%> Function call:
%> @code
%> [ total_error, reconstructionTerm , localityTerm ] = calculate_errors_LLC ( dictionary, representation, coeff, lambda, weights )
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
%> @brief Calculates the total error of the LLC minimization problem.
%>
%> The reconstruction error is calculate through calculate_errors.m. It is not possible to calculate the sparsity term of the LLC solution using the L1-norm, once the LLC always create a solution vector with norm equals 1.
%>
%> @param  dictionary     - d x M @c double , dictionary with M words in a d-dim space.
%> @param  representation - d x 1 @c double , video representation in a d-dim space.
%> @param  coeff          - 1 x M @c double , vector of M coefficients related to each of the dictionary bases.
%> @param  lambda         - @c double       , regularization constant used during the LLC processing.
%> @param  weights        - 1 x M @c double , vector of M weights related to each of the dictionary bases (video frames).
%>
%> @retval total_error    - @c double       , weighted sum of the terms.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date   19/09/2019
% ========================================================================
function [ total_error, reconstructionTerm , localityTerm ] = calculate_errors_LLC ( dictionary, representation, coeff, lambda , weights )

    if ( nargin < 5 || isempty(weights) )
        weights = ones(size(dictionary,2),1);
    end

    %% Reconstruction term.
    [reconstructionTerm, ~ ]  = calculate_errors(dictionary,representation,coeff');
   
    %% locality term.
    D = distance_samples2dictionaryBases(dictionary',representation');
    localityTerm = sum(((D .* weights') .* coeff).^2);

    %% Total error.
    total_error = reconstructionTerm + lambda * localityTerm;
    
end

% ========================================================================
%> @brief Transform a d-dim vector to ( d x d ) matrix with the i-th vector value in the position M(i,i).
%>
%> @param  vector  - 1 x d @c double , vector in d-dim space.
%> 
%> @retval matrix  - d x d @c double , diagonal matrix with i-th vector position in M(i,i).
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date march 31/03/2017 
% ========================================================================
function [ matrix ] = vector2diag_matrix(vector)

    matrix = zeros(size(vector,2), size(vector,2));
    for i = 1:size(vector,2)
        matrix(i,i) = vector(i);
    end
     
%     for i = 1:length(vector)
%         E = zeros(length(vector),length(vector));
%         e = zeros(1,length(vector));
%         E(i,i) = 1;
%         e(i) = 1;
%         Z = E*vector'*e;
%         matrix = matrix + Z;
%      end
    
end

% ========================================================================
%> @brief Distance from samples to dictionary words.
%>
%> @param   dictionary  - M x d @c double  , dictionary with M words in a d-dim space.
%> @param   samples     - N x d @c double  , N data points in a d-dim space.
%>
%> @retval  distances   - 1 x M @c double  , vector which each postion is the distance of the sample for each dictionary base.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date   03/04/2017
% ========================================================================
function [ distances ] = distance_samples2dictionaryBases( dictionary, samples )

    nframe=size(samples,1); 
    nbase=size(dictionary,1);

    XX = sum(samples.*samples, 2);
    BB = sum(dictionary.*dictionary, 2);
    distances  = repmat(XX, 1, nbase)-2*samples*dictionary'+repmat(BB', nframe, 1);
end

% ========================================================================
%> @brief Computes the reconstruction error of the sample X using the coeff elements of the dictionary and the sparsity term of the Sparse Coding solution given by the coeff.
%>
%> @param dictionary     - M x d @c double , dictionary in a matrix form with M words in a d-dim space.
%> @param representation - 1 x d @c double , sample vector in a d-dim space.
%> @param coeff          - 1 x M @c double , coeff vector related to activations of the dictionary bases.
%>
%> @retval reconstructionError - @c double , square sum of the reconstruction error.
%> @retval sparsityTerm        - @c double , L1 norm of the spare coeff vector.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 21/08/2017
% ========================================================================
function [ reconstructionError , sparsityTerm ] = calculate_errors ( dictionary , representation , coeff )

    reconstructionError = reconstruction_error(dictionary, representation, coeff);
    sparsityTerm = sparsity_term(coeff);

end

% ========================================================================
%> @brief Calculates the reconstruction error of the sample X using the coeff elements of the dictionary.
%>
%> @param dictionary     - M x d @c double , dictionary in a matrix form with M words in a d-dim space.
%> @param representation - 1 x d @c double , sample vector in a d-dim space.
%> @param coeff          - 1 x M @c double , coeff vector related to activations of the dictionary bases.
%>
%> @retval error - @c double , square sum of the errors.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 03/04/2017
% ========================================================================
function [ error ] = reconstruction_error( dictionary , representation , coeff )
    
    error = sum((representation - dictionary * coeff).^2);

end

% ========================================================================
%> @brief Calculates the sparsity term of the Sparse Coding solution.
%>
%> @param coeff      - 1 x M @c double , coeff vector related to activations of the dictionary bases.
%>
%> @retval sparsity - @c double , L1 norm of the coeff vector.
%>
%> @author Michel M. Silva (michelms@dcc.ufmg.br)
%>
%> @date 21/08/2017
% ========================================================================
function [ sparsity ] = sparsity_term( coeff )
    
    sparsity = norm(coeff, 1);

end
