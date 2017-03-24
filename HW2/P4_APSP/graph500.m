SCALE = 6;
edgefactor = 24;
disScale = 10;

ij = kronecker_generator (SCALE, edgefactor, disScale);

fileID = fopen('rmat6.txt','w');
fprintf(fileID,'%d %d %d\n',ij);
fclose(fileID);
