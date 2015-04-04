function zzpic2arrays(jpg_name, save_name)
% Takes in a name of a jpg photo
% saves all 3 matricies into the 
% name indicated for saving.

% Read in image and store in a matrix
string = strcat(jpg_name,'.jpg');
image = imread(string);

% Separate matrix into 3 arrays
a1 = image(:,:,1);                          % Stores R into array
a2 = image(:,:,2);                          % Stores G into array
a3 = image(:,:,3);                          % Stores B into array

% Write first array
b1 = a1';                                   % Transpose matrix
c1 = reshape(b1, 7990272, 1);               % Convert to a 1D array
name1 = strcat(save_name,'1.csv');          % Create the file name
dlmwrite(name1, c1);  % Write the csv file

% Write second array
b2 = a2';
c2 = reshape(b2, 7990272, 1);
name2 = strcat(save_name,'2.csv');
%dlmwrite(name2, c2, 'precision', '%0.9f');

% Write third array
b3 = a3';
c3 = reshape(b3, 7990272, 1);
name3 = strcat(save_name,'3.csv');
%dlmwrite(name3, c3, 'precision', '%0.9f');

% Rebuild the original image
%[x, y, z] = size(image);





% Rebuild the original image
%{
[x, y, z] = size(image);

d1 = b1';
d2 = b2';
d3 = b3';

new = uint8(zeros(x,y,z));
for i = 1:x
  for j = 1:y
    for k = 1:z
        switch k
            case 1
              new(i,j,k) = d1(i,j);
            case 2
              new(i,j,k) = d2(i,j);
            case 3
              new(i,j,k) = d3(i,j);
        end
    end
  end
end

output = new;
%}
