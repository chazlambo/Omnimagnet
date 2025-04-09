classdef multimagnet < handle
    %System of multiple omnimagnets interacting simultaneously
    
    properties (Access = protected)
        omnisystem
        numOmni
    end
    
    methods (Access = public)
        function obj = multimagnet(omnisystem)
            if nargin>0
                obj.omnisystem = omnisystem;
                obj.numOmni = length(omnisystem);
            end
        end
        
        function obj = SetMagnet(obj,index, omnimagnet)
            obj.omnisystem(index)=omnimagnet;
        end
        
        function obj = AddMagnet(obj,omnimagnet)
            obj.omnisystem(end+1)=omnimagnet;
            obj.numOmni = length(obj.omnisystem);
        end
        
        function obj = RemoveMagnet(obj,index)
            obj.omnisystem(index) = [];
            obj.numOmni = length(obj.omnisystem);
        end
        
        function current = Zdes2Current(obj,Zdes,toolPos,toolDip,Wmat)
            %Finds minimized current for desired torque and force using
            %a given positive-definite symmetric weighting matrix, and
            %making the assumption that Zt is negligible
            %Equation 24 in the Omnimagnet System paper
            
            A = obj.Amat(toolPos,toolDip);
            D = obj.Dmat;
            M = obj.Mmat;
            W = Wmat^(-1/2);
            current = W*pinv(A*D'*M*W)*Zdes;
        end
        
        function obj = SetSystemCurrent(obj,current)
            if length(current)== obj.numOmni*3
                for n = 1:obj.numOmni
                    obj.omnisystem(n).SetCurrent(current(3*n-2:3*n));
                end
            else
                fprintf("The input current vector is the wrong size");
            end
        end
        
        function prop = GetProp(obj, index)
            prop = obj.omnisystem(index).GetProp;
        end
        
    end
    
    methods(Access = protected)
        function Pmap = Pmap(~, pos_i,pos_j)
            %Generates a 3x3 |P Mapping matrix for two given position vectors
            %Equation 1 of Omnimagnet System Paper
            dispVec = pos_i-pos_j;
            vecNorm = norm(dispVec);
            normDir = normalize(dispVec,'norm');
            
            Pmap = (3*normDir.*normDir' - eye(3))/(vecNorm.^3);
        end
        
        function Fmap = Fmap(~, pos_i,pos_j,dip_i)
            %Generates a 3x3 |F Mapping matrix for two given position vectors
            %Equation 4 of Omnimagnet System Paper
            dispVec = pos_i-pos_j;
            vecNorm = norm(dispVec);
            normDir = normalize(dispVec,'norm');
            
            Fmap = (dip_i*normDir' + normDir*dip_i' + (dot(normDir,dip_i)).*(eye(3)-5.*normDir*normDir'))/vecNorm^4;
        end
        
        function Dmat = Dmat(obj)
            %Generates the |D matrix defined in Eq. 11 of the Omnimagnet System Paper
            matrix = zeros(obj.numOmni*3);
            
            for i = 1:obj.numOmni
                for j = 1:obj.numOmni
                    if i == j
                        newMat = zeros(3);
                    else
                        newMat = -obj.omnisystem(i).GetCoreSize^3*obj.Pmap(obj.omnisystem(i).GetPosition,obj.omnisystem(j).GetPosition);
                    end
                    matrix(3*i-2:3*i,3*j-2:3*j) = newMat;
                end
            end
            
            Dmat = matrix;
        end
        
        function Mmat = Mmat(obj)
            %Generates the |M matrix defined in Eq. 12 of the Omnimagnet System Paper
            matrix = zeros(obj.numOmni*3);
            for i = 1:obj.numOmni
                for j = 1:obj.numOmni
                    if i == j
                        newMat = obj.omnisystem(i).GetMapping;
                    else
                        newMat = zeros(3);
                    end
                    matrix(3*i-2:3*i,3*j-2:3*j) = newMat;
                end
            end
            Mmat = matrix;
        end
        
        function skew = skew(~,vec)
            %Generates skew symmetric matrix of a 1x3 vector
            %Note: Matlab has a more expanded version of this program in it's "Robust Control Toolbox"
            skew=[0 -vec(3) vec(2);
                vec(3) 0 -vec(1);
                -vec(2) vec(1) 0 ];
        end
        
        function Amat = Amat(obj,toolPos,toolDip)
            %Generates the torque-force actuation matrix |A for single tool
            %Equation 18 in the Omnimagnet System paper
            S = [obj.skew(toolDip),zeros(3);zeros(3),3*eye(3)];
            Ptmat=zeros(3,3*obj.numOmni);
            Ftmat=zeros(3,3*obj.numOmni);
            
            for n = 1:obj.numOmni
                Ptmat(1:3,(3*n-2):3*n)=obj.Pmap(toolPos,obj.omnisystem(n).GetPosition);
                Ftmat(1:3,(3*n-2):3*n)=obj.Fmap(toolPos,obj.omnisystem(n).GetPosition,toolDip);
            end
            
            T = [Ptmat;Ftmat];
            Amat = 1e-7*S*T;
        end
        
    end
end


