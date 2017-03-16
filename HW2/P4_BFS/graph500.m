SCALE = 5;
edgefactor = 24;

ij = kronecker_generator (SCALE, edgefactor);

fileID = fopen('rmat5.txt','w');
fprintf(fileID,'%d %d\n',ij);
fclose(fileID);
