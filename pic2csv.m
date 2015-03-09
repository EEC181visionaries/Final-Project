function pic2csv(jpg_name, save_name)
% NOT WORKING YET
% Takes in a name of a jpg image. Reads
% jpg image into a matrix. Converts 
% matrix into FPGA formatted arrays. 
% Name used to indicate csv file name.

% Read in image and store in a matrix
string = strcat(jpg_name,'.jpg');
image = imread(string);

% Separate matrix into 3 arrays
a1 = image(:,:,1);                          % Stores R into array
a2 = image(:,:,2);                          % Stores G into array
a3 = image(:,:,3);                          % Stores B into array

% Transpose all matricies
b1 = a1';                                   
b2 = a2';
b3 = a3';

% Convert to a 1D arrays
c1 = reshape(b1, 7990272, 1);               
c2 = reshape(b2, 7990272, 1);
c3 = reshape(b3, 7990272, 1);

% Create the file names
name1 = strcat(save_name,'1.csv');      
name2 = strcat(save_name,'2.csv');

% Write csv files
dlmwrite(name1, c1);
dlmwrite(name2, c2);
