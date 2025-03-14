Radius = 3e-3; % m
Height = 4e-3; % m
Remanence = 1.35; % Tesla for N45 Magnet


lis3mdl_positions = cell(1, 16);

for i = 1:numel(lis3mdl_positions)
    lis3mdl_positions{i} = struct('x', 40e-3 + floor((i - 1) / 4) * 30e-3, 'y', 45e-3 + mod(i - 1, 4) * 30e-3, 'z', 0);
end

%crappy edge fartest away from the array:
holes_positions_direction_magnet_faces_x = cell(10, 11);


for x_pos = 1:12
    for y_pos = 1:13
        holes_positions_direction_magnet_faces_x{x_pos, y_pos} = struct('x', 37.5e-3 + (x_pos - 1) * 15e-3, 'y', 30e-3 + (y_pos - 1) * 15e-3, 'z', 50e-3 + 2e-3 + Radius);
    end
end

holes_positions_direction_magnet_face_y = cell(11, 10);

for x_pos = 1:13
    for y_pos = 1:12
        holes_positions_direction_magnet_face_y{x_pos, y_pos} = struct('x', 30e-3 + (x_pos - 1) * 15e-3, 'y',  37.5e-3 + (y_pos - 1) * 15e-3, 'z', 50e-3 + 2e-3 + Radius);
    end
end

function B = dipol(sensor_position, magnet_position, magnet_radius, magnet_height, magnet_remanence, magnet_magnetic_direction)
    magnetic_permeability = 4 * pi * 1e-7; % N/A²
    
    r = [magnet_position.x - sensor_position.x, magnet_position.y - sensor_position.y, magnet_position.z - sensor_position.z]; % Vector vom Sensor zum Magneten
    M = magnet_remanence / magnetic_permeability; % Magnetisierung
    m = magnet_magnetic_direction * M * pi * magnet_radius^2 * magnet_height;

    B = magnetic_permeability / (4 * pi) * ((3*r*(m*r')) ./ norm(r)^5 - m ./ norm(r)^3);
end

function B = biot_savat(sensor_position, magnet_position, magnet_radius, magnet_height, magnet_remanence) % magnet_magnetic_direction can only be [1, 0, 0].
    magnetic_permeability = 4 * pi * 1e-7; % N/A²
    M = magnet_remanence / magnetic_permeability; % Magnetisierung

    idet = @(a, b, c, theta, r, x) -sin(theta) .* (c - r .* sin(theta)) - cos(theta) .* (b - r .* cos(theta));
    jdet = @(a, b, c, theta, r, x) cos(theta) .* (a - x);
    kdet = @(a, b, c, theta, r, x) sin(theta) .* (a - x);
    
    ifun = @(a, b, c, theta, r, x) idet(a, b, c, theta, r, x) ...
        ./ ((a - x).^2 + (b - r .* cos(theta)).^2 + (c - r .* sin(theta)).^2).^(3/2);
    
    jfun = @(a, b, c, theta, r, x) jdet(a, b, c, theta, r, x) ...
        ./ ((a - x).^2 + (b - r .* cos(theta)).^2 + (c - r .* sin(theta)).^2).^(3/2);
    
    kfun = @(a, b, c, theta, r, x) kdet(a, b, c, theta, r, x) ...
        ./ ((a - x).^2 + (b - r .* cos(theta)).^2 + (c - r .* sin(theta)).^2).^(3/2);

    r = [magnet_position.x - sensor_position.x, magnet_position.y - sensor_position.y, magnet_position.z - sensor_position.z]; % Vector vom Sensor zum Magneten

    ifun2 = @(x, theta) ifun(r(1), r(2), r(3), theta, magnet_radius, x);
    jfun2 = @(x, theta) jfun(r(1), r(2), r(3), theta, magnet_radius, x);
    kfun2 = @(x, theta) kfun(r(1), r(2), r(3), theta, magnet_radius, x);
    
    
    B = [integral2(ifun2, -magnet_height / 2, magnet_height / 2, 0, 2 * pi), integral2(jfun2, -magnet_height / 2, magnet_height / 2, 0, 2 * pi), integral2(kfun2, -magnet_height / 2, magnet_height / 2, 0, 2 * pi)] ...
        * magnet_radius * magnetic_permeability / (4 * pi) * M;
end

dipol(lis3mdl_positions{1}, holes_positions_direction_magnet_faces_x{12, 1}, Radius, Height, Remanence, [-1, 0, 0]) * 1e6
biot_savat(lis3mdl_positions{1}, holes_positions_direction_magnet_faces_x{12, 1}, Radius, Height, Remanence) * 1e6


calibration_values_x = readtable("../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h26min51s_calibration_x.txt");
calibration_values_y = readtable("../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h39min41s_calibration_y.txt");
calibration_values_z = readtable("../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h45min44s_calibration_z.txt");

points = table
points = [holes_positions_direction_magnet_faces_x{1, 1}...
    holes_positions_direction_magnet_faces_x{1, 1}...
    holes_positions_direction_magnet_faces_x{10, 1}...
    holes_positions_direction_magnet_faces_x{10, 1}...
    holes_positions_direction_magnet_faces_x{10, 11}...
    holes_positions_direction_magnet_faces_x{10, 11}...
    holes_positions_direction_magnet_faces_x{1, 11}...
    holes_positions_direction_magnet_faces_x{1, 11}...
    holes_positions_direction_magnet_faces_x{5, 6}...
    holes_positions_direction_magnet_faces_x{5, 6}];

for i = 1:10
    calibration_values_x.x0
    
end



test_values = readtable("../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h32min12s_(1-10_3).txt");


%(res1 - no_influence2) * [0 1 0; -1 0 0; 0 0 -1]
%(res2 - no_influence2) * [0 1 0; -1 0 0; 0 0 -1]

dipol(lis3mdl_positions{1}, struct('x', 0e-3, 'y', 45e-3, 'z', 0), Radius, Height, Remanence, [-1, 0, 0]) * 1e6


