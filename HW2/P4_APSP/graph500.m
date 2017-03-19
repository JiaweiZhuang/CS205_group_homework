SCALE = 14;
edgefactor = 24;
disScale = 10;

ij = kronecker_generator (SCALE, edgefactor, disScale);

fileID = fopen('rmat14.txt','w');
fprintf(fileID,'%d %d %d\n',ij);
fclose(fileID);
