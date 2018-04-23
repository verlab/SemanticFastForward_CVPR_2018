%> @file calculate_errors.m
%>
%> Functions to compute the sparse code reconstruction error and sparsity term.
%> 
%> Function call:
%> @code
%> [ reconstructionError , sparsityTerm ] = calculate_errors ( dictionary , representation , coeff )
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


