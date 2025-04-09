classdef omnimagnet < handle
    %Singular omnimagnet
    properties(Access = public)
        
        subdev
        chan
        range
        subdevice
        res
        estimate_
        wire_width
        wire_len_in
        wire_len_mid
        wire_len_out
        core_size
        D2A_pin_number
        
        orientation_
        axis_rot_Z
        temperature_
        power_
        position_
        current_
        current_density
        mapping_
        max_current
        
        
    end
    
    methods
        function obj = omnimagnet(wirewidth,wirelenin,wirelenmid,wirelenout,coresize,pinin,pinmid,pinout,estimate)
            if nargin>0
                obj = SetProp(obj,wirewidth,wirelenin,wirelenmid,wirelenout,coresize,pinin,pinmid,pinout,estimate);
            end
        end
        
        function obj = SetProp(obj,wirewidth,wirelenin,wirelenmid,wirelenout,coresize,pinin,pinmid,pinout,estimate)
            obj.wire_width = wirewidth;
            obj.wire_len_in = wirelenin;
            obj.wire_len_mid = wirelenmid;
            obj.wire_len_out = wirelenout;
            obj.core_size = coresize;
            obj.orientation_ = 0.0;
            obj.current_ = [0,0,0];
            obj.mapping_ = [0,0,0;
                0,0,0;
                0,0,0];
            obj.D2A_pin_number = [pinin, pinmid, pinout];
            obj.estimate_ = estimate;
        end
        function prop = GetProp(obj)
            prop = [obj.wire_width, obj.wire_len_in, obj.wire_len_mid, obj.wire_len_out, obj.core_size, obj.D2A_pin_number, obj.estimate_];
        end
        
        function obj = UpdateMapping(obj)
            if (obj.estimate_==1)
                obj.mapping_ = eye(3).*((51.45*10^(-3)*0.115^4));
                %obj.axis_rot_Z = (Eigen::AngleAxisd(orientation_*M_PI/180, Eigen::Vector3d::UnitZ()));
                obj.axis_rot_Z = normalize(obj.axis_rot_Z,'norm');
                obj.mapping_ = obj.axis_rot_Z.*obj.mapping_;
            else
                fprintf("No method, use the estimate method.");
            end
        end%UNFINISHED
        function mapping = GetMapping(obj)
            mapping = obj.mapping_;
        end
        
        function obj = SetCurrent(obj,current)
            obj.current_=current;
        end
        function current = GetCurrent(obj)
            current = obj.current_;
        end
        
        function orientation = GetOrientation(obj)
            orientation=obj.orientation_;
        end
        function  obj = SetOrientation(obj,orientation)
            obj.orientation_=orientation;
            obj.UpdateMapping;
        end
        
        function current = Dipole2Current(obj, dipole)
            current = 1; %REMOVE ONCE LINE BELOW IS TRANSLATED
            %current = mapping_.completeOrthogonalDecomposition().solve(dipole);
            current = current.*(obj.wire_width^2);
        end%UNFINISHED
        
        function position = GetPosition(obj)
            position=obj.position_;
        end
        function obj = SetPosition(obj,position)
            obj.position_=position;
        end
        
        function temperature = GetTemperature(obj)
            temperature=obj.temperature_;
        end
        function obj = SetTemperature(obj,temperature)
            obj.temperature_=temperature;
        end
        
        function power = GetPower(obj)
            power=obj.power_;
        end
        function obj = SetPower(obj,power)
            obj.power_=power;
        end
        
        function coresize = GetCoreSize(obj)
            coresize = obj.core_size;
        end
        
        %I don't know how to implement the RotatingDipole function
    end
end

