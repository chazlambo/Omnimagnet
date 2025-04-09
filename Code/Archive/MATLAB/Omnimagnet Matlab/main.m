clc, clear all

omni{1} = omnimagnet(1,1,1,1,1,1,1,1,1);
omni{1}.SetPosition([1,1,1]');
omni{1}.mapping_=randi([0,10],3);

omni{2} = omnimagnet(2,2,2,2,2,2,2,2,1);
omni{2}.SetPosition([2,2,2]');
omni{2}.mapping_=randi([0,10],3);

omni{3} = omnimagnet(3,3,3,3,3,3,3,3,1);
omni{3}.SetPosition([3,3,3]');
omni{3}.mapping_=randi([0,10],3);

omni{4} = omnimagnet(4,4,4,4,4,4,4,4,1);
omni{4}.SetPosition([4,4,4]');
omni{4}.mapping_=randi([0,10],3);

omni{5} = omnimagnet(5,5,5,5,5,5,5,5,1);
omni{5}.SetPosition([5,5,5]');
omni{5}.mapping_=randi([0,10],3);

toolPos = [1,2,3]';
toolDip = [4,5,6]';

system = [omni{1},omni{2},omni{3},omni{4},omni{5}];

multi = multimagnet(system);

Wmat = eye(15);
Zdes = [10,10,0,10,0,0]';





