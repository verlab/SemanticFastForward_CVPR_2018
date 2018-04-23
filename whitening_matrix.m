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
%> @brief ZCA whitening of a data matrix (make the covariance matrix an identity matrix)
%>
%> @warning This form of whitening performs poorly if the number of dimensions are much greater than the number of instances.
%>
%> @param X       - m x n @c double , matrix of data with m rows containing the instances and n columns cointaining the features.
%> @param epsilon - @c double       , small number to compensate for nearly 0 eigenvalue [DEFAULT = 0.0001].
%>
%> @retval Xwh: whitened data, rows are instances, columns are features
%> @retval mu: mean of each feature of the orginal data
%> @retval invMat: the inverse data whitening matrix
%> @retval whMat: the whitening matrix
%>
%> EXAMPLE
%> @code
%> X = rand(100,20); % 100 instance with 20 features
%> 
%> figure; imagesc(cov(X)); colorbar; title('original covariance matrix');
%> 
%> [Xwh, mu, invMat, whMat] = whiten(X,0.0001);
%> 
%> figure;
%> imagesc(cov(Xwh)); colorbar; title('whitened covariance matrix');
%> 
%> Xwh2 = (X-repmat(mean(X), size(X,1),1))*whMat; 
%> figure;
%> plot(sum((Xwh-Xwh2).^2),'-rx'); title('reconstructed whitening error (should be 0)');
%> 
%> Xrec = Xwh*invMat + repmat(mu, size(X,1),1);
%> figure;
%> plot(sum((X-Xrec).^2),'-rx'); ylim([-1,1]); title('reconstructed data error (should be zero)');
%> 
%> @endcode
%> 
%> @author Colorado Reed colorado-reed@uiowa.edu
% ========================================================================
function [Xwh, mu, invMat, whMat] = whitening_matrix(X,epsilon)


    if ~exist('epsilon','var')
        epsilon = 0.0001;
    end

    mu = mean(X); 
    X = bsxfun(@minus, X, mu);
    A = X'*X;
    [V,D,~] = svd(A);
    whMat = sqrt(size(X,1)-1)*V*sqrtm(inv(D + eye(size(D))*epsilon))*V';
    Xwh = X*whMat;  
    invMat = pinv(whMat);

end

